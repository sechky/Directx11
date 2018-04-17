#include "stdafx.h"
#include "MoModelMesh.h"
#include "MoModel.h"
#include "MoMaterial.h"
#include "../Model/Model.h"
#include "../Model/ModelMesh.h"
#include "../Model/ModelMaterial.h"

MoModelMesh::MoModelMesh(MoModel * model, MoMaterial * material)
	: model(model), material(material)
{

}

MoModelMesh::~MoModelMesh()
{
}

void MoModelMesh::PushVertex(D3DXVECTOR3 & position, D3DXVECTOR3 & normal, D3DXVECTOR2 & uv)
{
	VertexTextureNormal vertex;
	vertex.position = position;
	vertex.uv = uv;
	vertex.normal = normal;
	vertices.push_back(vertex);

	indices.push_back((UINT)indices.size());
}

void MoModelMesh::Write(BinaryWriter * w)
{
	UINT materialNumber = material->GetNumber();
	w->UInt(materialNumber);

	w->UInt(vertices.size());
	w->Byte(&vertices[0], sizeof(VertexTextureNormal) * vertices.size());

	w->UInt(indices.size());
	w->Byte(&indices[0], sizeof(UINT) * indices.size());
}

void MoModelMesh::Read(BinaryReader * r, Model * model, ModelMesh * modelMesh)
{
	UINT materialNumber = r->UInt();
	ModelMaterial* material = model->GetMatchMaterial(materialNumber);

	modelMesh->SetMaterial(material);

	
	modelMesh->vertexCount = r->UInt();
	modelMesh->vertexData = new VertexTextureNormal[modelMesh->vertexCount];
	r->Byte((void **)&modelMesh->vertexData, sizeof(VertexTextureNormal) * modelMesh->vertexCount);


	modelMesh->indexCount = r->UInt();
	modelMesh->indexData = new UINT[modelMesh->indexCount];
	r->Byte((void **)&modelMesh->indexData, sizeof(UINT) * modelMesh->indexCount);

	modelMesh->CreateBuffer();
}

void MoModelMesh::Write(BinaryWriter * w, ModelMesh * modelMesh)
{
	UINT materialNumber = modelMesh->material->GetNumber();
	w->UInt(materialNumber);

	w->UInt(modelMesh->vertexCount);
	w->Byte(modelMesh->vertexData, sizeof(VertexTextureNormal) * modelMesh->vertexCount);

	w->UInt(modelMesh->indexCount);
	w->Byte(modelMesh->indexData, sizeof(UINT) * modelMesh->indexCount);
}
