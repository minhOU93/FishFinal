#pragma once

#include "CameraStandard.h"

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

	protected:
		float wheelButtonVelocityScalar;
		int wheelScrollCounter;

		int rel_x;
		int rel_y;
	};
}