#ifndef MGL_GUI_HPP
#define MGL_GUI_HPP

#include <imgui.h>

namespace mgl {

	struct GUIState {
		bool showWindow = true;
		bool enableNormalMapping = true;
		bool enableAmbientOcclusion = true;
		float gamma = 2.2f;
		float exposure = 1.0f;
	};

	class GUI {
	public:	
		GUI() = default;
		~GUI() = default;

		void initGUI();
		void renderGUI();
		void enableWindow(bool enable);
		void renderWindowGUI();
		void generateBuffersGUI();
		void enableKeyboardNavigationGUI(bool enable);
		void processGlobalCallbacksGUI();
		void processWindowCallbacksGUI();
		bool guiWantsKeyboard() const;
		bool guiWantsMouse() const;
		GUIState getState() const;

	private:
		ImGuiIO io;
		GUIState state;
	};

}


#endif // MGL_GUI_HPP