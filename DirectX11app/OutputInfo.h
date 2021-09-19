#pragma once
#include <string>
#include <dxgi.h>

struct OutputInfo
{
	std::string DeviceName;
	IDXGIOutput* Output;
	int Index;
	void Initialize(IDXGIOutput* out, int index);
};

