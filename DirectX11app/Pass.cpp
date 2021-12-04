#include "Pass.h"

Pass::Pass(Shader* vertexShader, Shader* pixelShader, PassInfo * passInfo) {

	this->VertexShader = vertexShader;
	this->PixelShader = pixelShader;
	this->info = passInfo;

}

 Pass::Pass() { }

void Pass::RenderParams() const {
	for each (auto var in this->PixelShader->Parameters) {
		var.RenderUI();
	}
}

Pass* PassFileLoader::LoadPass( RetroSlang& sl,  PassInfo * passInfo) {
	auto vs = sl.CompileShader(passInfo->ShaderPath, "vs_5_0", "main");
	auto ps = sl.CompileShader(passInfo->ShaderPath, "ps_5_0", "main");

	if (!vs || !ps)
		return nullptr;

	auto vs2 = new Shader(vs.value());
	auto ps2 = new Shader(ps.value());
	auto d =  new Pass(vs2,ps2,passInfo);

	return d;
}
