#include "D3D11RenderManager.h"
#include "CGlobal.h"
#include "D3D11Shader.h"
#include "SlangHelper.h"
#include "TextureQuad.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/imgui.h"
#include <dxgi1_2.h>
#include <d3d11sdklayers.h>
#include "Preset.h"

bool D3D11RenderManager::CreateSamplerState(const D3D11_SAMPLER_DESC& desc, Sampler* samp) const {
	memcpy(&samp->sampDesc, &desc, sizeof(D3D11_SAMPLER_DESC));
	ID3D11SamplerState* state;
	this->D3dDevice->CreateSamplerState(&samp->sampDesc, &samp->sampState);
	return true;
}

void D3D11RenderManager::Present() const {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(1, 0);
}

std::shared_ptr<D3D11RenderManager> D3D11RenderManager::Create(CGlobal& global) {
	auto newRendering = std::make_shared<D3D11RenderManager>(global);

	newRendering->Global = global;
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = global.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	uint32_t createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};
	HRESULT name = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd,
		&newRendering->g_pSwapChain, &newRendering->D3dDevice, &featureLevel, &newRendering->D3dDeviceContext);
	if (name != S_OK)
		return nullptr;

	IDXGIFactory1* factory;
	auto f = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**) (&factory));

	global.displayInfo.Initialize(factory);
	factory->Release();

	auto pointSampler = Sampler();
	newRendering->pointSampler.sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	newRendering->pointSampler.sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	newRendering->pointSampler.sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	newRendering->pointSampler.sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	auto linearSampler = Sampler();

	newRendering->linearSampler.sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	newRendering->linearSampler.sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	newRendering->linearSampler.sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	newRendering->linearSampler.sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	newRendering->CreateSamplerState(newRendering->linearSampler.sampDesc, &newRendering->linearSampler);
	newRendering->CreateSamplerState(newRendering->pointSampler.sampDesc, &newRendering->pointSampler);

	newRendering->windowViewport.TopLeftX = 0;
	newRendering->windowViewport.TopLeftY = 0;
	newRendering->windowViewport.Width = 1920;
	newRendering->windowViewport.Height = 1080;
	newRendering->windowViewport.MinDepth = 0;
	newRendering->windowViewport.MaxDepth = 1;
	newRendering->CreateRenderTarget();
	newRendering->InitShaders();
	return newRendering;
}

void D3D11RenderManager::InitShaders() {

	this->retroSlang = std::make_shared<RetroSlang>(this->getSharedPtr());
	this->Global.fileDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_NoModal);
	this->Global.fileDialog.SetTitle("Shaders");
	this->Global.fileDialog.SetTypeFilters({ ".slangp" });
	return;
}

void D3D11RenderManager::LoadShader(std::string& shaderPath) {
	auto preset = Preset::LoadPresetFromFile(*retroSlang, std::filesystem::path(shaderPath));
	if (preset.has_value())
		this->Global.currentPreset = std::make_shared<Preset>(preset.value());
}

void D3D11RenderManager::CleanupDeviceD3D() {
	ID3D11Debug* m_d3dDebug;
	D3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_d3dDebug));

	if (linearSampler.sampState) {
		linearSampler.sampState->Release();
	}
	if (pointSampler.sampState) {
		pointSampler.sampState->Release();
	}
	if (g_pSwapChain) {
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (D3dDeviceContext) {
		D3dDeviceContext->Release();
		D3dDeviceContext = NULL;
	}
	m_d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	if (D3dDevice) {
		D3dDevice->Release();
		D3dDevice = NULL;
	}
	m_d3dDebug->Release();
}

void D3D11RenderManager::CreateRenderTarget() {
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	D3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_backbufferRenderTargetView);
	pBackBuffer->Release();
}

void D3D11RenderManager::CleanupRenderTarget() {
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
	if (Global.desktopRT)
		delete Global.desktopRT;
	if (tq)
		delete tq;
}

HRESULT D3D11RenderManager::InitDesktopTexture(int width, int height) {
	// Create render target texture

	Global.desktopRT = new RenderTarget(this, width, height);

	tq = new TextureQuad();
	return S_OK;
}

void D3D11RenderManager::RenderDesktopFrame() {
	if (Global.desktopRT == nullptr)
		return;

	ID3D11ShaderResourceView* texture = nullptr;
	if (this->Global.currentPreset != nullptr && this->Global.currentPreset->Passes.size() > 0) {
		this->Global.currentPreset->Process(Global.desktopRT);

		 texture = this->Global.currentPreset->GetOutput();
	}
	if (texture == nullptr)
		texture = Global.desktopRT->texture->ShaderResourceView;
	int col = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1.0f));

	if (Global.frameCount++ % (Global.intervalBFI + 1) == 0)
		col = -1;
	ImGui::GetBackgroundDrawList()->AddImage(texture,
	                                         ImVec2(0, 0),
	             ImVec2(Global.desktopRT->size.width, Global.desktopRT->size.height),ImVec2(0,0),ImVec2(1,1),col);
}

void D3D11RenderManager::RenderFrame() {
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	this->D3dDeviceContext->RSSetViewports(1, &this->windowViewport);
	AcquireFrame();
	RenderDesktopFrame();
}

void D3D11RenderManager::AcquireFrame() {
	IDXGIResource* desktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	if (!outputDuplication)
		return;
	HRESULT hr = outputDuplication->AcquireNextFrame(0, &frameInfo, &desktopResource);
	ID3D11Texture2D* acquiredDesktopImage;

	if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
		return;
	}

	if (FAILED(hr)) {
		//	InitD3D();
		return;
	}

	// Acquire texture
	hr = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&acquiredDesktopImage));

	if (FAILED(hr))
		return;

	desktopResource->Release();
	desktopResource = nullptr;

	D3D11_TEXTURE2D_DESC acquiredTextureDescription;
	acquiredDesktopImage->GetDesc(&acquiredTextureDescription);

	if (Global.desktopRT == nullptr) {
		hr = InitDesktopTexture(acquiredTextureDescription.Width, acquiredTextureDescription.Height);
		if (hr)
			return;
	}

	if (frameInfo.LastPresentTime.QuadPart > 0) {
		D3dDeviceContext->CopyResource(Global.desktopRT->texture->tex, acquiredDesktopImage);
	}
	outputDuplication->ReleaseFrame();
}

bool D3D11RenderManager::CreateDuplication(int output_num) {
	IDXGIAdapter1* dxgiAdapter;
	IDXGIDevice* dxgiDevice = nullptr;

	if (this->outputDuplication) {
		this->outputDuplication->Release();
		this->outputDuplication = nullptr;
	}

	HRESULT hr = D3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	if (FAILED(hr))
		return false;

	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter1), reinterpret_cast<void**>(&dxgiAdapter));
	if (FAILED(hr))
		return false;
	dxgiDevice->Release();

	// Select correct output
	IDXGIOutput* dxgiOutput = nullptr;
	hr = dxgiAdapter->EnumOutputs(output_num, &dxgiOutput);
	if (FAILED(hr))
		return false;

	IDXGIOutput1* dxgiOutput1 = nullptr;
	hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&dxgiOutput1));
	if (FAILED(hr))
		return false;

	dxgiOutput->Release();

	// Set duplication
	hr = dxgiOutput1->DuplicateOutput(D3dDevice, &this->outputDuplication);
	if (FAILED(hr))
		return false;

	return true;
}
