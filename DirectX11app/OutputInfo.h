#pragma once
#include <string>
#include <DXGI.h>

struct OutputInfo
{
	std::string DeviceName;
	IDXGIOutput* Output;
	int Index;
	void Initialize(IDXGIOutput* out, int index);
};

