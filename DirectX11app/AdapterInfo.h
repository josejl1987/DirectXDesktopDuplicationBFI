#pragma once
#include <string>
#include <vector>
#include "OutputInfo.h"
struct AdapterInfo
{
	std::string Name;
	std::vector<OutputInfo> Outputs;
	void Initialize(IDXGIAdapter1* adapter);
};