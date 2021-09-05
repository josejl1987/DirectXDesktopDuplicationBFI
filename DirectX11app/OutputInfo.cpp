#include "AdapterInfo.h"
void OutputInfo::Initialize(IDXGIOutput* out, int index)
{
	DXGI_OUTPUT_DESC desc;
	out->GetDesc(&desc);
	std::wstring ws(desc.DeviceName);
	MONITORINFOEXA mInfo;
	GetMonitorInfoA(desc.Monitor, &mInfo);
	DeviceName = std::string(ws.begin(), ws.end());
	Index = index;
	Output = out;
}
