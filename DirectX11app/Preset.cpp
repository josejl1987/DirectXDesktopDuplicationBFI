#include "Preset.h"
#include <regex>
#include <sstream>
#include <stb_c_lexer.h>
#include <stb_include.h>

#include "StringTools.h"
#include <iostream>
#include "TextureQuad.h"

using std::cin;
using std::cout;
using std::size;

constexpr inline auto string_hash(const char* s) {
	unsigned long long hash {}, c {};
	for (auto p = s; *p; ++p, ++c) {
		hash += *p << c;
	}
	return hash;
}

constexpr inline auto operator"" _sh(const char* s, size_t) {
	return string_hash(s);
}

Result<PresetInfo> PresetInfo::LoadPresetInfoFromFile(const std::filesystem::path& path) {
	char error[16384];
	char* contents =
	    stb_include_file((char*) path.string().c_str(), nullptr, (char*) path.parent_path().string().c_str(), error);

	if (contents == nullptr) {
		return tl::make_unexpected("Couldn't parse "+path.string());
	}

	std::map<std::string, std::string> presetLines;
	std::stringstream ss(contents);
	char buf[16384];
	char str[10000];
	free(contents);

	Preset out;

	std::string line;
	std::stringstream shaderPragma;
	std::vector<ParsedPresetLine> keyValues;
	while (std::getline(ss, line)) {
		auto result = ParseSlangPresetLine(line);

		if (!result) {
			return {};
		} else {
			if (result.value().size() > 0)
				keyValues.insert(keyValues.end(), std::make_move_iterator(result.value().begin()),
				                 std::make_move_iterator(result.value().end()));
		}
	}
	PresetInfo pInfo;
	pInfo.FilePath = path.string();

	auto passesInfo = ParseKeyValues(keyValues);

	if (!passesInfo) {
		return tl::make_unexpected(passesInfo.error());
	}
	pInfo.PassesInfo = passesInfo.value();

	for  (auto & var : pInfo.PassesInfo) {
		auto d = path.parent_path().string() + "/" + var.ShaderPathStr;
		var.ShaderPath = std::filesystem::path(d);
		continue;
	}

	


	return pInfo;
}

bool ShouldIgnore(int shaderNum, int pass)

{
	if (pass == shaderNum == 0)
		return false;
	if (shaderNum >= pass) {
		return true;
	}
	return false;
}

Result<std::vector<PassInfo>> PresetInfo::ParseKeyValues(const std::vector<ParsedPresetLine>& keyValues) {
	std::regex r("([a-zA-Z_]+)([0-9]*)");
	std::vector<PassInfo> passes;
	for (const auto& item : keyValues) {
		std::smatch sm;
		std::regex_search(item.key, sm, r);

		auto id = sm[1].str();
		int shaderNum = -1;
		int definitiveShaderCount;
		if (sm.size() == 3 && sm[2].str().size() > 0) {
			shaderNum = std::atoi(sm[2].str().c_str());
		}
		if (shaderNum >= (int) passes.size()) {
			passes.resize(shaderNum + 1);
		}

		switch (string_hash(id.c_str())) {
		case "shaders"_sh: {
			int value = to_uint32(item.value);
			passes.resize(value);

			break;
		}

		case "shader"_sh: {
			if (ShouldIgnore(shaderNum, passes.size()))
				continue;
			passes[shaderNum].ShaderPathStr = item.value;

			break;
		}
		case "scale_type"_sh: {
			if (ShouldIgnore(shaderNum, passes.size()))
				continue;
			passes[shaderNum].ScaleType = item.value;

			break;
		}

		case "scale_type_x"_sh: {
			if (ShouldIgnore(shaderNum, passes.size()))
				continue;
			passes[shaderNum].ScaleTypeX = item.value;

			break;
		}
		case "scale_type_y"_sh: {
			if (ShouldIgnore(shaderNum, passes.size()))
				continue;
			passes[shaderNum].ScaleTypeY = item.value;

			break;
		}

		case "scale"_sh: {
			if (ShouldIgnore(shaderNum, passes.size()))
				continue;
			passes[shaderNum].Scale = std::atof(item.value.c_str());
			passes[shaderNum].ScaleX = std::atof(item.value.c_str());
			passes[shaderNum].ScaleY = std::atof(item.value.c_str());

			break;

		case "scale_x"_sh: {
			if (ShouldIgnore(shaderNum, passes.size()))
				continue;
			passes[shaderNum].ScaleX = std::atof(item.value.c_str());

			break;
		}
		case "scale_y"_sh: {
			passes[shaderNum].ScaleY = std::atof(item.value.c_str());

			break;
		}

		case "wrap_mode"_sh: {
			passes[shaderNum].WrapMode = item.value;

			break;
		}
		case "alias"_sh: {
			passes[shaderNum].Alias = item.value;

			break;
		}

		case "float_framebuffer"_sh: {
			bool v = to_bool(item.value);

			passes[shaderNum].FloatFramebuffer = v;

			break;
		}
		case "texture"_sh: {
			passes[shaderNum].ShaderPathStr = item.value;

			break;
		}

		case "filter_linear"_sh: {
			bool v = to_bool(item.value);

			passes[shaderNum].FilterLinear = v;
			break;
		}

		default:
			break;
		//	return tl::make_unexpected("Field undefined " + id);
		}
		}
	}
	return passes;
}
Result<std::vector<ParsedPresetLine>> PresetInfo::ParseSlangPresetLine(std::string& line) {
	stb_lexer lexer;
	char buf[16384];
	char str[10000];
	memset(buf, 0, 16384);
	memset(str, 0, 10000);

	std::vector<ParsedPresetLine> out;
	// Vector of string to save tokens
	std::vector<std::string> tokens;

	// stringstream class check1
	std::stringstream check1(line);

	std::string intermediate;

	// Tokenizing w.r.t. space ' '
	while (getline(check1, intermediate, '=')) {
		tokens.push_back(intermediate);
	}
	int count = 0;
	ParsedPresetLine res;
	for (const auto item : tokens) {
		if (count % 2 == 0) {
			res.key = item;
		} else {
			res.value = item.substr(item.find_first_not_of(' '));
			out.push_back(res);
		}
		count++;
	}

	return out;
}

Result<Preset> Preset::LoadPresetFromFile( RetroSlang & sl,  std::filesystem::path& path) {

	std::shared_ptr<Preset> preset = std::make_shared<Preset>();
	auto p = PresetInfo::LoadPresetInfoFromFile(path);
	if (!p)
		return tl::make_unexpected(p.error());

	auto v = p.value();

	preset->Passes.resize(v.PassesInfo.size());

	int index = 0;
	for (auto& pInfo : v.PassesInfo) {

		preset->Passes[index] = Pass(*PassFileLoader::LoadPass(sl, new PassInfo(pInfo)));
		index++;
	}

	return *preset;
}

void Preset::Process(RenderTarget* input) { 


	for (auto& pInfo : this->Passes) {
		pInfo.FinalViewportSize.width = pInfo.info->ScaleX * input->size.width;
		pInfo.FinalViewportSize.height = pInfo.info->ScaleY * input->size.height;

		if (!pInfo.Output) {
			pInfo.Output =
			    new RenderTarget(input->render, pInfo.FinalViewportSize.width, pInfo.FinalViewportSize.height);
		}
		if (pInfo.Output->size.width != pInfo.FinalViewportSize.width ||
		    pInfo.Output->size.height != pInfo.FinalViewportSize.height) {
		
			if (pInfo.Output)
				delete pInfo.Output;
			pInfo.Output = new RenderTarget(input->render, pInfo.FinalViewportSize.width, pInfo.FinalViewportSize.height);

		}

		if (pInfo.Input.size() == 0) {
			pInfo.Input.push_back(*input);

		}

		input = pInfo.Output;
	}

		for (auto& pInfo : this->Passes) {
		input->render->tq->SetTechnique(input->render, &pInfo);
		    input->render->tq->Draw();

	}

}

ID3D11ShaderResourceView* Preset::GetOutput() {

	if (this->Passes.size() == 0)
		return nullptr;
	return this->Passes[this->Passes.size()-1].Output->texture.ShaderResourceView;

}
