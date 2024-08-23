#include "CameraShop.h"

using namespace std;

#include "CameraShop.h"

#include "CameraFirstPerson.h"
#include "HandlerMouseState.h"
#include "Model.h"

#include "SDL.h"

#include "AftrManagers.h"
#include "WO.h"
#include "GLView.h"
#include "WorldContainer.h"

#include <chrono>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono;


using namespace Aftr;


CameraShop::CameraShop(GLView* glView, HandlerMouseState* mouseHandler) : IFace(this), Camera(glView, mouseHandler)
{

}

CameraShop::~CameraShop()
{
}

void CameraShop::update()
{
    // begining sequence: generate data -> reelOut Animation -> wait until bite;

}


void CameraShop::onMouseDown(const SDL_MouseButtonEvent& e)
{
}

void Aftr::CameraShop::onMouseWheelScroll(const SDL_MouseWheelEvent& e)
{

    //setGenerateFish(true);

}

void CameraShop::onMouseUp(const SDL_MouseButtonEvent& e)
{
    //CameraStandard::onMouseUp(e);

    //end_timer = std::chrono::high_resolution_clock::now();

    //auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer);

    //std::cout << "Time taken by function: " << duration.count() << " seconds" << std::endl;

    //end_time = true;
}

void CameraShop::onMouseMove(const SDL_MouseMotionEvent& e)
{
    // Update the current position
    SDL_SetRelativeMouseMode(SDL_FALSE);
}
