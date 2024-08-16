#include "CameraFishing.h"

#include "CameraFirstPerson.h"
#include "HandlerMouseState.h"
#include "Model.h"
#include <limits>
#include "SDL.h"
#include <cstdlib> 

#include "AftrManagers.h"
#include "WO.h"
#include "GLView.h"
#include "WorldContainer.h"

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


int generateRandomNumber(int min, int max) {
    // Initialize a random device and a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the range for the random number
    std::uniform_int_distribution<> distr(min, max);

    // Generate and return the random number
    return distr(gen);
}

void CameraFishing::despawnRod()
{
    for (int i = 0; i < fishingLines.size(); i++)
    {
        fishingLines[i]->isVisible = false;
    }

    for (int i = 0; i < fishingRod.size(); i++)
    {
        fishingRod[i]->isVisible = false;
    }
}


void CameraFishing::spawnRod()
{
    for (int i = 0; i < fishingLines.size(); i++)
    {
        fishingLines[i]->isVisible = true;
    }

    for (int i = 0; i < fishingRod.size(); i++)
    {
        fishingRod[i]->isVisible = true;
    }
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

    this->begin = false;
    this->catch_score = 0.0f;
    this->catch_goal = 12.5f;
    this->pole_health = 100.0f;

    this->fish_bite = false;
    this->fish_struggle = false;;

    this->start_time = true;
    this->index = 0;
    this->change_direction = false;
    this->reel_index = 0;

    this->reelCheck = 0;
    this->reelSpeed = 0.03f;

    fishingLines.resize(4);
    fishingRod.resize(3);
    fishes.resize(4);

    // Rod Initialize
    {
        std::string pole(ManagerEnvironmentConfiguration::getLMM() + "models/pole.fbx");
        std::string bait(ManagerEnvironmentConfiguration::getLMM() + "models/bait.fbx");
        std::string reel(ManagerEnvironmentConfiguration::getLMM() + "models/reel.fbx");
        std::string line(ManagerEnvironmentConfiguration::getLMM() + "models/line.fbx");
        std::string line2(ManagerEnvironmentConfiguration::getLMM() + "models/line2.fbx");

        std::string skin(ManagerEnvironmentConfiguration::getLMM() + "models/uv_map.png");

        {
            // 120, 125, 6
            //pole
            WO* wo = WO::New(pole, Vector(0.07, 0.07, 0.07));
            wo->upon_async_model_loaded([wo, skin, this]()
                {
                    ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
                    spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
                    //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
                    wo->getModel()->getSkins().push_back(std::move(spidey));
                    wo->getModel()->useNextSkin();
                });
            wo->setPosition(124, 128, 6);
            wo->rotateAboutRelZ(144 * DEGtoRAD);
            wo->rotateAboutRelX(25 * DEGtoRAD);
            ManagerGLView::getGLView()->getWorldContainer()->push_back(wo);
            fishingRod[0] = wo;
        }

        {
            // 120, 125, 6
            //Reel
            WO* wo = WO::New(reel, Vector(0.07, 0.07, 0.07));
            wo->upon_async_model_loaded([wo, skin, this]()
                {
                    ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
                    spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
                    //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
                    wo->getModel()->getSkins().push_back(std::move(spidey));
                    wo->getModel()->useNextSkin();
                });
            wo->setPosition(122.15, 125.77, 4.635);
            wo->rotateAboutRelZ(144 * DEGtoRAD);
            wo->rotateAboutRelX(25 * DEGtoRAD);
            ManagerGLView::getGLView()->getWorldContainer()->push_back(wo);
            fishingRod[1] = wo;
        }

        {
            // 120, 125, 6
            //line
            for (int i = 0; i < fishingLines.size(); i++)
            {
                WO* wo = WO::New(line2, Vector(0.07, 0.07, 0.07));
                wo->upon_async_model_loaded([wo, skin, this]()
                    {
                        rel_x = wo->getModel()->getBoundingBox().getlxlylz().z;
                        ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
                        spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
                        //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
                        wo->getModel()->getSkins().push_back(std::move(spidey));
                        wo->getModel()->useNextSkin();
                    });
                wo->setPosition(127.78, 133.17, 6.57);
                wo->rotateAboutRelZ(144 * DEGtoRAD);
                //wo->rotateAboutRelX(55 *DEGtoRAD);
                ManagerGLView::getGLView()->getWorldContainer()->push_back(wo);
                fishingLines[i] = wo;
            }
        }

        {
            // 120, 125, 6
            //bait
            WO* wo = WO::New(bait, Vector(0.07, 0.07, 0.07));
            wo->upon_async_model_loaded([wo, skin, this]()
                {
                    ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
                    spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
                    //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
                    wo->getModel()->getSkins().push_back(std::move(spidey));
                    wo->getModel()->useNextSkin();
                });
            wo->setPosition(127.795, 133.145, 4.655);
            wo->rotateAboutRelZ(144 * DEGtoRAD);
            wo->rotateAboutRelZ(25 * DEGtoRAD);
            ManagerGLView::getGLView()->getWorldContainer()->push_back(wo);
            fishingRod[2] = wo;
        }
    }

    // Fish initialize;
    {
        std::string blue_fish_path(ManagerEnvironmentConfiguration::getLMM() + "models/Blue_Fish/blue_fish.obj");
        std::string fish_path(ManagerEnvironmentConfiguration::getLMM() + "models/common_fish/fish.obj");
        //std::string bigfish(ManagerEnvironmentConfiguration::getLMM() + "models/bigfish/Fish.fbx");
        std::string long_fin_path(ManagerEnvironmentConfiguration::getLMM() + "models/long_fin/long_fin.obj");
        std::string redfish_path(ManagerEnvironmentConfiguration::getLMM() + "models/redfish/fish.dae");

        std::string fish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/common_fish/fish_texture.png");
        std::string blue_fish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/Blue_Fish/blue_fish_skin.jpg");
        std::string long_fin_skin(ManagerEnvironmentConfiguration::getLMM() + "models/long_fin/long_fin_skin.jpg");
        std::string redfish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/redfish/fish.png");
        //std::string bigfish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/bigfish/FishTex.jpg");
        Vector baitPosition(127.795, 133.145, 4.655);

        Fish* blue_fish = Fish::New(blue_fish_path, blue_fish_skin);
        //blue_fish->setPosition(baitPosition.x - 0.3, baitPosition.y, baitPosition.z - 3.1);
        blue_fish->rotateAboutRelX(180 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(blue_fish);
        fishes[0] = blue_fish;

        Fish* fish = Fish::New(fish_path, fish_skin);
        //fish->setPosition(baitPosition.x - 0.3, baitPosition.y, baitPosition.z - 3.1);
        fish->rotateAboutRelX(-90 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(fish);
        fishes[1] = fish;

        Fish* long_fin = Fish::New(long_fin_path, long_fin_skin);
        //long_fin->setPosition(baitPosition.x, baitPosition.y, baitPosition.z - 3.1);
        long_fin->rotateAboutRelY(90 * DEGtoRAD);
        long_fin->rotateAboutRelX(125 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(long_fin);
        fishes[2] = long_fin;

        Fish* red_fish = Fish::New(redfish_path, redfish_skin);
        //red_fish->setPosition(baitPosition.x - 0.2, baitPosition.y - 0.4, baitPosition.z - 3.1);
        red_fish->rotateAboutRelX(-90 * DEGtoRAD);
        red_fish->rotateAboutRelZ(-90 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(red_fish);
        fishes[3] = red_fish;
        


    }
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
    if (begin)
    {
        reelOut();

        gui->catchGoal = catch_goal;
    }

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

            catch_score += 0.1f;
            std::cout << "CATCH SCORE: " << catch_score << std::endl;

        }
        else if(fish_struggle && this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
        {
            pole_health -= 0.5f;

            std::cout << "HEALTH SCORE: " << pole_health << std::endl;
            if(pole_health >= 0.0f)
                gui->setHealth(float(pole_health / 100));
        }

        if (catch_score == 1000)
        {
            fish_bite = false;
            pole_health = 100.0f;
            start_time = true;
        }

        end_timer = std::chrono::high_resolution_clock::now();

    }

    if (this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton() && catch_score < catch_goal)
    {
        fishingRod[1]->getModel()->rotateAboutRelX(3 * DEGtoRAD);
        if (catch_score >= 6.5)
        {
            reelIn();
            reelSpeed = 0.01;
        }
        catch_score += 0.02f;
        gui->setCatchProgress(catch_score);
    }

    if (this->mouseHandler->isMouseDownRightButton())
    {
        pole_health -= 0.5f;

        //std::cout << "HEALTH SCORE: " << pole_health << std::endl;
        gui->setHealth(float(pole_health / 100));
    }

    if (catch_score >= catch_goal)
    {
        reelIn();
    }

}

void CameraFishing::reelIn()
{
    if (reelCheck >= 3.5)
    {
        fishingLines[3]->moveRelative(Vector(0, 0, reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, reelSpeed));
    }
    else if (reelCheck >= 1.5)
    {
        fishingLines[2]->moveRelative(Vector(0, 0, reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, reelSpeed));
    }
    else if (reelCheck >= 0)
    {
        fishingLines[1]->moveRelative(Vector(0, 0, reelSpeed));
        fishingLines[2]->moveRelative(Vector(0, 0, reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, reelSpeed));
    }

    reelCheck -= reelSpeed;
    std::cout << reelCheck << std::endl;
}

void CameraFishing::reelOut()
{
    if (reelCheck <= 1.5)
    {
        fishingLines[1]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingLines[2]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, -reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingRod[1]->getModel()->rotateAboutRelX(-3 * DEGtoRAD);
    }
    else if (reelCheck <= 3.5)
    {
        fishingLines[2]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, -reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingRod[1]->getModel()->rotateAboutRelX(-3 * DEGtoRAD);
    }
    else if (reelCheck <= 5.5)
    {
        fishingLines[3]->moveRelative(Vector(0, 0, -reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingRod[1]->getModel()->rotateAboutRelX(-3 * DEGtoRAD);
    }
    else
    {
        begin = false;
    }

    reelCheck += reelSpeed;
    std::cout << reelCheck << std::endl;
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

    //if(!fish_struggle) this->changeLookAtViaMouse(rel_x * 0.5, rel_y * 0.5);
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