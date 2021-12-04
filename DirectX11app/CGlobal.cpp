#include "CGlobal.h"
#include "D3D11RenderManager.h"
#include "imgui/imgui.h"

void CGlobal::RenderShaderList() {
	if (ImGui::Button("Select shader")) {
		this->fileDialog.Open();
	}
	this->fileDialog.Display();

	if (fileDialog.HasSelected()) {
		auto path = fileDialog.GetSelected().string();
		this->render->LoadShader(path);
		fileDialog.ClearSelected();
	}
}

void CGlobal::RenderDisplayInfo() {
	int i = 0;
	ImGui::Text("%f", ImGui::GetIO().Framerate);
	ImGui::SliderInt("BFI interval", &this->intervalBFI, 0, 16);
	for (auto a : this->displayInfo.Adapters) {
		if (a.Outputs.size() == 0)
			continue;
		ImGui::Text(a.Name.c_str());

		for (auto b : a.Outputs) {
			ImGui::Text(b.DeviceName.c_str());
			i++;
			ImGui::PushID(i);
			ImGui::SameLine();
			if (ImGui::Button("Mirror")) {
				this->initIndex = b.Index;
			}
			ImGui::PopID();
		}
	}
}
