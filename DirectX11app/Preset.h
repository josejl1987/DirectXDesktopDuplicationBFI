#pragma once
#include "Pass.h"
#include "Result.h"


struct ParsedPresetLine {
	std::string key;
	std::string value;
};

struct PresetInfo {
	std::string FilePath;
	std::vector<PassInfo> PassesInfo;
	static Result<std::vector<PassInfo>> ParseKeyValues(const std::vector<ParsedPresetLine>& keyValues);

	static Result<std::vector<ParsedPresetLine>> ParseSlangPresetLine(std::string& line);
	
	static Result<PresetInfo> LoadPresetInfoFromFile(const std::filesystem::path& path);
};

class Preset {
public:	 
	std::vector<Pass> Passes;
	static Result<Preset> LoadPresetFromFile(RetroSlang &sl, std::filesystem::path& path);
	void Process(RenderTarget* input);
	ID3D11ShaderResourceView* GetOutput();

	private:

};

