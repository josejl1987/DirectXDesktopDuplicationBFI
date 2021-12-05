#pragma once
#include "D3D11RenderManager.h"
#include "Shader.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <map>
#include <memory>
#pragma comment(lib, "dxguid.lib")



class D3D11Shader {
public:
	D3D11RenderManager* render;
	Shader* ShaderData;
	std::shared_ptr<ID3D11VertexShader> VertexShader;
	std::shared_ptr<ID3D11PixelShader> PixelShader;

	D3D11Shader(Shader* shader, D3D11RenderManager* render);
	~D3D11Shader();
};


