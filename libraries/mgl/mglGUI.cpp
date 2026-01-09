#include "mglGUI.hpp"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

namespace mgl {

	void GUI::initGUI() {
		io = ImGui::GetIO();
	}

	void GUI::enableKeyboardNavigationGUI(bool enable) {
		if (enable) {
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  
		}
		else {
			io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard; 
		}
	}

	void GUI::renderWindowGUI() {
		if (state.showWindow) {
			ImGui::Begin("MGL GUI", &state.showWindow);
			ImGui::Text("Graphics Settings");
			ImGui::Checkbox("Enable Normal-Mapping", &state.enableNormalMapping);
			ImGui::Checkbox("Enable Ambient Occlusion", &state.enableAmbientOcclusion);
			ImGui::InputFloat("Gamma", &state.gamma, 0.1, 1.0, "%.2f");
			ImGui::InputFloat("Exposure", &state.exposure, 0.1, 1.0, "%.2f");

			processWindowCallbacksGUI();
			
			ImGui::End();
		}
	}

	void GUI::generateBuffersGUI() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GUI::renderGUI() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GUI::processGlobalCallbacksGUI() {
		if (!io.WantCaptureKeyboard) {

		}

		if (!io.WantCaptureMouse) {
		
		}
	}

	void GUI::processWindowCallbacksGUI() {
		if (ImGui::IsWindowFocused) {

		}
	}

	GUIState GUI::getState() const {
		return this->state;
	}

	void GUI::enableWindow(bool enable) {
		this->state.showWindow = enable;
	}

	bool GUI::guiWantsKeyboard() const {
		return io.WantCaptureKeyboard;
	}
	bool GUI::guiWantsMouse() const {
		return io.WantCaptureMouse;
	}
}
