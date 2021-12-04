#include "TextureQuad.h"

#include "Pass.h"
#include "imgui.h"

void TextureQuad::SetTechnique(D3D11RenderManager* render, Pass* pass) {
	static float x, y, z;
	static float scaleX, scaleY, scaleZ;
	static float l = 0, r = 1920, b = 0, t = 1080;
	this->pass = pass;
	this->render = render;

	// Create vertex buffer
	D3D11_BUFFER_DESC BufferDesc;
	RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(TextureVertex) * 6;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = this->Vertices.data();

	HRESULT hr = this->render->D3dDevice->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
	this->pixelBuffer.InputSize.x = pass->Input[0].texture.size.width;
	this->pixelBuffer.InputSize.y = pass->Input[0].texture.size.height;
	this->pixelBuffer.OutputSize.x = pass->Output->texture.size.width;
	this->pixelBuffer.OutputSize.y = pass->Output->texture.size.height;
	this->pixelBuffer.SourceSize.x = pass->Input[0].texture.size.width;
	this->pixelBuffer.SourceSize.y = pass->Input[0].texture.size.height;

		// Create vertex buffer
	RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(TextureQuadBuffer);
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = 0;

		 hr = this->render->D3dDevice->CreateBuffer(&BufferDesc, nullptr, &ConstantBuffer);

		RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(TextureQuadPixelConstantBuffer);
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = 0;
	hr = this->render->D3dDevice->CreateBuffer(&BufferDesc, nullptr, &ConstantPSBuffer);



	 	if (FAILED(hr))
		 return;


	this->VertexShader = new D3D11Shader(pass->VertexShader, render);

	D3D11_INPUT_ELEMENT_DESC Layout[] = {
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (InputLayout != nullptr) {
		InputLayout->Release();
		InputLayout = nullptr;
	}

	uint32_t NumElements = ARRAYSIZE(Layout);
	 hr = render->D3dDevice->CreateInputLayout(Layout, NumElements, this->VertexShader->ShaderData->BinaryData,
	                                                  this->VertexShader->ShaderData->BinaryDataSize, &InputLayout);
	if (FAILED(hr))
		return;

	this->PixelShader = new D3D11Shader(pass->PixelShader, render);

	auto pos = XMMatrixTranslation(x, y, z);
	auto scaleM = XMMatrixScaling(pass->FinalViewportSize.width, pass->FinalViewportSize.height, 1);
	auto proj = XMMatrixOrthographicLH(pass->FinalViewportSize.width, pass->FinalViewportSize.height, 0, 1);
	auto mat = XMMatrixMultiply(XMMatrixMultiply(scaleM, pos), proj);
	XMStoreFloat4x4(&Buffer.MVP, mat);

	render->D3dDeviceContext->UpdateSubresource(this->ConstantBuffer, 0, nullptr, &Buffer, 0, 0);
	render->D3dDeviceContext->UpdateSubresource(this->ConstantPSBuffer, 0, nullptr, &pixelBuffer, 0, 0);

	this->render = render;
}

void TextureQuad::Draw() {
	this->render->D3dDeviceContext->IASetInputLayout(InputLayout);


	this->render->D3dDeviceContext->VSSetShader(this->VertexShader->VertexShader.get(), nullptr, 0);
	this->render->D3dDeviceContext->PSSetShader(this->PixelShader->PixelShader.get(), nullptr, 0);
	this->render->D3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	this->render->D3dDeviceContext->PSSetShaderResources(0, 1, &pass->Input[0].texture.ShaderResourceView);
	this->render->D3dDeviceContext->PSSetShaderResources(2, 1, &pass->Input[0].texture.ShaderResourceView);
	this->render->D3dDeviceContext->OMSetRenderTargets(1, &pass->Output->RenderTargetView, nullptr);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.Height = pass->FinalViewportSize.height;
	vp.Width = pass->FinalViewportSize.width;
	vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	this->render->D3dDeviceContext->RSSetViewports(1,&vp);


	 auto samplerState = render->pointSampler.sampState;
	 this->render->D3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
	 this->render->D3dDeviceContext->PSSetSamplers(2, 1, &samplerState);
	 render->D3dDeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	 render->D3dDeviceContext->PSSetConstantBuffers(1, 1, &ConstantPSBuffer);
	uint32_t Stride = sizeof(TextureVertex);
	 uint32_t Offset = 0;
	this->render->D3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->render->D3dDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
	this->render->D3dDeviceContext->Draw(6, 0);
}
