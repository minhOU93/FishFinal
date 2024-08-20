#include "CameraFirstPerson.h"
#include "HandlerMouseState.h"
#include "Model.h"
#include <limits>
#include "SDL.h"

#include <chrono>
#include <thread>


using namespace Aftr;

void CameraFirstPerson::despawnRod()
{
    if (fishingRod != nullptr) fishingRod->isVisible = false;
}

void CameraFirstPerson::spawnRod()
{
    if (fishingRod != nullptr) fishingRod->isVisible = true;
}

CameraFirstPerson::CameraFirstPerson(GLView* glView, HandlerMouseState* mouseHandler) : IFace(this), Camera(glView, mouseHandler)
{
    SDL_SetRelativeMouseMode(SDL_TRUE);

    this->wheelButtonVelocityScalar = 1.5f;
    this->wheelScrollCounter = 1;
    this->rel_x = 0;
    this->rel_y = 0;
    this->catch_score = 0.0f;
    this->doneTerrain = false;

    this->inventory["Blue Fish"] = 0;
    this->inventory["Long Fin"] = 0;
    this->inventory["Red Fish"] = 0;
    this->inventory["Common Fish"] = 0;

    this->inventory["Money"] = 0;
}

CameraFirstPerson::~CameraFirstPerson()
{
}

void CameraFirstPerson::trackRod()
{
    //ector yolo(this->getCameraLookAtPoint().x + 4, this->getCameraLookAtPoint().y, this->getCameraLookAtPoint().z);
    Vector inFront(this->getCameraLookAtPoint() + (this->getLookDirection() * 0.08));
    Mat4 hello;

    hello.setPosition(Vector(inFront.x, inFront.y, inFront.z));
    hello.setXYZ(this->getPose().getX(), this->getPose().getY(), this->getPose().getZ());

    fishingRod->setPose(hello);

    fishingRod->rotateAboutRelZ(96.2 * DEGtoRAD);

    fishingRod->moveRelative(fishingRod->getModel()->getRelXDir() * -0.41);
} 


void CameraFirstPerson::update()
{
    // CameraStandard::update();

    //if (keystates[SDL_SCANCODE_W])
    //{
    //    Vector noZ(this->getModel()->getRelXDir().x, this->getModel()->getRelXDir().y, 0);
    //    this->setPosition(this->getPosition() + (noZ * this->cameraVelocity * this->wheelButtonVelocityScalar));
    //}
    //else if (keystates[SDL_SCANCODE_A])
    //{
    //    this->moveLeft();

    //}
    //else if (keystates[SDL_SCANCODE_S])
    //{
    //    Vector noZ(this->getModel()->getRelXDir().x, this->getModel()->getRelXDir().y, 0);
    //    this->setPosition(this->getPosition() + (noZ * this->cameraVelocity * -1.0 * this->wheelButtonVelocityScalar));

    //}
    //else if (keystates[SDL_SCANCODE_D])
    //{
    //    this->moveRight();
    //}

    //if(fishingRod != nullptr) trackRod();

    if (keystates[SDL_SCANCODE_W])
    {
        Vector noZ(this->getModel()->getRelXDir().x, this->getModel()->getRelXDir().y, 0);
        Vector dis(noZ * this->cameraVelocity * this->wheelButtonVelocityScalar);
        
        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1 / 60, collisionFilters);
    }
    else if (keystates[SDL_SCANCODE_A])
    {
        Vector dis(this->getModel()->getRelYDir() * this->cameraVelocity * this->wheelButtonVelocityScalar);

        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1 / 60, collisionFilters);

    }
    else if (keystates[SDL_SCANCODE_S])
    {
        Vector noZ(this->getModel()->getRelXDir().x, this->getModel()->getRelXDir().y, 0);
        Vector dis(noZ * -1.0f * this->cameraVelocity * this->wheelButtonVelocityScalar);

        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1 / 60, collisionFilters);

    }
    else if (keystates[SDL_SCANCODE_D])
    {
        Vector dis(this->getModel()->getRelYDir() * this->cameraVelocity * -1.0f * this->wheelButtonVelocityScalar);
        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1 / 60, collisionFilters);

    }


    if (doneTerrain && actor->controller != nullptr && collisionFlags != PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        collisionFlags = actor->controller->move(PxVec3(0, 0, -0.25), 1E-4, 1 / 60, collisionFilters);
    }

    if(actor->controller != nullptr) this->setPosition(actor->controller->getActor()->getGlobalPose().p.x, actor->controller->getActor()->getGlobalPose().p.y, actor->controller->getActor()->getGlobalPose().p.z);

    if(fishingRod != nullptr) trackRod();

}

void CameraFirstPerson::onMouseDown(const SDL_MouseButtonEvent& e)
{
    ////CameraStandard::onMouseDown(e);
    //if (this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
    //{
    //    std::cout << "GO GO GO!!!";

    //    catch_score += 0.01f;

    //    std::cout << catch_score << std::endl;
    //}

}

void Aftr::CameraFirstPerson::onMouseWheelScroll(const SDL_MouseWheelEvent& e)
{
    //Camera::onMouseWheelScroll(e);

    //const Uint8* keystate = SDL_GetKeyboardState(NULL);
    //if (keystate[SDL_SCANCODE_LCTRL])
    //{ // LCTRL IS held down, increment the velocity exponent by +/- 10
    //    int scaleFactor = 10;

    //    if (keystate[SDL_SCANCODE_LALT])
    //        scaleFactor = 100; // If LCTRL and LALT are held, increase exponent by 100

    //    if (e.y > 0)
    //        this->wheelScrollCounter += scaleFactor;
    //    else if (e.y < 0)
    //        this->wheelScrollCounter -= scaleFactor;
    //}
    //else
    //{ // LCTRL is NOT held down, increment the velocity exponent by +/- 1
    //    if (e.y > 0)
    //        ++this->wheelScrollCounter;
    //    else if (e.y < 0)
    //        --this->wheelScrollCounter;
    //}

    ////update the camera velocity scalar
    //if (this->wheelScrollCounter >= 0)
    //    this->wheelButtonVelocityScalar = (float)this->wheelScrollCounter;
    //else
    //    this->wheelButtonVelocityScalar = -1.0f / (float)this->wheelScrollCounter;

    //if (std::abs(this->wheelButtonVelocityScalar) <= std::numeric_limits< float >::min())
    //    this->wheelButtonVelocityScalar = 1.0f;

    //std::cout << "Cam velocity multiplier " << this->wheelButtonVelocityScalar
    //    << ", wheelScroll count " << this->wheelScrollCounter
    //    << ", velocity (m/frame) " << this->cameraVelocity * this->wheelButtonVelocityScalar << "\n";
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

//void CameraFirstPerson::changeLookForObj(int deltaX, int deltaY)
//{
//    if (deltaX == 0 && deltaY == 0)
//        return;
//
//    actorMover->getModel()->rotateAboutGlobalAribitraryAxis(
//        this->axisOfHorizontalRotationViaMouseMotion,
//        (float)(((float)-deltaX) / this->mouseSensitivityDivider));
//
//    actorMover->getModel()->rotateAboutRelY((float)(((float)deltaY) / this->mouseSensitivityDivider));
//
//    //actor->setPose(actorMover->getPose());
//}

void CameraFirstPerson::onMouseMove(const SDL_MouseMotionEvent& e)
{
    // Update the current position
    SDL_GetRelativeMouseState(&rel_x, &rel_y);

    // Checks to see if the camera is looking straight up or down (1 = up, -1 = down)
    float angle = this->getLookDirection().dotProduct(Vector(0, 0, 1));
    
    // if rel_y is negative that means the camera is trying to look up
    if (angle >= 0.979f && rel_y < 0)
    {
        rel_y = 0;
    }
    else if(angle <= -0.979f && rel_y > 0)
    {
        rel_y = 0;
    }

    this->changeLookAtViaMouse(rel_x * 0.5, rel_y * 0.5);

    //if(actorMover != nullptr) changeLookForObj(rel_x * 0.5, rel_y * 0.5);
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
