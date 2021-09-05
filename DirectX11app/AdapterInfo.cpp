#include "AdapterInfo.h"

void AdapterInfo::Initialize(IDXGIAdapter1* adapter)
{
	UINT i = 0;
	IDXGIOutput* pOutput;
	std::vector<DXGI_OUTPUT_DESC> descList;
	DXGI_ADAPTER_DESC1 adapterDesc;
	adapter->GetDesc1(&adapterDesc);
	std::wstring ws(adapterDesc.Description);
	this->Name = std::string(ws.begin(), ws.end());
	while (adapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND)
	{
		OutputInfo outputInfo;
		outputInfo.Initialize(pOutput,i);
		this->Outputs.push_back(outputInfo);
		++i;
	}
}
