#pragma once
#include "WOImGuiAbstract.h"
#include "AftrImGuiIncludes.h"
#include "ManagerWindowing.h"
#include "ManagerEnvironmentConfiguration.h"

#include "stb/stb_image.h"

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
		//textBox("HIII!!!!!!!");

		//sellMenu();

		if(showProgress) fishCatchProgress(catchProgress, catchGoal);

		if(showHealth) healthBar(health, 15);
	}

	void setHealth(float hp) { health = hp; }
	void setCatchProgress(float progress) { catchProgress = progress; }

	void textBox(std::string text)
	{
		getCenterPosition();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;

		int offset_y = 50;
		int offset_x = 0;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.45f));
		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::GetIO().FontGlobalScale = 1.5f;
		ImGui::Begin("W", nullptr, window_flags);

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), text.c_str());
		ImGui::PopStyleColor();

		ImGui::End();
	}

	void sellMenu()
	{
		getCenterPosition();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

		int offset_y = -150;
		int offset_x = -300;
		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Sell Fish Here", nullptr, window_flags);


		ImGui::Image((void*)(intptr_t)carp_image, ImVec2(200, 150), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Common Carp\n\nAmount: 14");
		ImGui::Separator();
		ImGui::Button("Sell");
		ImGui::SameLine();
		ImGui::Text("$15");
		ImGui::Separator();

		ImGui::End();
	}

	void fishCatchProgress(float current, float total)
	{
		getCenterPosition();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNavInputs;
		int offset_x = -225;
		int offset_y = -25;
		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.45f));
		ImGui::Begin("slider", nullptr, window_flags);

		ImGui::VSliderFloat("##int", ImVec2(18, 160), &current, 0, total, "");
		//ImGui::Text("Catch Progress");
		ImGui::PopStyleColor();

		ImGui::End();
	}

	void healthBar(float current, float total)
	{
		getCenterPosition();
		if (health >= 0.5f)
		{
			float t = (health - 0.5f) / 0.5f;
			ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f * (1.0f - t), 1.0f, 0.0f, 1.0f);
		}
		else
		{
			float t = health / 0.5f;
			ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, (t), 0.0f, 1.0f);
		}
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNavInputs;
		int offset_x = -400 * x_scale;
		int offset_y = -325 * y_scale;
		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("healthbar", nullptr, window_flags);

		ImGui::Text("Line Health");
		ImGui::ProgressBar(current, ImVec2(450.0f, total), "");

		ImGui::End();
	}

	void getCenterPosition()
	{
		int height = ManagerWindowing::getWindowHeight();
		int width = ManagerWindowing::getWindowWidth();

		if (prev_height != height)
			y_scale = height / prev_height;

		if (prev_width != width)
			x_scale = width / prev_width;

		SDL_GetWindowPosition(ManagerWindowing::getCurrentWindow(), window_pos_x, window_pos_y);

		*x = width * 0.5f + (*window_pos_x);
		*y = height * 0.5f + (*window_pos_y);
		
		prev_height = height;
		prev_width = width;

	}

	// Simple helper function to load an image into a OpenGL texture with common settings
	bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
	{
		// Load from file
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;

		// Create a OpenGL texture identifier
		GLuint image_texture;
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D, image_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Upload pixels into texture
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		stbi_image_free(image_data);

		*out_texture = image_texture;
		*out_width = image_width;
		*out_height = image_height;

		return true;
	}

	// Open and read a file, then forward to LoadTextureFromMemory()
	bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height)
	{
		FILE* f = fopen(file_name, "rb");
		if (f == NULL)
			return false;
		fseek(f, 0, SEEK_END);
		size_t file_size = (size_t)ftell(f);
		if (file_size == -1)
			return false;
		fseek(f, 0, SEEK_SET);
		void* file_data = IM_ALLOC(file_size);
		fread(file_data, 1, file_size, f);
		bool ret = LoadTextureFromMemory(file_data, file_size, out_texture, out_width, out_height);
		IM_FREE(file_data);
		return ret;
	}

	float catchGoal;
	bool showHealth;
	bool showProgress;

	bool showVictoryText;

protected:
	GuiText(WOGUI* parentWOGUI) : WOImGuiAbstract::WOImGuiAbstract(parentWOGUI), Aftr::IFace(this)
	{

		//glewInit();
		std::cout << "HELLO" << std::endl;

		x = new int;
		y = new int;

		window_pos_x = new int;
		window_pos_y = new int;

		my_image_width = 0;
		my_image_height = 0;

		std::string bait(ManagerEnvironmentConfiguration::getLMM() + "images/carp.png");
		bool ret = LoadTextureFromFile(bait.c_str(), &carp_image, &my_image_width, &my_image_height);
		auto pic = load_image_from_file(bait);
		IM_ASSERT(ret);

		prev_height = ManagerWindowing::getWindowHeight();
		prev_width = ManagerWindowing::getWindowWidth();

		health = 1.0f;
		catchProgress = 0;

		catchGoal = 1.0f;
		showHealth = false;
		showProgress = false;
		timer = 0;
	}

	int* x;
	int* y;

	int timer;

	int* window_pos_x;
	int* window_pos_y;

	float prev_height;
	float prev_width;

	int my_image_width;
	int my_image_height;

	float x_scale = 1;
	float y_scale = 1;

	float hello;

	float health;
	float catchProgress;

	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;

	GLuint carp_image = 0;

};
