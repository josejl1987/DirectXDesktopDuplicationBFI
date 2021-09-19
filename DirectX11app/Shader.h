#pragma once
#include <map>
#include <string>
#include <vector>
struct ShaderParameter {

	std::string Name;
	std::string Description;
	float Initial;
	float Minimum;
	float Maximum;
	float Step;

	void RenderUI();
};
struct ShaderVariable {
	std::string Name;
	uint32_t StartOffset;
	uint32_t Size;
	uint32_t uFlags;
	void* DefaultValue;
	uint32_t StartTexture;
	uint32_t TextureSize;
	uint32_t StartSampler;
	uint32_t SamplerSize;


};

struct ShaderConstantBuffer {
	std::string Name;
	std::map<std::string, ShaderVariable> ConstantBufferVariables;
};
class Shader
{
public:
	std::string Name;
	std::string Profile;
	std::string EntryPoint;
	void* BinaryData;
	size_t BinaryDataSize;
	std::vector<ShaderParameter> Parameters;
	std::map<std::string, ShaderConstantBuffer> ConstantBuffers;
	std::map<std::string, ShaderVariable> Variables;
};

