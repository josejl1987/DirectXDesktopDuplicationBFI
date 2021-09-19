#include "SlangHelper.h"
#include "expected.h"
#include <d3dcompiler.h>
#include <fstream>
#include <memory>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <sstream>
#define STB_C_LEX_C_DECIMAL_INTS Y
#define STB_C_LEX_C_HEX_INTS Y
#define STB_C_LEX_C_OCTAL_INTS Y
#define STB_C_LEX_C_DECIMAL_FLOATS Y
#define STB_C_LEX_C99_HEX_FLOATS Y
#define STB_C_LEX_C_IDENTIFIERS Y
#define STB_C_LEX_C_DQ_STRINGS Y
#define STB_C_LEX_C_SQ_STRINGS Y
#define STB_C_LEX_C_CHARS Y
#define STB_C_LEX_C_COMMENTS N
#define STB_C_LEX_CPP_COMMENTS N
#define STB_C_LEX_C_COMPARISONS Y
#define STB_C_LEX_C_LOGICAL Y
#define STB_C_LEX_C_SHIFTS Y
#define STB_C_LEX_C_INCREMENTS Y
#define STB_C_LEX_C_ARROW Y
#define STB_C_LEX_EQUAL_ARROW Y
#define STB_C_LEX_C_BITWISEEQ Y
#define STB_C_LEX_C_ARITHEQ Y

#define STB_C_LEX_PARSE_SUFFIXES Y
#define STB_C_LEX_DECIMAL_SUFFIXES "uUlL"
#define STB_C_LEX_HEX_SUFFIXES "lL"
#define STB_C_LEX_OCTAL_SUFFIXES "lL"
#define STB_C_LEX_FLOAT_SUFFIXES "uulL"

#define STB_C_LEX_0_IS_EOF N
#define STB_C_LEX_INTEGERS_AS_DOUBLES N
#define STB_C_LEX_MULTILINE_DSTRINGS Y
#define STB_C_LEX_MULTILINE_SSTRINGS Y
#define STB_C_LEX_USE_STDLIB N
#define STB_C_LEX_DOLLAR_IDENTIFIER Y
#define STB_C_LEX_FLOAT_NO_DECIMAL Y

#define STB_C_LEX_DEFINE_ALL_TOKEN_NAMES Y
#define STB_C_LEX_DISCARD_PREPROCESSOR N
#define STB_C_LEXER_DEFINITIONS // This line prevents the header file from replacing your definitions
#define STB_C_LEXER_IMPLEMENTATION
#include <stb_c_lexer.h>
#define STB_INCLUDE_LINE_GLSL
#define STB_INCLUDE_IMPLEMENTATION
#include <stb_include.h>

#include "imgui.h"

void ParsePragma(stb_lexer& lexer, ShaderParsingInfo& info) {
	while (stb_c_lexer_get_token(&lexer)) {
		if (strcmp(lexer.string, "stage") == 0) {
			stb_c_lexer_get_token(&lexer);
			{
				if (info.shaderProfile != std::string_view(lexer.string))
					info.shouldAddLine = false;
				else
					info.shouldAddLine = true;
				break;
			}
		}

		else if (strcmp(lexer.string, "parameter") == 0) {
			ShaderParameter param;
			stb_c_lexer_get_token(&lexer);
			param.Name = lexer.string;
			stb_c_lexer_get_token(&lexer);
			param.Description = lexer.string;
			stb_c_lexer_get_token(&lexer);
			param.Initial = lexer.real_number;
			stb_c_lexer_get_token(&lexer);
			param.Minimum = lexer.real_number;
			stb_c_lexer_get_token(&lexer);
			param.Maximum = lexer.real_number;
			auto b = stb_c_lexer_get_token(&lexer);
			if (b) {
				param.Step = lexer.real_number;
			}
			info.parameters.push_back(param);
		}
	}
}
void CheckPragma(stb_lexer& lexer, ShaderParsingInfo& info) {
	while (stb_c_lexer_get_token(&lexer)) {
		if (lexer.token == '#') {
			stb_c_lexer_get_token(&lexer);
			if (strcmp("pragma", lexer.string) == 0) {
				ParsePragma(lexer, info);
			};
		}
		break;
	}
}

RetroSlang::RetroSlang(std::shared_ptr<D3D11RenderManager> render) {
	this->render = render;
}

Result<std::shared_ptr<Shader>> RetroSlang::CompileShader(const std::filesystem::path& path,
                                                                             const std::string& profileName,
                                                                             const std::string& entryPointName) {
	shaderc_shader_kind kind;
	const shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	ShaderParsingInfo info;

	if (profileName == "ps_5_0") {
		kind = shaderc_shader_kind::shaderc_fragment_shader;
		info.shaderProfile = "fragment";
	} else if (profileName == "vs_5_0") {
		kind = shaderc_shader_kind::shaderc_vertex_shader;
		info.shaderProfile = "vertex";
	}
	char error[256];
	const auto& glsl = ParseRetroSlangShader(path, error, info);
	
	if (!glsl) {
		return tl::make_unexpected(glsl.error());
	}


	const auto module = compiler.CompileGlslToSpv(glsl.value(), kind, "shader", options);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::string error = module.GetErrorMessage();
		return tl::make_unexpected(ErrorInfo(std::move(error)));
	}

	const auto spirvBinary = std::vector<uint32_t>(module.begin(), module.end());

	std::string source = SpirvToHlsl(spirvBinary);

	auto hlsl = CompileHlsl(source, path, entryPointName, profileName);
	if (!hlsl) {
		return tl::make_unexpected(hlsl.error());
	}
	hlsl->get()->Parameters = std::move(info.parameters);
	return hlsl;
}

Result<std::string> RetroSlang::ParseRetroSlangShader(const std::filesystem::path& path, char* error,
                                                      ShaderParsingInfo& info) const {
	char* contents =
	    stb_include_file((char*) path.string().c_str(), nullptr, (char*) path.parent_path().string().c_str(), error);

	std::stringstream ss(contents);
	char buf[16384];
	char str[10000];
	free(contents);

	std::string line;
	stb_lexer lexer;
	std::stringstream shaderPragma;
	while (std::getline(ss, line)) {
		memset(buf, 0, 16384);
		auto length = strncpy(str, line.c_str(), line.size());
		str[line.size()] = 0;
		stb_c_lexer_init(&lexer, str, str + strlen(str), buf, 16384);
		CheckPragma(lexer, info);
		if (info.shouldAddLine)
			shaderPragma << line << "\n";
	}

	return shaderPragma.str();
}

Result<std::shared_ptr<Shader>> RetroSlang::CompileHlsl(const std::string& source, const std::filesystem::path& path,
                                                        const std::string& entryPointName,
                                                        const std::string& profileName) const {
	ID3DBlob* blob;
	ID3DBlob* errorBlob;
	D3DCompile(source.c_str(), source.size(), path.filename().stem().string().c_str(), nullptr, nullptr,
	           entryPointName.c_str(), profileName.c_str(), 0, 0, &blob, &errorBlob);
	if (errorBlob != nullptr) {
		char* errorStr = (char*) errorBlob->GetBufferPointer();
		return nullptr;
	}

	std::shared_ptr<Shader> output = std::make_shared<Shader>();
	output->BinaryData = blob->GetBufferPointer();
	output->BinaryDataSize = blob->GetBufferSize();
	output->EntryPoint = std::string(entryPointName);
	output->Name = path.filename().string();
	output->Profile = std::string(profileName);
	return output;
}

std::string RetroSlang::SpirvToHlsl(std::vector<uint32_t> spirvBinary) {
	spirv_cross::CompilerHLSL hlsl(spirvBinary);

	// The SPIR-V is now parsed, and we can perform reflection on it.
	spirv_cross::ShaderResources resources = hlsl.get_shader_resources();

	// Get all sampled images in the shader.
	for (auto& resource : resources.sampled_images) {
		unsigned set = hlsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = hlsl.get_decoration(resource.id, spv::DecorationBinding);
		printf("Image %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);

		// Modify the decoration to prepare it for GLSL.
		hlsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);

		// Some arbitrary remapping if we want.
		hlsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
	}

	// Set some options.
	spirv_cross::CompilerHLSL::Options options2;

	options2.shader_model = 50;
	hlsl.set_hlsl_options(options2);

	// Compile to GLSL, ready to give to GL driver.
	return hlsl.compile();
}

void ShaderParameter::RenderUI() {
	ImGui::InputFloat(this->Description.c_str(), &Initial, Step);
}
