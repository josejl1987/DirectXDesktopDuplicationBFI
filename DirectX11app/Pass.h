#pragma once
#include "D3D11RenderManager.h"
#include "SlangHelper.h"
#include <d3d11.h>
#include <vector>
class Shader;

struct PixelSize {
	int width;
	int height;
};

class Texture {
public:
	ID3D11Texture2D* tex;
	ID3D11ShaderResourceView* ShaderResourceView;

	PixelSize size;
	~Texture() {
		if (tex)
			tex->Release();
		tex = nullptr;
		if (ShaderResourceView)
			ShaderResourceView->Release();
		ShaderResourceView = nullptr;
	}
	Texture() {
		tex = nullptr;
		ShaderResourceView = nullptr;
	}
	Texture(D3D11RenderManager* renderManager, int width, int height) {
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
		desktopTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desktopTextureDesc.CPUAccessFlags = 0;
		desktopTextureDesc.MiscFlags = 0;
		// Create render target resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC desktopResourceViewDesc;
		desktopResourceViewDesc.Format = desktopTextureDesc.Format;
		desktopResourceViewDesc.Texture2D.MipLevels = desktopTextureDesc.MipLevels;
		desktopResourceViewDesc.Texture2D.MostDetailedMip = desktopTextureDesc.MipLevels - 1;
		desktopResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		HRESULT hr = renderManager->D3dDevice->CreateTexture2D(&desktopTextureDesc, NULL, &tex);
		if (FAILED(hr))
			return;
		hr = renderManager->D3dDevice->CreateShaderResourceView(this->tex, &desktopResourceViewDesc,
		                                                       &this->ShaderResourceView);

		this->size.height = height;
		this->size.width = width;
		if (FAILED(hr))
			return;
	}
};

class RenderTarget {
public:
	PixelSize size;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	Texture* texture;
	RenderTarget(D3D11RenderManager* render, int width, int height);
	~RenderTarget();

public:
	D3D11RenderManager* render;
};

struct PassInfo {
	bool FilterLinear;
	bool FloatFramebuffer;
	uint32_t FrameCountMod;
	std::string ScaleType;
	std::string ScaleTypeX;
	std::string ScaleTypeY;

	float ScaleX = 1;
	float ScaleY = 1;
	float Scale = 1;
	std::string WrapMode;
	std::string Alias;
	std::string ShaderPathStr;
	std::filesystem::path ShaderPath;

	std::vector<std::string> Textures;
};

class Pass {
public:
	PassInfo* info = nullptr;
	std::vector<RenderTarget> Input;
	PixelSize FinalViewportSize;
	Shader* VertexShader = nullptr;
	Shader* PixelShader = nullptr;
	RenderTarget* Output = nullptr;
	Pass();
	Pass(Shader* vertexShader, Shader* pixelShader, PassInfo* passInfo);
	void RenderParams() const;
};

class PassFileLoader {
public:
	static Pass* LoadPass(RetroSlang& sl, PassInfo* passInfo);
};
