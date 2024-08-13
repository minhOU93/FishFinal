#include "CameraFishing.h"

#include "CameraFirstPerson.h"
#include "HandlerMouseState.h"
#include "Model.h"
#include <limits>
#include "SDL.h"
#include <cstdlib> 

#include <chrono>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono;


using namespace Aftr;

bool compare_float(float x, float y, float epsilon = 0.01f) {
    if (fabs(x - y) < epsilon)
        return true; //they are same
    return false; //they are not same
}

CameraFishing::CameraFishing(GLView* glView, HandlerMouseState* mouseHandler) : IFace(this), Camera(glView, mouseHandler)
{
    SDL_SetRelativeMouseMode(SDL_TRUE);
    fish_struggle_times.resize(5);

    fillVectorWithRandomValues(fish_struggle_times, 4, 6);

    this->wheelButtonVelocityScalar = 1.0f;
    this->wheelScrollCounter = 1;
    this->rel_x = 0;
    this->rel_y = 0;

    this->catch_score = 0.0f;
    this->catch_goal = 1.0f;
    this->pole_health = 100.0f;

    this->fish_bite = false;
    this->fish_struggle = false;;

    this->start_time = true;
    this->index = 0;
    this->change_direction = false;
}

CameraFishing::~CameraFishing()
{
}


void CameraFishing::fillVectorWithRandomValues(std::vector<int>& vec, int minValue, int maxValue) {
    // Random number generator
    std::random_device rd;  // Seed
    std::mt19937 gen(rd()); // Mersenne Twister generator
    std::uniform_int_distribution<> dis(minValue, maxValue); // Distribution for integers in the range [minValue, maxValue]

    // Fill the vector with random values
    std::generate(vec.begin(), vec.end(), [&]() { return dis(gen); });
}

void CameraFishing::shakeCamera()
{

    if (!change_direction)
    {
        if (this->getLookDirection().x <= 0.81f)
            this->changeLookAtViaMouse(5, 0);
        else
            change_direction = true;
    }
    else
    {
        if (this->getLookDirection().x >= 0.41f)
            this->changeLookAtViaMouse(-15, 0);
        else
            change_direction = false;
    }

    this->changeLookAtViaMouse(5, 0);

}

void CameraFishing::update()
{

    if (fish_bite)
    {
        if (fish_struggle)
        {
            shakeCamera();
        }

        if (start_time)
        {
            start_timer = std::chrono::high_resolution_clock::now();
            start_time = false;
        }
        else if (!fish_struggle && std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer).count() == fish_struggle_times[index])
        {
            fish_struggle = true;
            start_time = true;
        }

        if (fish_struggle && std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer).count() == 2)
        {
            fish_struggle = false;
            start_time = true;
            index++;
            if (index > 4) { index = 0; }

            //this->setCameraLookDirection(Vector(0.6271f, 0.692003f, -0.263519f));
        }

        if (!fish_struggle && this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
        {

            catch_score += 1.0f;
            std::cout << "CATCH SCORE: " << catch_score << std::endl;

        }
        else if(fish_struggle && this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
        {
            pole_health -= 0.5f;

            std::cout << "HEALTH SCORE: " << pole_health << std::endl;
        }

        if (catch_score == 1000)
        {
            fish_bite = false;
            pole_health = 100.0f;
            start_time = true;
        }

        end_timer = std::chrono::high_resolution_clock::now();

    }

}



void CameraFishing::onMouseDown(const SDL_MouseButtonEvent& e)
{
}

void Aftr::CameraFishing::onMouseWheelScroll(const SDL_MouseWheelEvent& e)
{

    //setGenerateFish(true);
    
}

void CameraFishing::moveInLookDirection()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelXDir() * this->cameraVelocity * this->wheelButtonVelocityScalar));
}

void CameraFishing::moveOppositeLookDirection()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelXDir() * this->cameraVelocity * -1.0f * this->wheelButtonVelocityScalar));
}

void CameraFishing::moveLeft()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelYDir() * this->cameraVelocity * this->wheelButtonVelocityScalar));
}

void CameraFishing::moveRight()
{
    this->setPosition(this->getPosition() + (this->getModel()->getRelYDir() * this->cameraVelocity * -1.0f * this->wheelButtonVelocityScalar));
}

void CameraFishing::onMouseUp(const SDL_MouseButtonEvent& e)
{
    //CameraStandard::onMouseUp(e);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    //end_timer = std::chrono::high_resolution_clock::now();

    //auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer);

    //std::cout << "Time taken by function: " << duration.count() << " seconds" << std::endl;

    //end_time = true;
}

void CameraFishing::onMouseMove(const SDL_MouseMotionEvent& e)
{
    // Update the current position
    SDL_GetRelativeMouseState(&rel_x, &rel_y);

    // Checks to see if the camera is looking straight up or down (1 = up, -1 = down)
    //float angle = this->getLookDirection().dotProduct(Vector(0, 0, 1));

    ////float x_angle = this->getLookDirection().dotProduct(Vector(1, 0, 0));

    ////float y_angle = this->getLookDirection().dotProduct(Vector(0, 1, 0));

    //// if rel_y is negative that means the camera is trying to look up
    //if (angle >= 0.979f && rel_y < 0)
    //{
    //    rel_y = 0;
    //}
    //else if (angle <= -0.979f && rel_y > 0)
    //{
    //    rel_y = 0;
    //}

    if(!fish_struggle) this->changeLookAtViaMouse(rel_x * 0.5, rel_y * 0.5);
}

void CameraFishing::setCameraVelocityMultiplier(float camVelMultiplier)
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