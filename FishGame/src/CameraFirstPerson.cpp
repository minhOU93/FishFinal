#include "CameraFirstPerson.h"
#include "HandlerMouseState.h"
#include "Model.h"
#include <limits>
#include "SDL.h"
#include <algorithm>

#include <chrono>
#include <thread>


using namespace Aftr;

bool CameraFirstPerson::compare_float(double x, double y, float epsilon) {
    if (fabs(x - y) < epsilon)
        return true; //they are same
    return false; //they are not same
}

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
    this->frequency = 1.8f;
    this->stepCycle = 0.0f;
    this->bobbing = 0;
    this->stopCycle = 1.0 / 60.0;
    stopSway = 0;
    prev_sway_x = 0;
    prev_sway_y = 0;

    this->hasPlayed = false;

    this->inventory["Blue Fish"] = 0;
    this->inventory["Long Fin"] = 0;
    this->inventory["Red Fish"] = 0;
    this->inventory["Common Fish"] = 0;

    this->inventory["Money"] = 0;

    fishData = new std::vector<Fish*>;
    sway = 0;
    initialSway = 0;
    posSway = -0.41;
    initialPosSway = -0.41;

    initialAngleSway = 96.2;
    angleSway = 96.2;
}

CameraFirstPerson::~CameraFirstPerson()
{
}

void CameraFirstPerson::trackRod()
{
    Vector inFront(this->getCameraLookAtPoint() + (this->getLookDirection() * 0.08));
    Mat4 hello;

    hello.setPosition(Vector(inFront.x, inFront.y, inFront.z));

    hello.setXYZ(this->getPose().getX(), this->getPose().getY(), this->getPose().getZ());

    fishingRod->setPose(hello);

    fishingRod->rotateAboutRelZ(angleSway * DEGtoRAD);
    fishingRod->rotateAboutRelX(-sway * DEGtoRAD);
    fishingRod->moveRelative(fishingRod->getModel()->getRelXDir() * initialPosSway);

    fishingRod->rotateAboutRelX(bobbing * 1.13 * DEGtoRAD);
    //fishingRod->rotateAboutRelY(bobbing * 1.0 * DEGtoRAD);
    //fishingRod->rotateAboutRelZ(bobbing * 1.0 * DEGtoRAD);

    fishingRod->moveRelative(fishingRod->getModel()->getRelYDir() * bobbing * 0.01);
    fishingRod->moveRelative(fishingRod->getModel()->getRelZDir() * bobbing * 0.02);
    fishingRod->moveRelative(fishingRod->getModel()->getRelXDir() * bobbing * 0.02);

} 


void CameraFirstPerson::update()
{
    if (prev_sway_x == sway_x && prev_sway_y == sway_y)
    {
        if(!compare_float(sway, initialSway)) sway = std::lerp(sway, initialSway, stopSway * 0.6);
        if(!compare_float(angleSway, initialSway)) angleSway = std::lerp(angleSway, initialAngleSway, stopSway * 0.6);

        stopSway += 1.0 / 60.0f;
    }
    else
    {
        prev_sway_x = sway_x;
        prev_sway_y = sway_y;

        int yea = std::clamp(sway_x, -5, 5);
        int yea2 = std::clamp(sway_y, -5, 5);

        float fr = std::clamp(yea * 0.05, -0.2, 0.3);
        float fr2 = std::clamp(yea2 * 0.05, -0.3, 0.3);

        sway = std::lerp(sway, initialSway + -(fr2 * 60), 1.0 / 40.0);
        angleSway = std::lerp(angleSway, initialAngleSway + -(fr * 40), 1.0 / 60.0);

        stopSway = 0;

    }


    if (keystates[SDL_SCANCODE_W])
    {
        Vector noZ(this->getModel()->getRelXDir().x, this->getModel()->getRelXDir().y, 0);
        Vector dis(noZ * this->cameraVelocity * this->wheelButtonVelocityScalar);
        
        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1.0 / 60, collisionFilters);

        stepCycle += 1.0 / 60.0;
        bobbing = sin(2.0f * PI * stepCycle * frequency) * AMPLITUDE;
        if (bobbing > 0.2f && hasPlayed) hasPlayed = false;
        if (compare_float(-AMPLITUDE, bobbing, 2E-3) && !hasPlayed)
        {
            std::string walkingSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/WALKING2.ogg");
            soundPlayer->play2D(walkingSound.c_str());
            hasPlayed = true;
        }
    }
    else if (keystates[SDL_SCANCODE_A])
    {
        Vector dis(this->getModel()->getRelYDir() * this->cameraVelocity * this->wheelButtonVelocityScalar);

        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1.0 / 60.0, collisionFilters);

        stepCycle += 1.0 / 60.0;
        bobbing = sin(2.0f * PI * stepCycle * frequency) * AMPLITUDE;
        if (compare_float(-AMPLITUDE, bobbing, 2E-3))
        {
            std::string walkingSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/WALKING2.ogg");
            soundPlayer->play2D(walkingSound.c_str());
        }

    }
    else if (keystates[SDL_SCANCODE_S])
    {
        Vector noZ(this->getModel()->getRelXDir().x, this->getModel()->getRelXDir().y, 0);
        Vector dis(noZ * -1.0f * this->cameraVelocity * this->wheelButtonVelocityScalar);

        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1.0 / 60.0, collisionFilters);

        stepCycle += 1.0 / 60.0;
        bobbing = sin(2.0f * PI * stepCycle * frequency) * AMPLITUDE;
        if (compare_float(-AMPLITUDE, bobbing, 2E-3))
        {
            std::string walkingSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/WALKING2.ogg");
            soundPlayer->play2D(walkingSound.c_str());
        }
    }
    else if (keystates[SDL_SCANCODE_D])
    {
        Vector dis(this->getModel()->getRelYDir() * this->cameraVelocity * -1.0f * this->wheelButtonVelocityScalar);
        collisionFlags = actor->controller->move(PxVec3(dis.x, dis.y, dis.z), 1E-4, 1.0 / 60.0, collisionFilters);

        stepCycle += 1.0 / 60.0;
        bobbing = sin(2.0f * PI * stepCycle * frequency) * AMPLITUDE;
        if (compare_float(-AMPLITUDE, bobbing, 2E-3))
        {
            std::string walkingSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/WALKING2.ogg");
            soundPlayer->play2D(walkingSound.c_str());
        }
    }
    else
    {

        if (!compare_float(bobbing, 0.0f, 1E-4))
        {
            bobbing = std::lerp(bobbing, 0.0f, stopCycle);
            stopCycle += 1.0 / 60.0;
        }
        else
        {
            stepCycle = 0.0f;
            stopCycle = 0.0f;
            bobbing = 0;
        }
    }

    if (actor->controller != nullptr && collisionFlags != PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        collisionFlags = actor->controller->move(PxVec3(0, 0, -0.85), 1E-4, 1.0 / 60.0, collisionFilters);
    }


    if(actor->controller != nullptr) this->setPosition(actor->controller->getActor()->getGlobalPose().p.x, actor->controller->getActor()->getGlobalPose().p.y, actor->controller->getActor()->getGlobalPose().p.z + bobbing);
    if(fishingRod != nullptr) trackRod();

}

void CameraFirstPerson::onMouseDown(const SDL_MouseButtonEvent& e)
{
    ////CameraStandard::onMouseDown(e);


    //std::cout << event2.motion.xrel << " " << event2.motion.yrel << std::endl;
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

    sway_x = e.xrel;
    sway_y = e.yrel;


    //std::cout << rel_x << " " << rel_y << std::endl;

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
