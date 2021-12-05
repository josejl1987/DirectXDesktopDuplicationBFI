#include "Pass.h"

Pass::Pass(Shader* vertexShader, Shader* pixelShader, PassInfo * passInfo) {

	this->VertexShader = vertexShader;
	this->PixelShader = pixelShader;
	this->info = passInfo;

}

 Pass::Pass() { }

void Pass::RenderParams() const {
	for each (auto var in this->PixelShader->Parameters) {
		var.RenderUI();
	}
}

Pass* PassFileLoader::LoadPass( RetroSlang& sl,  PassInfo * passInfo) {
	auto vs = sl.CompileShader(passInfo->ShaderPath, "vs_5_0", "main");
	auto ps = sl.CompileShader(passInfo->ShaderPath, "ps_5_0", "main");

	if (!vs || !ps)
		return nullptr;

	auto vs2 = new Shader(vs.value());
	auto ps2 = new Shader(ps.value());
	auto d =  new Pass(vs2,ps2,passInfo);

	return d;
}

 RenderTarget::RenderTarget(D3D11RenderManager* render, int width, int height) {
	this->render = render;
	this->RenderTargetView = nullptr;
	this->texture = new Texture(render, width, height);

	// Create render target resource view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	renderTargetViewDesc.Format = renderTargetViewDesc.Format;
	this->size.height = height;
	this->size.width = width;
	HRESULT hr;
	hr = render->D3dDevice->CreateRenderTargetView(this->texture->tex, &renderTargetViewDesc, &this->RenderTargetView);

	if (FAILED(hr))
		return;
}

RenderTarget::~RenderTarget() {
	if (this->RenderTargetView)
		this->RenderTargetView->Release();
	this->RenderTargetView = nullptr;
	delete texture;

	
}
