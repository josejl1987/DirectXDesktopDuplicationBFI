#include "CGlobal.h"
#include "imgui/imgui.h"

void CGlobal::RenderDisplayInfo()
{

	int i = 0;
	ImGui::Text("%f", ImGui::GetIO().Framerate);
	ImGui::SliderInt("BFI interval", &this->intervalBFI, 0, 16);
	for (auto a : this->displayInfo.Adapters) {
		if (a.Outputs.size() == 0) continue;
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

