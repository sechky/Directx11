#pragma once

class MoModel;
class MoMaterial;
class Model;
class ModelMesh;
class MoModelMesh
{
public:
	MoMaterial* GetMaterial() { return material; }

private:
	friend class MoLoader;
	friend class MoModel;

	MoModelMesh(MoModel* model, MoMaterial* material);
	~MoModelMesh();

	void PushVertex(D3DXVECTOR3& position, D3DXVECTOR3& normal, D3DXVECTOR2& uv);
	
	void Write(BinaryWriter* w);
	static void Read(BinaryReader* r, Model* model, ModelMesh* modelMesh);
	static void Write(BinaryWriter* w, ModelMesh* modelMesh);

	MoModel* model;
	MoMaterial* material;

	vector<VertexTextureNormal> vertices;
	vector<UINT> indices;
};
