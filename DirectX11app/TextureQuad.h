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


class IMesh {
public:
	std::vector<TextureVertex> Vertices;
	std::vector<uint32_t> Indices;
};


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


};

