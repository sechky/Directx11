#include "stdafx.h"
#include "D3D.h"

D3D* D3D::instance = NULL;

D3DDesc D3D::d3dDesc;
ID3D11Device* D3D::device = NULL;
ID3D11DeviceContext* D3D::deviceContext = NULL;
IDXGISwapChain* D3D::swapChain = NULL;

D3D * D3D::Get()
{
	//assert(instance != NULL);

	return instance;
}

void D3D::Create()
{
	assert(instance == NULL);

	instance = new D3D();
}

void D3D::Delete()
{
	SAFE_DELETE(instance);
}

void D3D::SetRenderTarget(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv)
{
	if (rtv == NULL)
		rtv = renderTargetView;

	if (dsv == NULL)
		dsv = depthStencilView;

	D3D::GetDC()->OMSetRenderTargets(1, &rtv, dsv);
}

void D3D::Clear(D3DXCOLOR color, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	if (rtv == NULL)
		rtv = renderTargetView;

	if (dsv == NULL)
		dsv = depthStencilView;

	deviceContext->ClearRenderTargetView(rtv, color);
	deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1, 0);
}

void D3D::Present()
{
	swapChain->Present(d3dDesc.bVsync == true ? 1 : 0, 0);
}

void D3D::ResizeScreen(float width, float height)
{
	//리사이즈를 할 경우 백퍼퍼를 지우고 새로 만들어 줘야한다.
	//TODO :: 다시 듣기...

	d3dDesc.Width = width;
	d3dDesc.Height = height;

	DeleteBackBuffer();//release시킨다.
	{
		HRESULT hr = swapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0);//unknown으로하면 기본 설정값 사용
		assert(SUCCEEDED(hr));
	}
	CreateBackBuffer(width, height);
}

D3D::D3D()
	: version(D3D_FEATURE_LEVEL_11_0)
	, numerator(0), denominator(1)
{
	SetGpuInfo();

	CreateSwapChainAndDevice();
	CreateBackBuffer(d3dDesc.Width, d3dDesc.Height);
}

D3D::~D3D()
{
	DeleteBackBuffer();

	if (swapChain != NULL)
		swapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(deviceContext);
	SAFE_RELEASE(device);
	SAFE_RELEASE(swapChain);
}

void D3D::SetGpuInfo()
{
	HRESULT hr;
	
	IDXGIFactory* factory;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory);
	assert(SUCCEEDED(hr));

	IDXGIAdapter* adapter;
	hr = factory->EnumAdapters(0, &adapter);
	assert(SUCCEEDED(hr));

	IDXGIOutput* adapterOutput;
	hr = adapter->EnumOutputs(0, &adapterOutput);
	assert(SUCCEEDED(hr));

	UINT modeCount;
	hr = adapterOutput->GetDisplayModeList
	(
		DXGI_FORMAT_R8G8B8A8_UNORM
		, DXGI_ENUM_MODES_INTERLACED
		, &modeCount
		, NULL
	);
	assert(SUCCEEDED(hr));

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[modeCount];
	hr = adapterOutput->GetDisplayModeList
	(
		DXGI_FORMAT_R8G8B8A8_UNORM
		, DXGI_ENUM_MODES_INTERLACED
		, &modeCount
		, displayModeList
	);
	assert(SUCCEEDED(hr));

	//device와 swapchain을 동시생성할 경우 adapter를 Mode 대신 넣어주면 된다.

	for (UINT i = 0; i < modeCount; i++)
	{
		bool isCheck = true;
		isCheck &= displayModeList[i].Width == d3dDesc.Width;
		isCheck &= displayModeList[i].Height == d3dDesc.Height;

		if (isCheck == true)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	hr = adapter->GetDesc(&adapterDesc);
	assert(SUCCEEDED(hr));

	gpuMemorySize = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
	gpuDescription = adapterDesc.Description;
	
	SAFE_DELETE_ARRAY(displayModeList);

	
	SAFE_RELEASE(adapterOutput);
	SAFE_RELEASE(adapter);
	SAFE_RELEASE(factory);
}

void D3D::CreateSwapChainAndDevice()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = (UINT)d3dDesc.Width;
	swapChainDesc.BufferDesc.Height = (UINT)d3dDesc.Height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (d3dDesc.bVsync == true)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = d3dDesc.Handle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !d3dDesc.bFullScreen;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		NULL
		, D3D_DRIVER_TYPE_HARDWARE
		, NULL
		, 0
		, &version
		, 1
		, D3D11_SDK_VERSION
		, &swapChainDesc
		, &swapChain
		, &device
		, NULL
		, &deviceContext
	);
	assert(SUCCEEDED(hr));
}

void D3D::CreateBackBuffer(float width, float height)
{
	HRESULT hr;

	//Backbuffer
	{
		//스왑체인을 가리키는 포인터를 가져올 변수
		ID3D11Texture2D* backbufferPointer;		
		//buffer, the type of interface, a pointer to a back-buffer interface
		//buffer(여러개 가능)의 스왑체인을 가리키는 포인터를 가져온다.
		//포인터의 포인터를 void**로 캐스팅해야한다.
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbufferPointer);//com의 참조횟수가 증가됨.
		assert(SUCCEEDED(hr));

		//위에서 가져온 buffer를 이용하여 사용 할 rendertargetview를 생성한다.
		hr = D3D::GetDevice()->CreateRenderTargetView(backbufferPointer, NULL, &renderTargetView);
		assert(SUCCEEDED(hr));

		SAFE_RELEASE(backbufferPointer);
	}

	//Depth * Stencil Buffer
	{
		//depth*stencil buffer를 저장할 2차원 텍스쳐 정보
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		//Depthstencil 포함
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//MultiSampling
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &backBuffer);//2번째 매개변수로 초기자료를 지정할 데이터를 가리킬 수 있다.
		assert(SUCCEEDED(hr));
	}
		
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		hr = D3D::GetDevice()->CreateDepthStencilView(backBuffer, &desc, &depthStencilView);//2번째 매개변수로 자료 형식을 정할 수 있다.
		assert(SUCCEEDED(hr));

		SetRenderTarget(renderTargetView, depthStencilView);
	}	
}

void D3D::DeleteBackBuffer()
{
	SAFE_RELEASE(depthStencilView);
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(backBuffer);
}
