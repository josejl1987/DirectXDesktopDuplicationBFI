#include "DisplayInfo.h"

void DisplayInfo::Initialize(IDXGIFactory1* factory)
{
	IDXGIAdapter1* adapter;
	int i = 0;
	while (factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		AdapterInfo ai;
		DXGI_ADAPTER_DESC1 desc;
		ai.Initialize(adapter);
		Adapters.push_back(ai);
		i++;
	}
}
