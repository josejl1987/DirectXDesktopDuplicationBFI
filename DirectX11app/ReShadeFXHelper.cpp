#include "ReShadeFXHelper.h"

reshadefx::module ReShadeFXHelper::CompileReshadeFXShader(const char* name, int buffer_width, int buffer_height)
{
	reshadefx::parser parser;
	reshadefx::preprocessor pp;
	unsigned int shader_model = 50;

	pp.add_macro_definition("__RESHADE_PERFORMANCE_MODE__", "0");
	pp.add_macro_definition("BUFFER_WIDTH", std::to_string(buffer_width).c_str());
	pp.add_macro_definition("BUFFER_HEIGHT", std::to_string(buffer_height).c_str());
	pp.add_macro_definition("BUFFER_RCP_WIDTH", "(1.0 / BUFFER_WIDTH)");
	pp.add_macro_definition("BUFFER_RCP_HEIGHT", "(1.0 / BUFFER_HEIGHT)");
	std::unique_ptr<reshadefx::codegen> backend;
	backend.reset(reshadefx::create_codegen_hlsl(shader_model, false, false));
	pp.append_file(std::filesystem::path(std::string(name)));
	reshadefx::module module;

	if (!parser.parse(pp.output(), backend.get()))
	{

		return module;

	}

	backend->write_result(module);
	return module;
}
