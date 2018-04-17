#pragma once

class ModelMaterial;
class ModelMesh;
class Model
{
public:
	~Model();

	D3DXMATRIX GetGeometricOffset() { return matGeometricOffset; }
	ModelMaterial* GetMatchMaterial(UINT number);
	
	ModelMaterial* GetMaterial(UINT number)
	{
		return materials[number];
	}

	UINT GetMaterialCount() { return materials.size(); }


	void Update();
	void Render(Camera* camera);

private:
	friend class MoModel;

	Model();

	D3DXMATRIX matGeometricOffset;

	vector<ModelMaterial *> materials;
	vector<ModelMesh *> meshes;
};