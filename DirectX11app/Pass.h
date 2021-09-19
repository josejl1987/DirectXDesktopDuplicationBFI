#pragma once
#include <vector>
#include <d3d11.h>
#include "D3D11RenderManager.h"
#include "SlangHelper.h"
class Shader;

struct PixelSize {
	int width;
	int height;
};

class RenderTarget {
	PixelSize size;
	ID3D11Texture2D* Texture;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11ShaderResourceView* ShaderResourceView;
private:
	D3D11RenderManager* render;
	RenderTarget(D3D11RenderManager* render, int width, int height) {
		this->render = render;
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
		desktopTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desktopTextureDesc.CPUAccessFlags = 0;
		desktopTextureDesc.MiscFlags = 0;

		hr = render->D3dDevice->CreateTexture2D(&desktopTextureDesc, NULL, &Texture);
		if (FAILED(hr))
			return;



		// Create render target resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC desktopResourceViewDesc;
		desktopResourceViewDesc.Format = desktopTextureDesc.Format;
		desktopResourceViewDesc.Texture2D.MipLevels = desktopTextureDesc.MipLevels;
		desktopResourceViewDesc.Texture2D.MostDetailedMip = desktopTextureDesc.MipLevels - 1;
		desktopResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;



		// Create render target resource view
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		renderTargetViewDesc.Format = renderTargetViewDesc.Format;


		hr = render->D3dDevice->CreateShaderResourceView(Texture, &desktopResourceViewDesc, &this->ShaderResourceView);
		hr = render->D3dDevice->CreateRenderTargetView(Texture, &renderTargetViewDesc, &this->RenderTargetView);

		if (FAILED(hr))
			return;
	}

};



class Texture {
	PixelSize size;
};


struct PassInfo {
	bool FilterLinear;
	bool FloatFramebuffer;
	uint32_t FrameCountMod;
	std::string ScaleType;
	std::string ScaleTypeX;
	std::string ScaleTypeY;

	float ScaleX;
	float ScaleY;
	float Scale;
	std::string WrapMode;
	std::string Alias;
	std::string ShaderPath;
	std::vector<std::string> Textures;
};

class Pass
{
public:


	std::vector<Texture> Input;
	PixelSize FinalViewportSize;
	Shader& VertexShader;
	Shader& PixelShader;
	RenderTarget* Output;
	Pass(Shader& vertexShader, Shader& pixelShader);
	void RenderParams() const;

};




class PassFileLoader {

	static Pass LoadPass(const std::filesystem::path& path) {
		
	}
};


