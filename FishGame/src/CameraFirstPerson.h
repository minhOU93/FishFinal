#pragma once

#include "CameraStandard.h"
#include "WO.h"
#include <map>
#include "WOPxController.h"
#include "Fish.h"
#include "irrKlang.h"

namespace Aftr
{
	class CameraFirstPerson : public Camera
	{
	public:
		CameraFirstPerson(GLView* glView, HandlerMouseState* mouseHandler);
		virtual ~CameraFirstPerson();

		CameraFirstPerson() = delete;
		CameraFirstPerson(const CameraFirstPerson& toCopy) = delete;
		CameraFirstPerson& operator=(CameraFirstPerson& toAssign) = delete;
		CameraFirstPerson& operator=(CameraFirstPerson&& toMove) = delete;

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

		void changeLookForObj(int deltaX, int deltaY);

		void trackRod();
		void setRod(WO* rod) { fishingRod = rod; }
		void despawnRod();
		void spawnRod();

		bool compare_float(double x, double y, float epsilon = 0.01f);

		std::map<std::string, int> inventory;

		//WOPxObj* actor;
		WO* actorMover;

		WOPxController* actor;
		physx::PxControllerCollisionFlags collisionFlags;
		physx::PxControllerFilters collisionFilters;


		bool doneTerrain;

		std::vector<Fish*>* fishData;
		irrklang::ISoundEngine* soundPlayer;

	protected:
		float wheelButtonVelocityScalar;
		int wheelScrollCounter;

		const Uint8* keystates = SDL_GetKeyboardState(NULL);

		int rel_x;
		int rel_y;

		long double bobbing;
		long double stepCycle;
		double stopCycle;
		float frequency;
		bool hasPlayed;

		const long double AMPLITUDE = 0.5f;

		float catch_score;

		WO* fishingRod = nullptr;
	};
}