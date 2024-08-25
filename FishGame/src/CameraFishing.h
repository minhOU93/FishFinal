#pragma once

#include "CameraStandard.h"
#include <algorithm>
#include <random>
#include "GuiText.h"
#include "Fish.h"
#include <cstdlib> 
#include <limits>
#include "CameraFirstPerson.h"
#include "irrKlang.h"

namespace Aftr
{
	class CameraFishing : public Camera
	{
	public:
		CameraFishing(GLView* glView, HandlerMouseState* mouseHandler);
		virtual ~CameraFishing();

		CameraFishing() = delete;
		CameraFishing(const CameraFishing& toCopy) = delete;
		CameraFishing& operator=(CameraFishing& toAssign) = delete;
		CameraFishing& operator=(CameraFishing&& toMove) = delete;

		virtual void update();

		virtual void onMouseDown(const SDL_MouseButtonEvent& e) override;
		virtual void onMouseUp(const SDL_MouseButtonEvent& e);
		virtual void onMouseWheelScroll(const SDL_MouseWheelEvent& e);
		virtual void onMouseMove(const SDL_MouseMotionEvent& e) override;
		virtual void moveInLookDirection();
		virtual void moveOppositeLookDirection();
		virtual void moveLeft(); ///< Pans camera to the 'left' (along camera's local +y axis)
		virtual void moveRight(); ///< Pans camera to the 'right' (along camera's local -y axis)
		virtual void setCameraVelocityMultiplier(float camVelMultiplier); ///< Multiplier must be greater than zero
		
		
		void victoryScreen();
		void returnFromVictory();

		void fillVectorWithRandomValues(std::vector<int>& vec, int minValue, int maxValue);
		void shakeCamera();

		void setGenerateFish(bool awesome) { generate_fish = awesome; }
		void setFishBite(bool activate) { fish_bite = activate; }
		void setBeginGame(bool yes) { begin = yes; }
		void despawnRod();
		void spawnRod();
		void resetGame();
		bool normalizeCamera();

		void reelIn();
		void reelOut();

		GuiText* gui;

		int generateRandomNumber(int min, int max);

		bool failGame;
		bool startGame;
		bool showVictory;
		bool returnVictory;
		bool allowExit;
		Mat4 saveFishPose;

		CameraFirstPerson* player;
		std::vector<Fish*> fishes;
		irrklang::ISoundEngine* soundPlayer;


		//irrklang::ISoundSource* reelInSFX;
		irrklang::ISound* playReelIn;
		irrklang::ISound* playReelOut;
		irrklang::ISound* playFishStruggle;

	protected:
		float wheelButtonVelocityScalar;
		int wheelScrollCounter;

		const Uint8* keystates = SDL_GetKeyboardState(NULL);

		int rel_x;
		int rel_y;
		int angleVictory;

		//std::vector<Fish*> fishes;

		float catch_score;
		float catch_goal;
		float pole_health;
		float reelSpeed;

		bool begin;
		bool fish_bite;
		bool fish_play;
		bool fish_struggle;
		std::vector<int> fish_struggle_times;
		int fishStruggleTime;

		std::vector<WO*> fishingRod;
		std::vector<WO*> fishingLines;


		bool generate_fish;
		bool start_time;
		bool end_time;
		bool change_direction;
		bool reelOutStatus;
		bool startWait;

		bool endGame;
		WO* baitPointer;

		int index;
		int reel_index;

		float reelCheck;

		int waitTime;
		int fishIndex;

		float camLookX;
		float camLookY;
		float camLookZ;

		std::random_device rd;
		std::mt19937 gen;

		std::chrono::steady_clock::time_point start_timer;
		std::chrono::steady_clock::time_point end_timer;
	};
}