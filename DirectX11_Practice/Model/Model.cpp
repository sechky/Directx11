#include "stdafx.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "ModelMesh.h"
#include "../Editor.h"

Model::Model()
{
}

Model::~Model()
{
	for (ModelMaterial* material : materials)
		SAFE_DELETE(material);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);
}

ModelMaterial * Model::GetMatchMaterial(UINT number)
{
	for (ModelMaterial* material : materials)
	{
		if (material->GetNumber() == number)
			return material;
	}

	return NULL;
}

void Model::Update()
{
	for (ModelMesh* mesh : meshes)
		mesh->Update();
}

void Model::Render(Camera * camera)
{
	for (ModelMesh* mesh : meshes)
		mesh->Render(camera);
}


///////////////////////////////////////////////////////////////////////////


DLL UINT Cs_GetMaterialCount()
{
	return Editor::Get()->GetModel()->GetMaterialCount();
}

DLL const char* Cs_GetMaterialName(UINT number)
{
	ModelMaterial* material = Editor::Get()->GetModel()->GetMaterial(number);
	return String::ToCSharp(material->GetName());
}

DLL const char* Cs_GetMaterialDiffuseTextureFile(UINT number)
{
	ModelMaterial* material = Editor::Get()->GetModel()->GetMaterial(number);
	return String::ToCSharp(material->GetDiffuseFile());
}

DLL void Cs_SetMaterialDiffuseTextureFile(UINT number, const char* file)
{
	ModelMaterial* material = Editor::Get()->GetModel()->GetMaterial(number);
	material->SetDiffuseTexture(file);
}