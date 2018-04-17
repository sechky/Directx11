#include "stdafx.h"
#include "MoLoader.h"
#include "MoModel.h"
#include "MoMaterial.h"
#include "MoModelUtility.h"

void MoLoader::Convert(string fbxFile, string saveFile)
{
	MoModel* model = NULL;
	MoLoader* loader = new MoLoader();
	model = loader->Load(fbxFile);
	SAFE_DELETE(loader);

	model->Write(saveFile);
	SAFE_DELETE(model);
}

void MoLoader::LoadBinary(string binFile, Model ** model)
{
	MoModel::Read(binFile, model);
}

void MoLoader::WriteBinary(string binFile, Model * model)
{
	MoModel::Write(binFile, model);
}

MoLoader::MoLoader()
{
	manager = FbxManager::Create();
	assert(manager != NULL);

	scene = FbxScene::Create(manager, "");
	assert(scene != NULL);
}

MoLoader::~MoLoader()
{
	scene->Destroy();
	manager->Destroy();
}

MoModel * MoLoader::Load(string file)
{
	int major, minor, revision;
	FbxManager::GetFileFormatVersion(major, minor, revision);

	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");
	bool check = importer->Initialize(file.c_str(), -1, ios);
	assert(check == true);

	importer->Import(scene);
	converter = new FbxGeometryConverter(manager);
	{
		model = new MoModel(file);

		ProcessMaterial();
		ProcessNode(scene->GetRootNode(), FbxNodeAttribute::eMesh);
	}
	SAFE_DELETE(converter);

	ios->Destroy();
	importer->Destroy();

	return model;
}

void MoLoader::ProcessMaterial()
{
	for (int i = 0; i < scene->GetMaterialCount(); i++)
	{
		FbxSurfaceMaterial* temp = scene->GetMaterial(i);
		materials.push_back(temp);

		MoMaterial* material = new MoMaterial(i, temp);
		model->PushMaterial(material);
	}
}

void MoLoader::ProcessNode(FbxNode * node, FbxNodeAttribute::EType type)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == type)
		{
			switch (nodeType)
			{
				case FbxNodeAttribute::eSkeleton:
					break;

				case FbxNodeAttribute::eMesh:
					ProcessMesh(node);
					break;

				case FbxNodeAttribute::eMarker:
				case FbxNodeAttribute::eNurbs:
				case FbxNodeAttribute::ePatch:
				case FbxNodeAttribute::eCamera:
				case FbxNodeAttribute::eLight:
				case FbxNodeAttribute::eNull:
					break;
			}//switch(nodeType)
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessNode(node->GetChild(i), type);
}

void MoLoader::ProcessMesh(FbxNode * node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	converter->Triangulate(attribute, true, true);

	FbxMesh* mesh = node->GetMesh();
	assert(mesh != NULL);

	int vertexCount = mesh->GetControlPointsCount();
	assert(vertexCount > 0);


	FbxVector4* controlPoints = mesh->GetControlPoints();
	for (int i = 0; i < mesh->GetPolygonCount(); i++)
	{
		MoMaterial* material = NULL;

		for (int vi = 0; vi < 3; vi++)
		{
			int vertexIndex = mesh->GetPolygonVertex(i, vi);
			if (vertexIndex < 0 || vertexIndex >= vertexCount)
				continue;

			if (material == NULL)
				material = LinkMaterialWithPolygon(mesh, 0, i, 0, vertexIndex);

			FbxVector4 fbxPos = controlPoints[vertexIndex];
			FbxVector4 fbxNormal;
			mesh->GetPolygonVertexNormal(i, vi, fbxNormal);
			fbxNormal.Normalize();

			D3DXVECTOR3 vecPos = MoModelUtility::ToVector3(fbxPos);
			D3DXVECTOR3 vecNormal = MoModelUtility::ToVector3(fbxNormal);

			D3DXVECTOR2 vecUv = GetUV(mesh, 0, i, vi, vertexIndex);
			model->PushVertex(material, vecPos, vecNormal, vecUv);
		}//for(vi)
	}//for(i)

	model->geometricOffset = GetGeometricOffset(node);
}

MoMaterial * MoLoader::LinkMaterialWithPolygon(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	if (layerIndex < 0 || layerIndex > mesh->GetLayerCount())
		return NULL;


	FbxNode* node = mesh->GetNode();
	if (node == NULL) return NULL;

	FbxLayerElementMaterial* fbxMaterial = mesh->GetLayer(layerIndex)->GetMaterials();
	if (fbxMaterial == NULL) return NULL;

	int mappingIndex = GetMappingIndex
	(
		fbxMaterial->GetMappingMode()
		, polygonIndex
		, 0
		, vertexIndex
	);
	if (mappingIndex < 0) return NULL;

	FbxLayerElement::EReferenceMode refMode = fbxMaterial->GetReferenceMode();
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			return GetMaterial(node->GetMaterial(mappingIndex));
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = fbxMaterial->GetIndexArray();

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				return GetMaterial(node->GetMaterial(tempIndex));
		}//if(mappingIndex)
	}//if(refMode)

	return NULL;
}

int MoLoader::GetMappingIndex(FbxLayerElement::EMappingMode mode, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	switch (mode)
	{
		case FbxLayerElement::eAllSame:
			return 0;
		case FbxLayerElement::eByControlPoint:
			return vertexIndex;
		case FbxLayerElement::eByPolygonVertex:
			return polygonIndex * 3 + polygonVertexIndex;
		case FbxLayerElement::eByPolygon:
			return polygonIndex;
		default:
			return -1;
	}
}

D3DXVECTOR2 MoLoader::GetUV(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	int layerCount = mesh->GetLayerCount();
	if (layerIndex >= layerCount) return D3DXVECTOR2();

	FbxLayer* layer = mesh->GetLayer(layerIndex);
	if (layer == NULL) return D3DXVECTOR2();

	FbxLayerElementUV* uv = layer->GetUVs(FbxLayerElement::eTextureDiffuse);
	if (uv == NULL) return D3DXVECTOR2();


	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	const FbxLayerElementArrayTemplate<FbxVector2>& uvArray = uv->GetDirectArray();
	const FbxLayerElementArrayTemplate<int>& uvIndexArray = uv->GetIndexArray();

	int mappingIndex = 0;
	if (mappingMode == FbxLayerElement::eByControlPoint)
	{
		mappingIndex = vertexIndex;

		if (refMode == FbxLayerElement::eDirect)
		{
			if (mappingIndex < uvArray.GetCount())
				return MoModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}
		else if (refMode == FbxLayerElement::eIndexToDirect)
		{
			if (mappingIndex < uvArray.GetCount())
			{
				int tempIndex = uvIndexArray.GetAt(mappingIndex);

				if (tempIndex < uvArray.GetCount())
					return MoModelUtility::ToUv(uvArray.GetAt(tempIndex));
			}
		}//if(refMode)
	}
	else if (mappingMode == FbxLayerElement::eByPolygonVertex)
	{
		mappingIndex = mesh->GetTextureUVIndex
		(
			polygonIndex
			, polygonVertexIndex
			, FbxLayerElement::eTextureDiffuse
		);

		switch (refMode)
		{
			case FbxLayerElement::eDirect:
			case FbxLayerElement::eIndexToDirect:
				if (mappingIndex < uvArray.GetCount())
					return MoModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}//switch(refMode)
	}//if(mappingMode)

	return D3DXVECTOR2();
}

MoMaterial * MoLoader::GetMaterial(FbxSurfaceMaterial * fbxMaterial)
{
	for (size_t i = 0; i < materials.size(); i++)
	{
		if (materials[i] == fbxMaterial)
			return model->materials[i];			
	}

	return NULL;
}

D3DXMATRIX MoLoader::GetGeometricOffset(FbxNode * node)
{
	if (node == NULL)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);

		return matrix;
	}

	FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxMatrix offset;
	offset.SetIdentity();
	offset.SetTRS(T, R, S);

	return MoModelUtility::ToMatrix(offset);
}
