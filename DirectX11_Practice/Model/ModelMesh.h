#pragma once

class Model;
class ModelMaterial;
class ModelMesh
{
public:
	void SetMaterial(ModelMaterial* material) { this->material = material; }

private:
	friend class MoModel;
	friend class MoModelMesh;
	friend class Model;


	ModelMesh(Model* model);
	~ModelMesh();

	void Update();
	void Render(Camera* camera);

	void CreateBuffer();

	Model* model;
	ModelMaterial* material;

	VertexTextureNormal* vertexData;
	UINT* indexData;

	ID3D11Buffer* vertexBuffer, *indexBuffer;
	UINT vertexCount, indexCount;

	D3DXMATRIX world; 

};
