#pragma once
#include "WOImGuiAbstract.h"
#include "AftrImGuiIncludes.h"
#include "ManagerWindowing.h"
#include "ManagerEnvironmentConfiguration.h"
#include "CameraFirstPerson.h"
#include "Cat.h"

#include <algorithm>
#include <random>
#include <cstdlib> 
#include <limits>
#include "irrKlang.h"

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

	int generateRandomNumber(int min, int max) {
		// Initialize a random device and a random number generator
		std::random_device rd;
		std::mt19937 gen(rd());

		// Define the range for the random number
		std::uniform_int_distribution<> distr(min, max);

		// Generate and return the random number
		return distr(gen);
	}

	void drawImGui_for_this_frame()
	{
		if(showFish) textBox(indicator);

		if(showShopText) textBox(indicator);

		if (resetDialog)
		{
			resetDialog = false;
			timer = 0;
			victoryText = "";
			disapperTimer = 0;
			answer = "";
			wordLength = 1;
			realSpeed = 1;
			puncChecker = 0;
			startIndex = 0;
			playTalk->setIsPaused(true);
			playWinSound->setIsPaused(true);
		}

	    if(showShop) sellMenu();

		if(showProgress) fishCatchProgress(catchProgress, catchGoal);

		if(showHealth) healthBar(health, 15);

		if (showVictoryText)
		{
			streamText(victoryText, 0, 125, 2);
		}

		if (showDialog)
		{
			streamDialog(dialog, 300, 100, 1, true);
		}

		if(showInventory) inventoryGui();

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

	void streamText(std::string text, int dx = 0, int dy = 125, int speed = 10)
	{
		getCenterPosition();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;

		ImGui::GetIO().FontGlobalScale = 1.5f;

		int offset_y = dy;
		int offset_x = dx;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.45f));
		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::Begin("W", nullptr, window_flags);
		if (timer != speed)
		{
			timer++;
		}
		else
		{
			wordLength++;
			if (wordLength <= text.size())
			{
				answer = text.substr(0, wordLength);
				playWinSound->setIsPaused(false);
			}
			timer = 0;
		}

		if (wordLength >= text.size())
		{
			playWinSound->setIsPaused(true);
		}

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), answer.c_str());

		ImGui::PopStyleColor();
		ImGui::End();
	}

	void streamDialog(std::string text, int dx = 0, int dy = 125, int speed = 10, bool disapper = false)
	{
		getCenterPosition();

		//if (resetDialog)
		//{
		//	resetDialog = false;
		//	timer = 0;
		//	disapperTimer = 0;
		//	answer = "";
		//	wordLength = 1;
		//	realSpeed = 1;
		//	puncChecker = 0;
		//}
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;

		int offset_y = dy;
		int offset_x = dx;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.45f));
		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::Begin("Dialog", nullptr, window_flags);

		ImGui::GetStyle().WindowRounding = 15.0f;

		if (timer != realSpeed)
		{
			timer++;
		}
		else
		{
			wordLength++;
			puncChecker++;
			if (realSpeed == 34 && puncChecker != text.size())
			{
				startIndex = puncChecker;
				wordLength = 1;
				realSpeed = speed;
				playTalk->setIsPaused(true);
			}
			if (puncChecker <= text.size())
			{
				answer = text.substr(startIndex, wordLength);
				if (puncChecker % 2 == 0)
				{
					catDialog->moveMouth();
					playTalk->setIsPaused(false);
				}
			}
			timer = 0;
		}

		if (wordLength != 0 && puncChecker < text.size() && (text[puncChecker] == '!' || text[puncChecker] == '.' || text[puncChecker] == '?'))
		{
			realSpeed = 34;
			catDialog->mouthOpen = true;
			catDialog->moveMouth();
			playTalk->setIsPaused(true);
		}
		else realSpeed = speed;

		if (disapper && puncChecker >= text.size())
		{
			disapperTimer += 1;
			catDialog->mouthOpen = true;
			catDialog->moveMouth();
			playTalk->setIsPaused(true);
		}

		if (disapper && disapperTimer == 150)
		{
			showDialog = false;
			dialog = "";
			timer = 0;
			wordLength = 1;
			disapperTimer = 0;
			answer = "";
			realSpeed = 1;
			startIndex = 0;
			puncChecker = 0;
		}

		ImGui::PushTextWrapPos(400.0f);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), answer.c_str());
		ImGui::PopTextWrapPos();

		ImGui::PopStyleColor();
		ImGui::End();
	}

	void sellMenu()
	{
		getCenterPosition();
		ImGui::GetIO().FontGlobalScale = 1.2f;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
		int offset_y = 0; // -150;
		int offset_x = -280;
		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImVec2 window_size = ImVec2(500, 600); // Width: 800, Height: 600
		ImGui::SetNextWindowSize(window_size);
		std::string titleMoney = "Money: $" + std::to_string(player->inventory["Money"]);
		ImGui::Begin("Shop", nullptr, window_flags);
		ImGui::Text(titleMoney.c_str());

		ImGui::BeginChild("ScrollableRegion", ImVec2(0, 0), true);
		// CARP
        std::string carp_amount = std::to_string(player->inventory[player->fishData->at(1)->name]);

		ImGui::Image((void*)(intptr_t)carp_image, ImVec2(200, 150), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("Common Carp\n\nAmount: " + carp_amount).c_str());
		ImGui::Separator();
		if (ImGui::Button("Sell Common Carp"))
		{
			if (player->inventory[player->fishData->at(1)->name] > 0)
			{
				resetDialog = true;

				player->inventory[player->fishData->at(1)->name]--;
				player->inventory["Money"] += player->fishData->at(1)->price;

				dialog = catDialog->carpDialog[generateRandomNumber(0, catDialog->carpDialog.size() - 1)];

				showDialog = true;
			}
		}
		if (ImGui::IsItemHovered())
		{
			if (!hoveredButtons[0])
			{
				playHover = true;
				hoveredButtons[0] = true;
			}
		}
		else
		{
			hoveredButtons[0] = false;
		}
		ImGui::SameLine();
        ImGui::Text(("$" + std::to_string(player->fishData->at(1)->price)).c_str());
		ImGui::Separator();

		// LONG FIN
		std::string fin_amount = std::to_string(player->inventory[player->fishData->at(2)->name]);

		ImGui::Image((void*)(intptr_t)long_fin, ImVec2(200, 150), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("Long Fin\n\nAmount: " + fin_amount).c_str());
		ImGui::Separator();
		if (ImGui::Button("Sell Long Fin"))
		{
			if (player->inventory[player->fishData->at(2)->name] > 0)
			{
				resetDialog = true;

				player->inventory[player->fishData->at(2)->name]--;
				player->inventory["Money"] += player->fishData->at(2)->price;

				dialog = catDialog->longDialog[generateRandomNumber(0, catDialog->longDialog.size() - 1)];

				showDialog = true;
			}
		}
		if (ImGui::IsItemHovered())
		{
			if (!hoveredButtons[1])
			{
				playHover = true;
				hoveredButtons[1] = true;
			}
		}
		else
		{
			hoveredButtons[1] = false;
		}
		ImGui::SameLine();
		ImGui::Text(("$" + std::to_string(player->fishData->at(2)->price)).c_str());
		ImGui::Separator();

		// RED FISH
		std::string red_amount = std::to_string(player->inventory[player->fishData->at(3)->name]);

		ImGui::Image((void*)(intptr_t)red_fish, ImVec2(200, 150), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("Red Fish\n\nAmount: " + red_amount).c_str());
		ImGui::Separator();
		if (ImGui::Button("Sell Red Fish"))
		{
			if (player->inventory[player->fishData->at(3)->name] > 0)
			{
				resetDialog = true;

				player->inventory[player->fishData->at(3)->name]--;
				player->inventory["Money"] += player->fishData->at(3)->price;

				dialog = catDialog->redDialog[generateRandomNumber(0, catDialog->redDialog.size() - 1)];

				showDialog = true;
			}
		}
		if (ImGui::IsItemHovered())
		{
			if (!hoveredButtons[2])
			{
				playHover = true;
				hoveredButtons[2] = true;
			}
		}
		else
		{
			hoveredButtons[2] = false;
		}
		ImGui::SameLine();
		ImGui::Text(("$" + std::to_string(player->fishData->at(3)->price)).c_str());
		ImGui::Separator();

		// BLUE FISH
		std::string blue_amount = std::to_string(player->inventory[player->fishData->at(0)->name]);

		ImGui::Image((void*)(intptr_t)blue_fish, ImVec2(200, 150), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("Blue Fish\n\nAmount: " + blue_amount).c_str());
		ImGui::Separator();
		if (ImGui::Button("Sell Blue Fish"))
		{
			if (player->inventory[player->fishData->at(0)->name] > 0)
			{
				resetDialog = true;

				player->inventory[player->fishData->at(0)->name]--;
				player->inventory["Money"] += player->fishData->at(0)->price;

				dialog = catDialog->blueDialog[generateRandomNumber(0, catDialog->blueDialog.size() - 1)];

				showDialog = true;
			}
		}
		if (ImGui::IsItemHovered())
		{
			if (!hoveredButtons[3])
			{
				playHover = true;
				hoveredButtons[3] = true;
			}
		}
		else
		{
			hoveredButtons[3] = false;
		}
		ImGui::SameLine();
		ImGui::Text(("$" + std::to_string(player->fishData->at(0)->price)).c_str());
		ImGui::EndChild();
		if (playHover)
		{
			std::string shopSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/ITEM_HOVER.ogg");
			soundPlayer->play2D(shopSound.c_str());
			playHover = false;
		}
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
		ImGui::GetIO().FontGlobalScale = 1.0f;
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

	void inventoryGui()
	{
		getCenterPosition();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
		ImGui::GetIO().FontGlobalScale = 1.2f;
		int offset_y = 0;
		int offset_x = 0;

		ImGui::SetNextWindowPos(ImVec2(*x + offset_x, *y + offset_y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImVec2 window_size = ImVec2(400, 400); // Width: 800, Height: 600
		ImGui::SetNextWindowSize(window_size);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.75f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.37f, 0.0f, 0.24f, 0.75f));
		ImGui::Begin("Inventory", nullptr, window_flags);

		std::string titleMoney = "Money: $" + std::to_string(player->inventory["Money"]);

		ImGui::Text(titleMoney.c_str());

		ImGui::BeginChild("Yesss", ImVec2(0, 0), true);
		for (auto it : player->inventory)
		{
			if (it.first != "Money" && it.second != 0)
			{
				std::string layout = it.first + ": " + std::to_string(it.second);
				ImGui::Text(layout.c_str());
				ImGui::Separator();
			}

		}
		ImGui::EndChild();

		ImGui::PopStyleColor(2);
		ImGui::End();
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
	bool showDialog;
	bool resetDialog;

	bool showFish;
	bool showShop;
	bool showShopText;

	bool showInventory;
	bool playHover;

	std::string victoryText;
	std::string dialog;

	std::string indicator;

	CameraFirstPerson* player;
	Cat* catDialog;

	irrklang::ISound* playTalk;
	irrklang::ISound* playWinSound;
	irrklang::ISoundEngine* soundPlayer;

protected:
	GuiText(WOGUI* parentWOGUI) : WOImGuiAbstract::WOImGuiAbstract(parentWOGUI), Aftr::IFace(this)
	{

		//glewInit();

		playHover = false;

		x = new int;
		y = new int;

		window_pos_x = new int;
		window_pos_y = new int;

		showInventory = false;
		indicator = "";
		showShopText = false;

		my_image_width = 0;
		my_image_height = 0;
		disapperTimer = 0;
		realSpeed = 1;

		resetDialog = false;

		hoveredButtons.resize(4);
		for (int i = 0; i < hoveredButtons.size(); i++)
		{
			hoveredButtons[i] = false;
		}

		std::string bait(ManagerEnvironmentConfiguration::getLMM() + "images/carp.png");
		std::string blueFish(ManagerEnvironmentConfiguration::getLMM() + "images/blue_fish.png");
		std::string longFin(ManagerEnvironmentConfiguration::getLMM() + "images/long_fin.png");
		std::string redFish(ManagerEnvironmentConfiguration::getLMM() + "images/red_fish.png");

		{
			bool ret = LoadTextureFromFile(bait.c_str(), &carp_image, &my_image_width, &my_image_height);
			auto pic = load_image_from_file(bait);
			IM_ASSERT(ret);
		}

		{
			bool ret = LoadTextureFromFile(blueFish.c_str(), &blue_fish, &my_image_width, &my_image_height);
			auto pic = load_image_from_file(blueFish);
			IM_ASSERT(ret);
		}

		{
			bool ret = LoadTextureFromFile(longFin.c_str(), &long_fin, &my_image_width, &my_image_height);
			auto pic = load_image_from_file(longFin);
			IM_ASSERT(ret);
		}

		{
			bool ret = LoadTextureFromFile(redFish.c_str(), &red_fish, &my_image_width, &my_image_height);
			auto pic = load_image_from_file(redFish);
			IM_ASSERT(ret);
		}

		prev_height = ManagerWindowing::getWindowHeight();
		prev_width = ManagerWindowing::getWindowWidth();

		health = 1.0f;
		catchProgress = 0;
		startIndex = 0;

		catchGoal = 1.0f;
		showHealth = false;
		showProgress = false;
		timer = 0;
		showVictoryText = false;
		wordLength = 1;
		answer = "";
		showDialog = false;
		puncChecker = 0;

		showShop = false;
		showFish = false;
	}

	int* x;
	int* y;

	int timer;
	int wordLength;
	int startIndex;
	std::string answer;
	int realSpeed;
	int puncChecker;
	std::vector<bool> hoveredButtons;

	GuiText* yo = nullptr;

	int* window_pos_x;
	int* window_pos_y;

	float prev_height;
	float prev_width;

	int my_image_width;
	int my_image_height;
	int disapperTimer;

	float x_scale = 1;
	float y_scale = 1;

	float hello;

	float health;
	float catchProgress;

	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;

	GLuint carp_image = 0;
	GLuint blue_fish = 0;
	GLuint red_fish = 0;
	GLuint long_fin = 0;

};
