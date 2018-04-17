#include "stdafx.h"
#include "MoModel.h"
#include "MoMaterial.h"
#include "MoModelMesh.h"
#include "../Model/Model.h"
#include "../Model/ModelMaterial.h"
#include "../Model/ModelMesh.h"

MoModel::MoModel(string file)
	: file(file)
{
	D3DXMatrixIdentity(&geometricOffset);
}

MoModel::~MoModel()
{
	for (MoMaterial* material : materials)
		SAFE_DELETE(material);
}

void MoModel::PushMaterial(MoMaterial * material)
{
	materials.push_back(material);
}

void MoModel::PushVertex(MoMaterial * material, D3DXVECTOR3 & position, D3DXVECTOR3 & normal, D3DXVECTOR2 & uv)
{
	bool bNew = true;

	for (MoModelMesh* mesh : meshes)
	{
		if (material == mesh->GetMaterial())
		{
			mesh->PushVertex(position, normal, uv);
			bNew = false;
		}
	}

	if (bNew == true)
	{
		MoModelMesh* mesh = new MoModelMesh(this, material);
		mesh->PushVertex(position, normal, uv);

		meshes.push_back(mesh);
	}
}

void MoModel::Write(string file)
{
	BinaryWriter* w = new BinaryWriter();
	wstring temp = String::StringToWString(file);

	w->Open(temp);
	{
		w->Matrix(geometricOffset);

		w->UInt(materials.size());
		for (MoMaterial* material : materials)
			material->Write(w);

		w->UInt(meshes.size());
		for (MoModelMesh* mesh : meshes)
			mesh->Write(w);
	}
	w->Close();

	SAFE_DELETE(w);
}

void MoModel::Read(string file, Model ** model)
{
	BinaryReader* r = new BinaryReader();
	wstring temp = String::StringToWString(file);
	
	UINT count = 0;
	r->Open(temp);
	{
		*model = new Model();
		(*model)->matGeometricOffset = r->Matrix();

		count = r->UInt();
		for (UINT i = 0; i < count; i++)
		{		
			ModelMaterial* material = new ModelMaterial();
			MoMaterial::Read(r, material);

			(*model)->materials.push_back(material);
		}

		count = r->UInt();
		for (UINT i = 0; i < count; i++)
		{
			ModelMesh* mesh = new ModelMesh(*model);
			MoModelMesh::Read(r, *model, mesh);

			(*model)->meshes.push_back(mesh);
		}
	}
	r->Close();

	SAFE_DELETE(r);
}

void MoModel::Write(string file, Model * model)
{
	BinaryWriter* w = new BinaryWriter();
	wstring temp = String::StringToWString(file);

	w->Open(temp);
	{
		w->Matrix(model->matGeometricOffset);

		w->UInt(model->materials.size());
		for (ModelMaterial* material : model->materials)
			MoMaterial::Write(w, material);

		w->UInt(model->meshes.size());
		for (ModelMesh* mesh : model->meshes)
			MoModelMesh::Write(w, mesh);
	}
	w->Close();

	SAFE_DELETE(w);
}
