#include "CameraFishing.h"

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

bool compare_float(float x, float y, float epsilon = 0.01f) {
    if (fabs(x - y) < epsilon)
        return true; //they are same
    return false; //they are not same
}


int CameraFishing::generateRandomNumber(int min, int max) {
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

void CameraFishing::rotateReel(int amount)
{
    Mat4 savePose = fishingRod[1]->getPose();
    fishingRod[1]->getModel()->rotateToIdentity();
    
    fishingRod[1]->setPose(savePose);

    fishingRod[1]->getModel()->rotateAboutRelX(amount * DEGtoRAD);
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
    this->angleVictory = 0;

    this->begin = false;
    this->catch_score = 0.0f;
    this->catch_goal = 11.4f;
    this->pole_health = 100.0f;
    this->reelOutStatus = false;
    this->allowExit = true;

    this->fish_bite = false;
    this->fish_struggle = false;
    this->showVictory = false;

    this->start_time = false;
    this->index = 0;
    this->change_direction = false;
    this->reel_index = 0;

    this->endGame = false;
    this->startGame = false;
    this->startWait = false;

    this->reelCheck = 0;
    this->reelSpeed = 0.03f;
    this->fishIndex = 0;
    this->failGame = false;

    this->waitTime = 0;

    fishingLines.resize(4);
    fishingRod.resize(3);
    fishes.resize(4);

    Vector displacement(1707.334229, 1710.420288, -63.118397);
    Vector old(120, 125, 6);

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
            wo->setPosition(Vector(124, 128, 6) - old + displacement);
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
            wo->setPosition(Vector(122.15, 125.77, 4.635) - old + displacement);
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
                wo->setPosition(Vector(127.78, 133.17, 6.57) - old + displacement);
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
            wo->setPosition(Vector(127.795, 133.145, 4.655) - old + displacement);
            wo->rotateAboutRelZ(144 * DEGtoRAD);
            wo->rotateAboutRelZ(25 * DEGtoRAD);
            ManagerGLView::getGLView()->getWorldContainer()->push_back(wo);
            fishingRod[2] = wo;
            baitPointer = wo;
        }
    }

    this->returnVictory = false;

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
        Vector baitPosition(Vector(127.795, 133.145, 4.655) - old + displacement);

        Fish* blue_fish = Fish::New(blue_fish_path, blue_fish_skin);
        blue_fish->setPosition(baitPosition.x - 0.3, baitPosition.y, baitPosition.z - 3.1);
        blue_fish->rotateAboutRelX(180 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(blue_fish);
        fishes[0] = blue_fish;
        blue_fish->struggleRange.first = 1;
        blue_fish->struggleRange.second = 3;
        blue_fish->name = "Blue Fish";
        blue_fish->price = 16;
        blue_fish->difficulty = 18.8f;

        Fish* fish = Fish::New(fish_path, fish_skin);
        fish->setPosition(baitPosition.x - 0.3, baitPosition.y, baitPosition.z - 3.1);
        fish->rotateAboutRelX(-90 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(fish);
        fishes[1] = fish;
        fish->struggleRange.first = 3;
        fish->struggleRange.second = 4;
        fish->name = "Common Fish";
        fish->price = 6;
        fish->difficulty = 12.3f;

        Fish* long_fin = Fish::New(long_fin_path, long_fin_skin);
        long_fin->setPosition(baitPosition.x, baitPosition.y, baitPosition.z - 3.1);
        long_fin->rotateAboutRelY(90 * DEGtoRAD);
        long_fin->rotateAboutRelX(125 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(long_fin);
        fishes[2] = long_fin;
        long_fin->struggleRange.first = 3;
        long_fin->struggleRange.second = 5;
        long_fin->name = "Long Fin";
        long_fin->price = 3;
        long_fin->difficulty = 6.33f;

        Fish* red_fish = Fish::New(redfish_path, redfish_skin);
        red_fish->setPosition(baitPosition.x - 0.2, baitPosition.y - 0.4, baitPosition.z - 3.1);
        red_fish->rotateAboutRelX(-90 * DEGtoRAD);
        red_fish->rotateAboutRelZ(-90 * DEGtoRAD);
        ManagerGLView::getGLView()->getWorldContainer()->push_back(red_fish);
        fishes[3] = red_fish;
        red_fish->struggleRange.first = 2;
        red_fish->struggleRange.second = 4;
        red_fish->name = "Red Fish";
        red_fish->price = 12;
        red_fish->difficulty = 15.6;

        for (int i = 0; i < fishes.size(); i++)
        {
            fishes[i]->isVisible = false;
        }

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
            this->changeLookAtViaMouse(-5, 0);
        else
            change_direction = false;
    }

    //this->changeLookAtViaMouse(5, 0);

}

bool CameraFishing::normalizeCamera()
{
    if (compare_float(this->getLookDirection().x, 0.646266))
    {
        return true;
    }
    else if (this->getLookDirection().x > 0.646266)
    {
        this->changeLookAtViaMouse(-5, 0);
    }
    else
        this->changeLookAtViaMouse(5, 0);

    return false;
}

void CameraFishing::resetGame()
{
    startGame = false;
    begin = false;
    reelOutStatus = false;
    startWait = false;
    fish_bite = false;
    reelCheck = 0.0f;
    endGame = false;
    fish_struggle = false;
    reelSpeed = 0.04;
    allowExit = true;

    gui->showHealth = false;
    gui->showProgress = false;

    Vector displacement(1707.334229, 1710.420288, -63.118397);
    Vector old(120, 125, 6);

    Vector baitPosition(Vector(127.795, 133.145, 4.655) - old + displacement);


    for (int i = 0; i < fishingLines.size(); i++)
    {
        fishingLines[i]->setPosition(Vector(127.78, 133.17, 6.57) - old + displacement);
    }

    fishingRod[2]->setPosition(baitPosition);

    if (fishIndex == 0)
    {
        fishes[fishIndex]->setPosition(baitPosition.x - 0.3, baitPosition.y, baitPosition.z - 3.1);
        fishes[fishIndex]->isVisible = false;
    }
    else if (fishIndex == 1)
    {
        fishes[fishIndex]->setPosition(baitPosition.x - 0.3, baitPosition.y, baitPosition.z - 3.1);
        fishes[fishIndex]->isVisible = false;
    }
    else if (fishIndex == 2)
    {
        fishes[fishIndex]->setPosition(baitPosition.x, baitPosition.y, baitPosition.z - 3.1);
        fishes[fishIndex]->isVisible = false;
    }
    else
    {
        fishes[fishIndex]->setPosition(baitPosition.x - 0.2, baitPosition.y - 0.4, baitPosition.z - 3.1);
        fishes[fishIndex]->isVisible = false;

    }
    pole_health = 100.0f;
    catch_score = 0;

    playFishStruggle->setIsPaused(true);
    playReelIn->setIsPaused(true);
    playReelOut->setIsPaused(true);
}

void CameraFishing::victoryScreen()
{
    if (angleVictory != 90)
    {
        playVictory->setIsPaused(false);

        this->rotateAboutRelY(-3 * DEGtoRAD);
        angleVictory += 3;
        fishes[fishIndex]->isVisible = false;
        fishes[fishIndex]->rotateToIdentity();

        if (bgmReference->getVolume() >= 0.0f)
        {
            bgmReference->setVolume(bgmReference->getVolume() - 0.003f);
        }

        if (playVictory->getVolume() < 0.15f)
        {
            bgmReference->setVolume(playVictory->getVolume() + 0.005f);
        }
    }
    else
    {
        bgmReference->setIsPaused(true);
        gui->showVictoryText = true;
        gui->victoryText = "You Caught a " + fishes[fishIndex]->name + "!";
        fishes[fishIndex]->isVisible = true;
        fishes[fishIndex]->setPosition(this->getCameraLookAtPoint() + (this->getLookDirection() * 8));
        //fishes[fishIndex]->rotateAboutRelZ(1 * DEGtoRAD);
        //fishes[fishIndex]->rotateAboutRelX(1 * DEGtoRAD);
        fishes[fishIndex]->rotateAboutRelY(1 * DEGtoRAD);

    }
}

void CameraFishing::returnFromVictory()
{
    if (angleVictory != 0)
    {
        bgmReference->setIsPaused(false);
        angleVictory -= 3;
        this->rotateAboutRelY(3 * DEGtoRAD);
        if (playVictory->getVolume() > 0.0f)
        {
            playVictory->setVolume(playVictory->getVolume() - 0.005f);
        }

        if (bgmReference->getVolume() <= 0.09f)
        {
            bgmReference->setVolume(bgmReference->getVolume() + 0.003f);
        }
    }
    else
    {
        fishes[fishIndex]->setPose(saveFishPose);

        playVictory->setIsPaused(true);
        playVictory->setPlayPosition(0);
        playVictory->setVolume(0.15f);
        bgmReference->setVolume(0.09f);

        fishes[fishIndex]->isVisible = false;
        returnVictory = false;
        begin = true;
        allowExit = true;
        gui->showVictoryText = false;
        gui->victoryText = "";
        gui->resetDialog = true;

        resetGame();

        this->begin = true;
    }
}

void CameraFishing::update()
{
    // begining sequence: generate data -> reelOut Animation -> wait until bite;
    if (begin)
    {
        //this->setCameraLookDirection(Vector(0.646266f, 0.713154f, this->getLookDirection().z));
        fishes[fishIndex]->isVisible = false;
        waitTime = generateRandomNumber(3, 8);
        fishIndex = generateRandomNumber(0, 3);

        reelOutStatus = true;

        gui->catchGoal = fishes[fishIndex]->difficulty;
        begin = false;
    }
    else if (reelOutStatus)
    {
        reelOut();
        rotateReel(-3);
    }
    else if (startWait && waitTime == std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer).count())
    {
        fish_bite = true;
        fishes[fishIndex]->isVisible = true;
        startWait = false;
    }


    // Main game sequence: bite -> startGame -> Endgame or Line breaks
    if (fish_bite)
    {
        gui->showHealth = true;
        pole_health -= 0.1f;
        playFishStruggle->setIsPaused(false);
        //gui->setHealth(float(pole_health / 100));
        rotateReel(-10);
        if (this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
        {
            this->catch_goal = fishes[fishIndex]->difficulty;
            startGame = true;
            fish_bite = false;
            start_time = true;
            fishStruggleTime = generateRandomNumber(fishes[fishIndex]->struggleRange.first, fishes[fishIndex]->struggleRange.second);
            gui->showProgress = true;
            allowExit = false;

            playFishStruggle->setIsPaused(true);
        }
    }
    else if (startGame && !failGame)
    {
        if (fish_struggle)
        {
            shakeCamera();
            playFishStruggle->setIsPaused(false);
            rotateReel(-12);
        }

        if (start_time)
        {
            start_timer = std::chrono::high_resolution_clock::now();
            start_time = false;
        }
        else if (!fish_struggle && std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer).count() == fishStruggleTime)
        {
            fish_struggle = true;
            start_time = true;
        }

        if (fish_struggle && std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer).count() == 2)
        {
            fish_struggle = false;
            start_time = true;
            fishStruggleTime = generateRandomNumber(fishes[fishIndex]->struggleRange.first, fishes[fishIndex]->struggleRange.second);
            playFishStruggle->setIsPaused(true);
        }

        if (!fish_struggle && this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
        {
            rotateReel(3);
            if (catch_score >= catch_goal * 0.65)
            {
                reelIn();
                reelSpeed = 0.01;
            }
            catch_score += 0.02f;
            if (playReelIn->getIsPaused()) playReelIn->setIsPaused(false);
            //gui->setCatchProgress(catch_score);

        }
        else if (fish_struggle && this->mouseHandler != NULL && this->mouseHandler->isMouseDownLeftButton())
        {
            pole_health -= 0.5f;

            //std::cout << "HEALTH SCORE: " << pole_health << std::endl;
            //if (pole_health >= 0.0f)
            //    gui->setHealth(float(pole_health / 100));
        }

        if (playReelIn->getIsPaused() == false && this->mouseHandler->isMouseDownLeftButton() == false)
        {
            playReelIn->setIsPaused(true);
        }

        if (catch_score >= catch_goal)
        {
            fish_struggle = false;
            fishStruggleTime = 1000;
            pole_health = 100.0f;
            start_time = false;
            startGame = false;
            endGame = true;
            catch_score = 0;

            gui->showHealth = false;
            gui->showProgress = false;
            playReelIn->setIsPaused(true);

            player->inventory[fishes[fishIndex]->getName()]++;
        }

        if (pole_health <= 0.0f)
        {

            failGame = true;
            resetGame();

        }
    }
    else if (endGame)
    {
        normalizeCamera();

        reelSpeed = 0.04;
        playReelOut->setIsPaused(false);
        rotateReel(3);
        reelIn();
    }

    if (showVictory)
    {
        victoryScreen();
    }
    else if (returnVictory)
    {
        returnFromVictory();
    }

    gui->setHealth(float(pole_health / 100));
    gui->setCatchProgress(catch_score);
    end_timer = std::chrono::high_resolution_clock::now();

}

void CameraFishing::reelIn()
{
    if (reelCheck >= 3.5)
    {
        fishingLines[3]->moveRelative(Vector(0, 0, reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, reelSpeed));
        fishes[fishIndex]->moveRelative(Vector(0, 0, reelSpeed));
    }
    else if (reelCheck >= 1.5)
    {
        fishingLines[2]->moveRelative(Vector(0, 0, reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, reelSpeed));
        fishes[fishIndex]->moveRelative(Vector(0, 0, reelSpeed));
    }
    else if (reelCheck >= 0)
    {
        fishingLines[1]->moveRelative(Vector(0, 0, reelSpeed));
        fishingLines[2]->moveRelative(Vector(0, 0, reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, reelSpeed));
        fishes[fishIndex]->moveRelative(Vector(0, 0, reelSpeed));
    }
    else
    {
        saveFishPose = fishes[fishIndex]->getPose();
        playReelOut->setIsPaused(true);
        endGame = false;
        sleep_for(milliseconds(650));
        showVictory = true;
    }

    reelCheck -= reelSpeed;
}

void CameraFishing::reelOut()
{
    if (reelCheck <= 1.5)
    {
        playReelOut->setIsPaused(false);
        fishingLines[1]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingLines[2]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, -reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, -reelSpeed));

        rotateReel(-3);

        fishes[fishIndex]->moveRelative(Vector(0, 0, -reelSpeed));
    }
    else if (reelCheck <= 3.5)
    {
        fishingLines[2]->moveRelative(Vector(0, 0, -reelSpeed));
        fishingLines[3]->moveRelative(Vector(0, 0, -reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, -reelSpeed));

        rotateReel(-3);

        fishes[fishIndex]->moveRelative(Vector(0, 0, -reelSpeed));
    }
    else if (reelCheck <= 5.5)
    {
        fishingLines[3]->moveRelative(Vector(0, 0, -reelSpeed));

        fishingRod[2]->moveRelative(Vector(0, 0, -reelSpeed));

        rotateReel(-3);

        fishes[fishIndex]->moveRelative(Vector(0, 0, -reelSpeed));
    }
    else
    {
        reelOutStatus = false;
        startWait = true;
        start_timer = std::chrono::high_resolution_clock::now();

        playReelOut->setIsPaused(true);
    }

    reelCheck += reelSpeed;
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