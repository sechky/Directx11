#include "stdafx.h"
#include "Sky.h"
#include "../Utilities/Mesh.h"

Sky::Sky()
	: meshFile(Contents + L"Meshes/Sphere.data")
	, shaderFile(Shaders + L"Sky.hlsl")
	, shader(NULL)
{
	VertexType* vertex = NULL;
	UINT* index = NULL;

	Mesh mesh;
	mesh.Open(meshFile);
	mesh.CopyVertex(&vertex, &vertexCount);
	mesh.CopyIndex(&index, &indexCount);
	
	
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = vertex;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));


	desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data = { 0 };
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(vertex);
	SAFE_DELETE_ARRAY(index);

	
	D3DXMatrixIdentity(&world);
	
	skyBuffer = new SkyBuffer();
	worldBuffer = new WorldBuffer();

	shader = new Shader(shaderFile);

	
	D3D11_RASTERIZER_DESC rasterizerDesc;
	States::GetRasterizerDesc(&rasterizerDesc);//기본세팅 desc를 가져온다
	rasterizerDesc.FrontCounterClockwise = false;//이건 고칠필요가 없지만 어떤 특징을 가졌는지 보기 좋도록 
	States::CreateRasterizer(&rasterizerDesc, &clockState);//생성한다.

	States::GetRasterizerDesc(&rasterizerDesc);
	rasterizerDesc.FrontCounterClockwise = true;//반시계 렌더(sky내부랜더를 위해)
	States::CreateRasterizer(&rasterizerDesc, &countClockState);

	D3D11_DEPTH_STENCIL_DESC depthDesc;
	States::GetDepthStencilDesc(&depthDesc);
	depthDesc.DepthEnable = true;
	States::CreateDepthStencil(&depthDesc, &depthOnState);

	States::GetDepthStencilDesc(&depthDesc);
	depthDesc.DepthEnable = false;
	States::CreateDepthStencil(&depthDesc, &depthOffState);
}

Sky::~Sky()
{
	SAFE_RELEASE(depthOnState);
	SAFE_RELEASE(depthOffState);
	SAFE_RELEASE(clockState);
	SAFE_RELEASE(countClockState);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(skyBuffer);
	SAFE_DELETE(shader);
}

void Sky::Update(Camera* camera)
{
	D3DXVECTOR3 position;
	camera->GetPosition(&position);

	D3DXMatrixTranslation(&world, position.x, position.y, position.z);
	worldBuffer->SetMatrix(world);
}

void Sky::Render()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);
	skyBuffer->SetPSBuffer(1);
	shader->Render();

	//sky는 내부를 그려야한다.
	States::SetDepthStencil(depthOffState);
	States::SetRasterizer(countClockState);//반시계 렌더
	{
		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	}
	States::SetRasterizer(clockState);//시계 렌더 복구
	States::SetDepthStencil(depthOnState);
}

void Sky::PostRender()
{
	ImGui::Begin("Environment");

	if (ImGui::CollapsingHeader("Sky"))
	{
		//이름이 같으면 그룹으로 묶어주자
		ImGui::ColorEdit3("Center", (float *)&skyBuffer->Data.Center);
		ImGui::ColorEdit3("Apex", (float *)&skyBuffer->Data.Apex);
	}

	ImGui::End();
}
