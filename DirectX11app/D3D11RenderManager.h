#pragma once
#include "CGlobal.h"
#include "Sampler.h"
#include <d3d11_2.h>
#include <memory>
struct RetroSlang;
struct D3D11Shader;
struct TextureQuad;
struct  Preset;
struct D3D11RenderManager : std::enable_shared_from_this<D3D11RenderManager> {
	ID3D11Device* D3dDevice = nullptr;
	ID3D11DeviceContext* D3dDeviceContext = nullptr;
	IDXGISwapChain* g_pSwapChain = nullptr;
	ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
	ID3D11RenderTargetView* g_backbufferRenderTargetView = nullptr;

	ID3D11Texture2D* desktopTexture = nullptr;
	ID3D11ShaderResourceView* desktopResourceView = nullptr;
	IDXGIOutputDuplication* outputDuplication = nullptr;

	D3D11_VIEWPORT renderTargetViewport;
	ID3D11Texture2D* pBackBuffer;
	ID3D11RenderTargetView* backbuffer;

	D3D11_VIEWPORT windowViewport;

	CGlobal& Global;
	bool frameVisible = false;
	int intervalBFI;

	Sampler pointSampler;
	Sampler linearSampler;

	TextureQuad* tq;

	std::shared_ptr<D3D11RenderManager> getSharedPtr() { return shared_from_this(); }

	bool CreateSamplerState(const D3D11_SAMPLER_DESC& desc, Sampler* samp) const;
	D3D11RenderManager(CGlobal& global)
		: renderTargetViewport(), pBackBuffer(nullptr), backbuffer(nullptr), windowViewport(), Global(global),
		  intervalBFI(0), tq(nullptr) {
	};

	void Present() const;

	static std::shared_ptr<D3D11RenderManager> Create(CGlobal& global);

	void InitShaders();

	void LoadShader(std::string& shaderPath);

	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	HRESULT InitDesktopTexture(int width, int height);
	void RenderDesktopFrame();
	void RenderFrame();
	void AcquireFrame();
	bool CreateDuplication(int output_num);
	std::shared_ptr<RetroSlang> retroSlang;
};
