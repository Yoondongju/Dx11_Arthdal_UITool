#pragma once

#include <process.h>


namespace Client
{
	const unsigned int			g_iWinSizeX = { 1600 };
	const unsigned int			g_iWinSizeY = { 900 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_MAPTOOL ,LEVEL_GAMEPLAY, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;
