#pragma once
#include "WOImGuiAbstract.h"
#include "AftrImGuiIncludes.h"
#include "ManagerWindowing.h"

using namespace Aftr;

class GuiText : public WOImGuiAbstract
{
public:
	static GuiText* New(WOGUI* parentWOGUI, float width = 1.0, float height = 1.0)
	{
		GuiText* gui = new GuiText(parentWOGUI);
		gui->onCreate(width, height);
		return gui;
	}

	void drawImGui_for_this_frame()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::SetNextWindowPos(ImVec2(500, 700), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::GetIO().FontGlobalScale = 1.5f;
		ImGui::Begin("HELLO", nullptr, window_flags);

		//int height = ManagerWindowing::getWindowHeight();
		//int width = ManagerWindowing::getWindowWidth();

		std::string height = std::to_string(ManagerWindowing::getScreenHeight());
		std::string width = std::to_string(ManagerWindowing::getScreenWidth());

		//ImVec2 window_pos = ImVec2(window_center_x - window_size.x * 0.5f, window_center_y - window_size.y * 0.5f);


		std::string resolution = height + "x" + width;

		ImGui::Text(resolution.c_str());

		ImGui::End();
	}

protected:
	GuiText(WOGUI* parentWOGUI) : WOImGuiAbstract::WOImGuiAbstract(parentWOGUI), Aftr::IFace(this)
	{
		std::cout << "HELLO" << std::endl;
	}

};
