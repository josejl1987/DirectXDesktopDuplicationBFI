#pragma once
#include <d3d11_2.h>
#include <string>
struct D3D11RenderManager;

struct Sampler {
	std::string Name;
	D3D11_SAMPLER_DESC sampDesc;
	Sampler();
private:
	ID3D11SamplerState* sampState = nullptr;



};

