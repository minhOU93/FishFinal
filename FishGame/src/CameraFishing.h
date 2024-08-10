#pragma once

#include "CameraStandard.h"
#include <algorithm>
#include <random>

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

		void fillVectorWithRandomValues(std::vector<int>& vec, int minValue, int maxValue);
		void shakeCamera();

		void setGenerateFish(bool awesome) { generate_fish = awesome; }
		void setFishBite(bool activate) { fish_bite = activate; }

	protected:
		float wheelButtonVelocityScalar;
		int wheelScrollCounter;

		const Uint8* keystates = SDL_GetKeyboardState(NULL);

		int rel_x;
		int rel_y;

		float catch_score;
		float catch_goal;
		float pole_health;

		bool fish_bite;
		bool fish_struggle;
		std::vector<int> fish_struggle_times;

		bool generate_fish;
		bool start_time;
		bool end_time;
		bool change_direction;

		int index;

		std::chrono::steady_clock::time_point start_timer;
		std::chrono::steady_clock::time_point end_timer;
	};
}