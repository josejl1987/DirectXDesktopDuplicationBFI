#pragma once
#include "D3D11RenderManager.h"
#include "TextureQuad.h"

class D3D11Mesh {
public:
	D3D11Mesh(D3D11RenderManager& render, const IMesh& mesh);
	const IMesh& MeshData;
	const D3D11RenderManager& RenderManager;

private:
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
	ID3D11Buffer* ConstantBuffer = nullptr;
};


