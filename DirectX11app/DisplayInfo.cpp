#include "DisplayInfo.h"

void DisplayInfo::Initialize(IDXGIFactory1* factory)
{
	IDXGIAdapter1* adapter;
	int i = 0;
	Adapters.clear();
	while (factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		AdapterInfo ai;
		ai.Initialize(adapter);
		Adapters.push_back(ai);
		i++;
	}
}
