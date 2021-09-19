#include "D3D11Mesh.h"
#include "D3D11RenderManager.h"
#include "TextureQuad.h"

inline D3D11Mesh::D3D11Mesh(D3D11RenderManager& render, const IMesh & mesh)
	: MeshData(mesh), RenderManager(render) {
	// Create vertex buffer
	D3D11_BUFFER_DESC BufferDesc;
	RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
	BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferDesc.ByteWidth = sizeof(TextureVertex) * 6;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	RtlZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh.Vertices.data();

	HRESULT hr = render.D3dDevice->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
	if (FAILED(hr))
		return;



	if (FAILED(hr)) {
		return;
	}
}
