#pragma once
#include "AdapterInfo.h"

struct DisplayInfo
{
	std::vector<AdapterInfo> Adapters;
	void Initialize(IDXGIFactory1* factory);

};

