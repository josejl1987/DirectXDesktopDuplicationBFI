#pragma once
#include "D3D11RenderManager.h"
#include <filesystem>
#include "Shader.h"
#include <vector>
#include "Result.h"
#include <DirectXMath.h>
 


struct GlobalParameters {
	DirectX::XMFLOAT4 SourceSize;
	DirectX::XMFLOAT4 OriginalSize;
	DirectX::XMFLOAT4 OutputSize;
	uint32_t FrameCount;

};




struct ShaderParsingInfo {
	std::string shaderProfile;
	bool shouldAddLine = true;
	std::vector<ShaderParameter> parameters;
};

class RetroSlang
{
public:
	RetroSlang(std::shared_ptr<D3D11RenderManager> render);
	Result<std::shared_ptr<Shader>> CompileShader(const std::filesystem::path& path, const std::string& profileName,
	                                              const std::string& entryPointName);



private:

	std::shared_ptr<D3D11RenderManager> render = nullptr;	
	Result<std::string> ParseRetroSlangShader(const std::filesystem::path& path, char* error, ShaderParsingInfo& info) const;
	Result<std::shared_ptr<Shader>> CompileHlsl(const std::string& source, const std::filesystem::path& path, const std::string& entryPointName, const std::string&
	                                    profileName) const;
	std::string SpirvToHlsl(std::vector<uint32_t> spirvBinary);
};

