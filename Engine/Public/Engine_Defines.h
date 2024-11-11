#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <time.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\ScreenGrab.h"

#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"

#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

using namespace DirectX;

/* d3dx11에 포함되어있었지만 너무 무겁다. */
/* 기존 d3dx11포함되어있었던 다양한 기능을 제공하는 헤더 + 라이브러리 파일을 선택적으로 추가한다. */
//#include <d3dx11.h>

#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>



using namespace std;

#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"

namespace Engine
{
	const _wstring		g_strTransformTag = TEXT("Com_Transform");
	const _uint			g_iMaxMeshBones = 512;

	enum MOUSEKEYSTATE { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };	
}

using namespace Engine;


#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "ImGuizmo.h"

#define EPSILON 0.01f


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


