#pragma once
#include "Pass.h"
#include "Result.h"


struct ParsedPresetLine {
	std::string key;
	std::string value;
};

struct PresetInfo {
	std::string FilePath;
	std::vector<PassInfo> Passes;
};

class Preset {
public:
	std::vector<Pass> Passes;
	static Result<PresetInfo> LoadPresetFile(const std::filesystem::path& path);


	private:
	static Result<std::vector<PassInfo>> ParseKeyValues(const std::vector<ParsedPresetLine>& keyValues);

	static Result<std::vector<ParsedPresetLine>> ParseSlangPresetLine(std::string& line);
	
};

