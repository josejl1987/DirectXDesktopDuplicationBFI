#include "Pass.h"

void Pass::RenderParams() const {
	for each ( auto var in this->PixelShader.Parameters) {
		var.RenderUI();
	}
}
