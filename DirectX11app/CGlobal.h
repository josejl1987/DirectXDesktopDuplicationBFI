#pragma once
#include "DisplayInfo.h"
#include <windows.h>
#include <memory>
struct D3D11RenderManager;
struct DisplayInfo;
struct CGlobal {
	int intervalBFI = 0;
	DisplayInfo displayInfo;
	int initIndex = 0;
	HWND hWnd ;
	std::shared_ptr<D3D11RenderManager> render;
	void RenderDisplayInfo();
};


