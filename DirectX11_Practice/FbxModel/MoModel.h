#pragma once

class MoMaterial;
class MoModelMesh;
class Model;
class MoModel
{
private:
	friend class MoLoader;

	MoModel(string file);
	~MoModel();

	void PushMaterial(MoMaterial* material);
	void PushVertex
	(
		MoMaterial* material
		, D3DXVECTOR3& position
		, D3DXVECTOR3& normal
		, D3DXVECTOR2& uv
	);

	void Write(string file);
	static void Read(string file, Model** model);
	static void Write(string file, Model* model);

	string file;
	D3DXMATRIX geometricOffset;

	vector<MoMaterial *> materials;
	vector<MoModelMesh* > meshes;
};