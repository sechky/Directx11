#include "stdafx.h"
#include "Window.h"
#include "../Program.h"

Program* Window::program = NULL;

Window::Window()
{
	//winmain에서 정의되는 desc정보를 이용하여 윈도우 창을 만들고 handle값을 반환 받기
	D3DDesc desc;
	D3D::GetDesc(&desc);

	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.Instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	if (desc.bFullScreen == true)
	{
		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)desc.Width;
		devMode.dmPelsHeight = (DWORD)desc.Height;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}

	desc.Handle = CreateWindowEx
	(
		WS_EX_APPWINDOW
		, desc.AppName.c_str()
		, desc.AppName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, desc.Instance
		, NULL
	);
	assert(desc.Handle != NULL);
	D3D::SetDesc(desc);


	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		desc.Handle
		, centerX, centerY
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);
	SetFocus(desc.Handle);

	ShowCursor(true);
}

Window::~Window()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (desc.bFullScreen == true)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(desc.Handle);

	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

WPARAM Window::Run()
{
	MSG msg = { 0 };

	D3DDesc desc;
	D3D::GetDesc(&desc);


	D3D::Create();
	Keyboard::Create();
	Mouse::Create();

	Time::Create();
	Time::Get()->Start();

	ImGui::Create(desc.Handle, D3D::GetDevice(), D3D::GetDC());
	ImGui::StyleColorsDark();

	program = new Program();

	//TODO :: 기존과 비교

	while(true)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Time::Get()->Update();

			if(ImGui::IsMouseHoveringAnyWindow() == false)
			{
				Keyboard::Get()->Update();
				Mouse::Get()->Update();
			}

			program->Update();
			ImGui::Update();


			program->PreRender();
			D3D::Get()->Clear();//기본NULL이면 기본 렌더
			{

				ImGuiWindowFlags flag = 0;
				flag |= ImGuiWindowFlags_NoTitleBar;
				flag |= ImGuiWindowFlags_NoMove;
				flag |= ImGuiWindowFlags_NoResize;

				ImGui::SetNextWindowPos(ImVec2(0, 0));
				ImGui::SetNextWindowSize(ImVec2(100, 0));
				ImGui::SetNextWindowBgAlpha(0);

				ImGui::Begin("Fps Window", NULL, flag);
				ImGui::Text("Fps %d", (int)ImGui::GetIO().Framerate);
				ImGui::End();

				program->Render();
				program->PostRender();//imgui렌더
				ImGui::Render();
			}
			D3D::Get()->Present();
		}
	}
	SAFE_DELETE(program);

	ImGui::Delete();
	Time::Delete();
	Mouse::Delete();
	Keyboard::Delete();
	D3D::Delete();

	return msg.wParam;
}

LRESULT CALLBACK Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	Mouse::Get()->InputProc(message, wParam, lParam);

	if (ImGui::WndProc((UINT*)handle, message, wParam, lParam))
		return true;

	//윈도우가 resize될시 이벤트를 받는다.
	//resize범위가 정해질 경우에만(원래는 리사이즈중에는 작동안되도록 세팅하려했는데)이벤트를 받는듯
	if (message == WM_SIZE)
	{
		ImGui::Invalidate();
		{
			if (program != NULL)
			{
				float width = (float)LOWORD(lParam);
				float height = (float)HIWORD(lParam);

				if (D3D::Get() != NULL)
					D3D::Get()->ResizeScreen(width, height);
				
				program->ResizeScreen();
			}		
		}
		ImGui::Validate();
	}

	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

