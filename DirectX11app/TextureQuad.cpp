#include "TextureQuad.h"


#include "imgui.h"




//
//void TextureQuad::SetTechnique(D3D11Shader* s1, D3D11Shader* s2)
//{
//
///*	static float x, y, z;
//	static float scaleX, scaleY, scaleZ;
//	static float l=0, r=1920, b=0, t=1080;
//	if (s1 != VertexShader) {
//		this->VertexShader = s1;
//
//		D3D11_INPUT_ELEMENT_DESC Layout[] =
//		{
//			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//		};
//
//
//		if (InputLayout != nullptr) {
//			InputLayout->Release();
//			InputLayout = nullptr;
//		}
//
//		uint32_t NumElements = ARRAYSIZE(Layout);
//		HRESULT hr = render->D3dDevice->CreateInputLayout(Layout, NumElements, s1->ShaderData.BinaryData, s1->ShaderData.BinaryDataSize, &InputLayout);
//		if (FAILED(hr))
//			return;
//
//
//
//	}
//	this->PixelShader = s2;
//	ImGui::SliderFloat3("POS", &x, -2000, 2000, "%02f", 1);
//	ImGui::SliderFloat3("scale", &scaleX, -2000, 2000, "%02f", 1);
//	ImGui::SliderFloat4("Proj", &l, -2000, 2000, "%02f", 1);
//
//	auto pos = XMMatrixTranslation(x, y, z);
//	auto scaleM = XMMatrixScaling(1920,1080,1);
//	auto proj  = XMMatrixOrthographicLH(1920,1080,0,1);
//	auto mat = XMMatrixMultiply(XMMatrixMultiply(scaleM, pos), proj);
//	XMStoreFloat4x4(&Buffer.MVP, mat);
//
//	render->D3dDeviceContext->UpdateSubresource(this->ConstantBuffer, 0, nullptr, &Buffer, 0, 0);
//
//	*/
//
//
//
//}
//
//void TextureQuad::Draw(ID3D11ShaderResourceView* shaderResourceView)
//{
//
//	/*this->render->D3dDeviceContext->OMSetRenderTargets(1, &this->render->g_backbufferRenderTargetView, nullptr);
//	this->render->D3dDeviceContext->IASetInputLayout(InputLayout);
//	render->D3dDeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
//	this->render->D3dDeviceContext->VSSetShader(VertexShader->VertexShader.get(), nullptr, 0);
//	this->render->D3dDeviceContext->PSSetShader(PixelShader->PixelShader.get(), nullptr, 0);
//	this->render->D3dDeviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
//	this->render->D3dDeviceContext->PSSetShaderResources(2, 1, &shaderResourceView);
//
//	//auto samplerState = render->pointSampler.GetSamplerState();
//	//this->render->D3dDeviceContext->PSSetSamplers(0, 1, &samplerState);
//
//	uint32_t Stride = sizeof(TextureVertex);
//	uint32_t Offset = 0;
//	this->render->D3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	this->render->D3dDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
//	this->render->D3dDeviceContext->Draw(6, 0);
//	*/
//
//}
