#pragma once

#include <process.h>
#include "Client_Enum.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	float g_fRack = { 0.f };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;


#define DEFAULT_DISTANCE 30.f
