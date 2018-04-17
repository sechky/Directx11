#include "stdafx.h"
#include "ModelMaterial.h"
#include "ModelBuffer.h"

ModelMaterial::ModelMaterial()
	: number(-1)
	, ambientFile(""), emissiveFile(""), diffuseFile(""), specularFile("")
	, ambientView(NULL), diffuseView(NULL), specularView(NULL), emissiveView(NULL)
	, shader(NULL), shaderFile("")
{
	shaderBuffer = new ModelBuffer();
}

ModelMaterial::~ModelMaterial()
{
	SAFE_RELEASE(ambientView);
	SAFE_RELEASE(emissiveView);
	SAFE_RELEASE(diffuseView);
	SAFE_RELEASE(specularView);

	SAFE_DELETE(shaderBuffer);
	SAFE_DELETE(shader);
}


void ModelMaterial::CreateTexture(string file, ID3D11ShaderResourceView ** view)
{
	DWORD fileValue = GetFileAttributesA(file.c_str());
	if (fileValue < 0xFFFFFFFF)
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFileA
		(
			D3D::GetDevice()
			, file.c_str()
			, NULL
			, NULL
			, view
			, NULL
		);
		assert(SUCCEEDED(hr));
	}
}

void ModelMaterial::SetShader(string file)
{
	SAFE_DELETE(shader);

	shaderFile = file;

	if (shaderFile == "0")
	{
		shader = new Shader(Shaders + L"Model.hlsl");
	}
	else
	{
		wstring temp = String::StringToWString(file);
		shader = new Shader(temp);
	}

	shader->CreateInputLayout(VertexTextureNormal::desc, VertexTextureNormal::count);
}

void ModelMaterial::SetAmbient(D3DXCOLOR & color)
{
	shaderBuffer->SetAmbient(color);
}

void ModelMaterial::SetDiffuse(D3DXCOLOR & color)
{
	shaderBuffer->SetDiffuse(color);
}

void ModelMaterial::SetSpecular(D3DXCOLOR & color)
{
	shaderBuffer->SetSpecular(color);
}

void ModelMaterial::SetEmissive(D3DXCOLOR & color)
{
	shaderBuffer->SetEmissive(color);
}

void ModelMaterial::SetAmbientTexture(string file)
{
	SAFE_RELEASE(ambientView);

	ambientFile = file;
	CreateTexture(file, &ambientView);
}

void ModelMaterial::SetDiffuseTexture(string file)
{
	SAFE_RELEASE(diffuseView);

	diffuseFile = file;
	CreateTexture(file, &diffuseView);
}

void ModelMaterial::SetSpecularTexture(string file)
{
	SAFE_RELEASE(specularView);

	specularFile = file;
	CreateTexture(file, &specularView);
}

void ModelMaterial::SetEmissiveTexture(string file)
{
	SAFE_RELEASE(emissiveView);

	emissiveFile = file;
	CreateTexture(file, &emissiveView);
}

void ModelMaterial::SetPSBuffer()
{
	if (diffuseView == NULL)
	{
		ID3D11ShaderResourceView* n[1]{ NULL };
		D3D::GetDC()->PSSetShaderResources(0, 1, n);
	}
	else
		D3D::GetDC()->PSSetShaderResources(0, 1, &diffuseView);

	shaderBuffer->SetPSBuffer(1);
	shader->Render();
}
