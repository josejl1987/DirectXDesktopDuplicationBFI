#pragma once
#include "DisplayInfo.h"
#include <windows.h>
#include <memory>
#include "imgui/imgui.h"
#include "imgui/misc/imguifilebrowser.h"
struct D3D11RenderManager;
struct DisplayInfo;
struct Preset;
struct  RenderTarget ;
struct CGlobal {
	int intervalBFI = 0;
	DisplayInfo displayInfo;
	int initIndex = 0;
	HWND hWnd ;
	RenderTarget* desktopRT;
	std::shared_ptr<D3D11RenderManager> render;
	std::shared_ptr<Preset> currentPreset;
	ImGui::FileBrowser fileDialog;
	void RenderShaderList();
	void RenderDisplayInfo();
	int frameCount;
};


