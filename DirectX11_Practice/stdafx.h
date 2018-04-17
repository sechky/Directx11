#pragma once

#include <Windows.h>
#include <assert.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>
using namespace std;

#include <atlbase.h>
#include <atlconv.h>

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")//쉐이더 생성할때 자동으로 생성하기위한 함수 shaderReflection을 사용하기 위해추가

#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p){ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }

const wstring Contents = L"../_Contents/";
const wstring Shaders = L"../_Shaders/";

#include "./Systems/D3D.h"
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"

#include "./Renders/VertexLayouts.h"
#include "./Renders/States.h"
#include "./Renders/Shader.h"
#include "./Renders/ShaderBuffer.h"
#include "./Renders/GlobalBuffers.h"

#include "./Viewer/Camera.h"

#include "./Utilities/BinaryFile.h"
#include "./Utilities/String.h"
#include "./Utilities/d3dx11effect.h"

//#include "./Model/Model.h"