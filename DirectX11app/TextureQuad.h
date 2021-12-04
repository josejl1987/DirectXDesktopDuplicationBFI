#pragma once
#include <DirectXMath.h>
#include "D3D11RenderManager.h"
#include "D3D11Shader.h"
using namespace DirectX;
struct TextureVertex {
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};


struct TextureQuadBuffer {
	XMFLOAT4X4 MVP;
};

#pragma pack(16)
struct TextureQuadPixelConstantBuffer {
	XMFLOAT4 SourceSize;
	XMFLOAT4 InputSize;
	XMFLOAT4 OutputSize;
	int frameCount;
	int padding[3];
};

class IMesh {
public:
	std::vector<TextureVertex> Vertices;
	std::vector<uint32_t> Indices;
};
struct Pass;

class TextureQuad : IMesh
{
public:
	std::vector<TextureVertex> Vertices =
	{
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	TextureQuadBuffer Buffer;
	Pass* pass;
	TextureQuadPixelConstantBuffer pixelBuffer;
	void SetTechnique(D3D11RenderManager* render, Pass* pass);
	void Draw();
	D3D11Shader* VertexShader;
	D3D11Shader* PixelShader;
	ID3D11InputLayout* InputLayout;
	ID3D11Buffer* ConstantBuffer;
	ID3D11Buffer* ConstantPSBuffer;

	ID3D11Buffer* VertexBuffer;

	D3D11RenderManager* render;
};

