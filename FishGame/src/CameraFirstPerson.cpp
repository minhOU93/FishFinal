#include "CameraFirstPerson.h"
#include "HandlerMouseState.h"
#include "Model.h"
#include <limits>
#include "SDL.h"


using namespace Aftr;

CameraFirstPerson::CameraFirstPerson(GLView* glView, HandlerMouseState* mouseHandler) : IFace(this), Camera(glView, mouseHandler)
{
    SDL_SetRelativeMouseMode(SDL_TRUE);

    this->wheelButtonVelocityScalar = 1.0f;
    this->wheelScrollCounter = 1;
    this->rel_x = 0;
    this->rel_y = 0;
}

CameraFirstPerson::~CameraFirstPerson()
{
}


void CameraFirstPerson::update()
{
    // CameraStandard::update();

    //if (this->mouseHandler != NULL && this->mouseHandler->isMouseDownRightButton())
    //{
    //    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    //    if (keystate[SDL_SCANCODE_LSHIFT])
    //        Camera::moveOppositeLookDirection(this->getCameraVelocity() * this->wheelButtonVelocityScalar);
    //    else
    //        Camera::moveInLookDirection(this->getCameraVelocity() * this->wheelButtonVelocityScalar);
    //}
}

void CameraFirstPerson::onMouseDown(const SDL_MouseButtonEvent& e)
{
    //CameraStandard::onMouseDown(e);

    if (this->mouseHandler != NULL && this->mouseHandler->isMouseDownMiddleButton())
    {
        //reset the wheelScrollCounter
        this->wheelScrollCounter = 1;
        this->wheelButtonVelocityScalar = 1.0f;
        std::cout << "Reset Cam velocity multiplier due to middle button click...\n";
    }
}

void Aftr::CameraFirstPerson::onMouseWheelScroll(const SDL_MouseWheelEvent& e)
{
    Camera::onMouseWheelScroll(e);

    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_LCTRL])
    { // LCTRL IS held down, increment the velocity exponent by +/- 10
        int scaleFactor = 10;

        if (keystate[SDL_SCANCODE_LALT])
            scaleFactor = 100; // If LCTRL and LALT are held, increase exponent by 100

        if (e.y > 0)
            this->wheelScrollCounter += scaleFactor;
        else if (e.y < 0)
            this->wheelScrollCounter -= scaleFactor;
    }
    else
    { // LCTRL is NOT held down, increment the velocity exponent by +/- 1
        if (e.y > 0)
            ++this->wheelScrollCounter;
        else if (e.y < 0)
            --this->wheelScrollCounter;
    }

    //update the camera velocity scalar
    if (this->wheelScrollCounter >= 0)
        this->wheelButtonVelocityScalar = (float)this->wheelScrollCounter;
    else
        this->wheelButtonVelocityScalar = -1.0f / (float)this->wheelScrollCounter;

    if (std::abs(this->wheelButtonVelocityScalar) <= std::numeric_limits< float >::min())
        this->wheelButtonVelocityScalar = 1.0f;

    std::cout << "Cam velocity multiplier " << this->wheelButtonVelocityScalar
        << ", wheelScroll count " << this->wheelScrollCounter
        << ", velocity (m/frame) " << this->cameraVelocity * this->wheelButtonVelocityScalar << "\n";
}

void CameraFirstPerson::moveInLookDirection()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelXDir() * this->cameraVelocity * this->wheelButtonVelocityScalar));
}

void CameraFirstPerson::moveOppositeLookDirection()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelXDir() * this->cameraVelocity * -1.0f * this->wheelButtonVelocityScalar));
}

void CameraFirstPerson::moveLeft()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelYDir() * this->cameraVelocity * this->wheelButtonVelocityScalar));
}

void CameraFirstPerson::moveRight()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelYDir() * this->cameraVelocity * -1.0f * this->wheelButtonVelocityScalar));
}

void CameraFirstPerson::onMouseUp(const SDL_MouseButtonEvent& e)
{
    //CameraStandard::onMouseUp(e);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void CameraFirstPerson::onMouseMove(const SDL_MouseMotionEvent& e)
{
    //SDL_SetRelativeMouseMode(SDL_TRUE);

    //if (this->getParentWorldObject() != NULL && this->isLockedWRTparent())
    //{
    //    if (this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
    //    {
    //        //check for CRTL key
    //        const Uint8* keystate = SDL_GetKeyboardState(NULL);
    //        if (keystate[SDL_SCANCODE_LCTRL])
    //        {
    //            int dy = this->mouseHandler->getMouseMotionDeltaY();
    //            float radianDeltaPhi = ((float)dy) * this->jointMouseMotionSensitivity;
    //            this->changeJointPhi(radianDeltaPhi);
    //        }
    //        else
    //        {
    //            int dx = this->mouseHandler->getMouseMotionDeltaX();
    //            float radianDeltaTheta = ((float)dx) * this->jointMouseMotionSensitivity;
    //            this->changeJointTheta(radianDeltaTheta);

    //            int dy = this->mouseHandler->getMouseMotionDeltaY();
    //            this->setDistFromJointToCam(this->getDistFromJointToCam() + ((float)dy / 100.0f));
    //        }
    //    }
    //}


    // Update the current position
    SDL_GetRelativeMouseState(&rel_x, &rel_y);

    // Calculate the relative movement

    std::cout << "X: " << rel_x;
    std::cout << "Y: " << rel_y;

    this->changeLookAtViaMouse(rel_x, rel_y);
}

void CameraFirstPerson::setCameraVelocityMultiplier(float camVelMultiplier)
{
    this->wheelButtonVelocityScalar = fabs(camVelMultiplier);

    if (fabs(this->wheelButtonVelocityScalar) >= 1.0f)
    {
        this->wheelScrollCounter = (unsigned int)this->wheelButtonVelocityScalar;
    }
    else
    {
        this->wheelScrollCounter = (int)(1.0f / this->wheelButtonVelocityScalar);
    }


    //this->wheelScrollCounter = camVelMultiplier;

    //if( this->wheelScrollCounter >= 0 )
    //   this->wheelButtonVelocityScalar = (float)this->wheelScrollCounter;//deltaWheelButtonVelocityScalarPerWheelScroll;
    //else
    //   this->wheelButtonVelocityScalar = -1.0f / (float)this->wheelScrollCounter;

    //if( fabs( this->wheelButtonVelocityScalar ) <= FLT_MIN )
    //   this->wheelButtonVelocityScalar = 1.0f;

    std::cout << "Cam velocity multiplier " << this->wheelButtonVelocityScalar
        << ", wheelScroll count " << this->wheelScrollCounter << "\n";
}
