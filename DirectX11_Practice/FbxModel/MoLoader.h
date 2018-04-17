#pragma once

class MoModel;
class MoMaterial;
class MoLoader
{
public:
	static void Convert(string fbxFile, string saveFile);
	static void LoadBinary(string binFile, Model** model);
	static void WriteBinary(string binFile, Model* model);
	
private:
	MoLoader();
	~MoLoader();

	MoModel* Load(string file);

	void ProcessMaterial();
	void ProcessNode(FbxNode* node, FbxNodeAttribute::EType type);
	void ProcessMesh(FbxNode* node);

	MoMaterial* LinkMaterialWithPolygon
	(
		FbxMesh* mesh, int layerIndex
		, int polygonIndex, int polygonVertexIndex
		, int vertexIndex
	);

	int GetMappingIndex
	(
		FbxLayerElement::EMappingMode mode
		, int polygonIndex
		, int polygonVertexIndex
		, int vertexIndex
	);
	
	D3DXVECTOR2 GetUV
	(
		FbxMesh* mesh, int layerIndex
		, int polygonIndex, int polygonVertexIndex
		, int vertexIndex
	);

	MoMaterial* GetMaterial(FbxSurfaceMaterial* fbxMaterial);
	D3DXMATRIX GetGeometricOffset(FbxNode* node);

	MoModel* model;
	FbxManager* manager;
	FbxScene* scene;
	FbxImporter* importer;
	FbxGeometryConverter* converter;

	vector<FbxSurfaceMaterial *> materials;
};
