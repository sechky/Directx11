#include "stdafx.h"
#include "ModelMesh.h"
#include "Model.h"
#include "ModelMaterial.h"

ModelMesh::ModelMesh(Model * model)
	: model(model)
	, material(NULL)
	, vertexData(NULL), indexData(NULL), vertexBuffer(NULL), indexBuffer(NULL)
	, vertexCount(0), indexCount(0)
{
	D3DXMatrixIdentity(&world);
}

ModelMesh::~ModelMesh()
{
	SAFE_DELETE_ARRAY(vertexData);
	SAFE_DELETE_ARRAY(indexData);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelMesh::Update()
{
	world = model->GetGeometricOffset();
}

void ModelMesh::Render(Camera * camera)
{
	if (vertexBuffer == NULL) return;
	if (indexBuffer == NULL) return;
	if (material == NULL) return;
	if (material->CanRender() == false) return;

	
	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->SetPSBuffer();
	camera->SetVSBuffer(&world);

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ModelMesh::CreateBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	if (vertexBuffer != NULL)
		SAFE_RELEASE(vertexBuffer);

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexTextureNormal) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));


	//2. Index Buffer
	if (indexBuffer != NULL)
		SAFE_RELEASE(indexBuffer);

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = indexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}
