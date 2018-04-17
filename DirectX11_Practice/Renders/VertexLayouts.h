#pragma once

struct Vertex
{
	Vertex();
	Vertex(D3DXVECTOR3 position);

	D3DXVECTOR3 position;

	static D3D11_INPUT_ELEMENT_DESC desc[];
	static UINT count;
};

////////////////////////////////////

struct VertexColor
{
	VertexColor();
	VertexColor(D3DXVECTOR3 position, D3DXCOLOR color);

	D3DXVECTOR3 position;
	D3DXCOLOR color;

	static D3D11_INPUT_ELEMENT_DESC desc[];
	static UINT count;
};

////////////////////////////////////

struct VertexColorNormal
{
	VertexColorNormal();
	VertexColorNormal
	(
		D3DXVECTOR3 position
		, D3DXCOLOR color
		, D3DXVECTOR3 normal
	);

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR3 normal;

	static D3D11_INPUT_ELEMENT_DESC desc[];
	static UINT count;
};

////////////////////////////////////

struct VertexTexture
{
	VertexTexture();
	VertexTexture
	(
		D3DXVECTOR3 position
		, D3DXVECTOR2 uv
	);

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;

	static D3D11_INPUT_ELEMENT_DESC desc[];
	static UINT count;
};

////////////////////////////////////

struct VertexTextureNormal
{
	VertexTextureNormal();
	VertexTextureNormal
	(
		D3DXVECTOR3 position
		, D3DXVECTOR2 uv
		, D3DXVECTOR3 normal
	);

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;

	static D3D11_INPUT_ELEMENT_DESC desc[];
	static UINT count;
};

////////////////////////////////////

struct VertexColorTextureNormal
{
	VertexColorTextureNormal();
	VertexColorTextureNormal
	(
		D3DXVECTOR3 position
		, D3DXCOLOR color
		, D3DXVECTOR2 uv
		, D3DXVECTOR3 normal
	);

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;

	static D3D11_INPUT_ELEMENT_DESC desc[];
	static UINT count;
};