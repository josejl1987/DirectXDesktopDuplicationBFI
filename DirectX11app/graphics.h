// #include <WICTextureLoader.h>
//#include <ScreenGrab.h>
//#include <DirectXTex.h>

#include <directxmath.h>
#include <d3d11_2.h>
#include "PixelShader.h"
#include "VertexShader.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"
#include <vector>
#include <string>
#include "DisplayInfo.h"
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")

using namespace DirectX;

IDXGISwapChain* swapchain;
ID3D11Device* device;
ID3D11DeviceContext* deviceContext;
ID3D11RenderTargetView* backbuffer;

IDXGIOutputDuplication* outputDuplication;
ID3D11Texture2D* acquiredDesktopImage;

ID3D11VertexShader* VertexShader;
ID3D11PixelShader* PixelShader;
ID3D11InputLayout* InputLayout;
ID3D11Buffer* VertexBuffer;

ID3D11RenderTargetView* renderTargetView;
ID3D11Texture2D* renderTargetTexture;
ID3D11ShaderResourceView* renderTargetResourceView;
ID3D11SamplerState* pointSamplerState;

typedef struct _VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
} VERTEX;
const int ox = 1920;
const int oy = 1080;

#define NUMVERTICES 6
UINT Stride = sizeof(VERTEX);
UINT Offset = 0;
VERTEX Vertices[NUMVERTICES] =
{
	{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
};

D3D11_VIEWPORT renderTargetViewport;
D3D11_VIEWPORT windowViewport;

ID3D11ShaderResourceView* desktopResourceView;
ID3D11Texture2D* desktopTexture;
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
   if(x != NULL)        \
   {                    \
      x->Release();     \
      x = NULL;         \
   }
#endif

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(x) \
   if(x != NULL)             \
   {                         \
      delete[] x;            \
      x = NULL;              \
   }
#endif



void EnumerateDisplays(IDXGIAdapter1* pAdapter) {

}
bool ImGuiInit = false;
IDXGIAdapter1* dxgiAdapter = nullptr;
DisplayInfo info;

void CleanD3D(void)
{
	if (ImGuiInit == true) {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	ImGuiInit = false;
	SAFE_RELEASE(swapchain);
	SAFE_RELEASE(deviceContext);
	SAFE_RELEASE(device);
	SAFE_RELEASE(outputDuplication);
	info.Adapters.clear();
	;
}
int initIndex = 0;

// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd, const int& outputNum)
{

	CleanD3D();
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = true;                                    // windowed/full-screen mode


	IDXGIFactory1* factory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory));


	info.Initialize(factory);


	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&device,
		NULL,
		&deviceContext);

	if (FAILED(hr))
		return;

	IDXGIDevice* dxgiDevice = nullptr;
	hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	if (FAILED(hr))
		return;

	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter1), reinterpret_cast<void**>(&dxgiAdapter));
	if (FAILED(hr))
		return;
	dxgiDevice->Release();

	// Select correct output
	IDXGIOutput* dxgiOutput = nullptr;
	hr = dxgiAdapter->EnumOutputs(outputNum, &dxgiOutput);
	if (FAILED(hr))
		return;

	IDXGIOutput1* dxgiOutput1 = nullptr;
	hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&dxgiOutput1));
	if (FAILED(hr))
		return;

	dxgiOutput->Release();

	// Set duplication
	hr = dxgiOutput1->DuplicateOutput(device, &outputDuplication);
	if (FAILED(hr))
		return;

	dxgiOutput1->Release();

	// Create rendertargetview for the backbuffer
	ID3D11Texture2D* pBackBuffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return;

	device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// Create the sampler state
	D3D11_SAMPLER_DESC SampDesc;
	RtlZeroMemory(&SampDesc, sizeof(SampDesc));
	SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.MinLOD = 0;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&SampDesc, &pointSamplerState);
	if (FAILED(hr))
		return;

	// Create vertex shader
	UINT Size = ARRAYSIZE(g_VS);
	hr = device->CreateVertexShader(g_VS, Size, nullptr, &VertexShader);
	if (FAILED(hr))
		return;

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT NumElements = ARRAYSIZE(Layout);
	hr = device->CreateInputLayout(Layout, NumElements, g_VS, Size, &InputLayout);
	if (FAILED(hr))
		return;

	deviceContext->IASetInputLayout(InputLayout);

	// Create pixel shader
	Size = ARRAYSIZE(g_PS);
	hr = device->CreatePixelShader(g_PS, Size, nullptr, &PixelShader);
	if (FAILED(hr))
		return;

	// Create vertex buffer
	D3D11_BUFFER_DESC BufferDesc;
	RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(VERTEX) * NUMVERTICES;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vertices;

	hr = device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
	if (FAILED(hr))
		return;

	// Create viewports
	ZeroMemory(&renderTargetViewport, sizeof(D3D11_VIEWPORT));
	renderTargetViewport.TopLeftX = 0;
	renderTargetViewport.TopLeftY = 0;
	renderTargetViewport.Width = ox;
	renderTargetViewport.Height = oy;

	ZeroMemory(&windowViewport, sizeof(D3D11_VIEWPORT));
	windowViewport.TopLeftX = 0;
	windowViewport.TopLeftY = 0;
	windowViewport.Width = 1920;
	windowViewport.Height = 1080;


	if (desktopTexture) {
		desktopTexture->Release();
		desktopTexture = nullptr;
	}
	if (desktopResourceView) {
		desktopResourceView->Release();
		desktopResourceView = nullptr;
	}
	if (renderTargetTexture) {
		renderTargetTexture->Release();
		renderTargetTexture = nullptr;
	}

	if (renderTargetResourceView) {
		renderTargetResourceView->Release();
		renderTargetResourceView = nullptr;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, deviceContext);
	ImGuiInit = true;
	initIndex = -1;
}
int frameVisible = 0;
// this is the function used to render a single frame
int intervalBFI = 0;


void RenderDisplayInfo(HWND hWnd) {

	int i = 0;

	ImGui::Text("%f", ImGui::GetIO().Framerate);
	ImGui::SliderInt("BFI interval", &intervalBFI,0,16);
	for (auto a : info.Adapters) {
		if (a.Outputs.size() == 0) continue;
		ImGui::Text(a.Name.c_str());

		for (auto b : a.Outputs) {
			ImGui::Text(b.DeviceName.c_str());
			i++;
			ImGui::PushID(i);
			ImGui::SameLine();
			if (ImGui::Button("Mirror")) {
				initIndex = b.Index;
			}
			ImGui::PopID();

		}
	}
}




HRESULT InitDesktopTexture(int width, int height)
{
	// Create render target texture
	HRESULT hr;
	D3D11_TEXTURE2D_DESC desktopTextureDesc;
	ZeroMemory(&desktopTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	desktopTextureDesc.Width = width;
	desktopTextureDesc.Height = height;
	desktopTextureDesc.MipLevels = 1;
	desktopTextureDesc.ArraySize = 1;
	desktopTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desktopTextureDesc.SampleDesc.Count = 1;
	desktopTextureDesc.SampleDesc.Quality = 0;
	desktopTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	desktopTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desktopTextureDesc.CPUAccessFlags = 0;
	desktopTextureDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&desktopTextureDesc, NULL, &desktopTexture);
	if (FAILED(hr))
		return hr;



	// Create render target resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC desktopResourceViewDesc;
	desktopResourceViewDesc.Format = desktopTextureDesc.Format;
	desktopResourceViewDesc.Texture2D.MipLevels = desktopTextureDesc.MipLevels;
	desktopResourceViewDesc.Texture2D.MostDetailedMip = desktopTextureDesc.MipLevels - 1;
	desktopResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	hr = device->CreateShaderResourceView(desktopTexture, &desktopResourceViewDesc, &desktopResourceView);
	if (FAILED(hr))
		return hr;
	return S_OK;
}

void RenderDesktopFrame()
{
	deviceContext->RSSetViewports(1, &renderTargetViewport);
	deviceContext->OMSetRenderTargets(1, &backbuffer, nullptr);

	// Set device resources
	deviceContext->VSSetShader(VertexShader, nullptr, 0);
	deviceContext->PSSetShader(PixelShader, nullptr, 0);
	deviceContext->PSSetShaderResources(0, 1, &desktopResourceView);
	deviceContext->PSSetSamplers(0, 1, &pointSamplerState);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);

	// Set window
	deviceContext->RSSetViewports(1, &windowViewport);
	deviceContext->OMSetRenderTargets(1, &backbuffer, NULL);
	deviceContext->Draw(NUMVERTICES, 0);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	swapchain->Present(1, 0);
}

void RenderFrame(HWND hWnd)
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	frameVisible = !frameVisible;



	if (frameVisible && intervalBFI > 0) {
		deviceContext->ClearRenderTargetView(backbuffer, color);

		for (int i = 0; i < intervalBFI; i++) {
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			RenderDisplayInfo(hWnd);
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			swapchain->Present(1, 0);
		}

		return;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	RenderDisplayInfo(hWnd);

	// clear the back buffer
	deviceContext->ClearRenderTargetView(backbuffer, color);

	// Request frame
	IDXGIResource* DesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;
	HRESULT hr = outputDuplication->AcquireNextFrame(0, &FrameInfo, &DesktopResource);


	if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
		// Draw window
		RenderDesktopFrame();
		return;
	}

	if (FAILED(hr))
	{
		InitD3D(hWnd, initIndex);
		return;
	}

	// Acquire texture
	hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&acquiredDesktopImage));


	if (FAILED(hr))
		return;

	DesktopResource->Release();
	DesktopResource = nullptr;

	D3D11_TEXTURE2D_DESC acquiredTextureDescription;
	acquiredDesktopImage->GetDesc(&acquiredTextureDescription);



	if (desktopTexture == nullptr)
	{
		hr = InitDesktopTexture(acquiredTextureDescription.Width, acquiredTextureDescription.Height);
		if (hr)
			return;
	}


	if (FrameInfo.LastPresentTime.QuadPart > 0) {
		deviceContext->CopyResource(desktopTexture, acquiredDesktopImage);
	}
	RenderDesktopFrame();

	outputDuplication->ReleaseFrame();

}

