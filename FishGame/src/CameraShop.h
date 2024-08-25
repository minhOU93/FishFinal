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
	class CameraShop : public Camera
	{
	public:
		CameraShop(GLView* glView, HandlerMouseState* mouseHandler);
		virtual ~CameraShop();

		CameraShop() = delete;
		CameraShop(const CameraShop& toCopy) = delete;
		CameraShop& operator=(CameraShop& toAssign) = delete;
		CameraShop& operator=(CameraShop&& toMove) = delete;

		virtual void update();

		virtual void onMouseDown(const SDL_MouseButtonEvent& e) override;
		virtual void onMouseUp(const SDL_MouseButtonEvent& e);
		virtual void onMouseWheelScroll(const SDL_MouseWheelEvent& e);
		virtual void onMouseMove(const SDL_MouseMotionEvent& e) override;

		void setReltoTrue();
		void setReltoFalse();

		int save_x;
		int save_y;

		irrklang::ISoundEngine* soundPlayer;
	};
}