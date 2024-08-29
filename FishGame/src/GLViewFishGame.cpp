#include "GLViewFishGame.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"
#include "AftrUtilities.h"
#include "WOOcean.h"
#include "Wave.h"
#include "WOWater.h"

#include "WOGUI.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "WOCameraSink.h"

using namespace Aftr;
using namespace physx;

GLViewFishGame* GLViewFishGame::New( const std::vector< std::string >& args )
{
   GLViewFishGame* glv = new GLViewFishGame( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}




GLViewFishGame::GLViewFishGame( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewFishGame::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewFishGame::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewFishGame::onCreate()
{
   //GLViewFishGame::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }

   loadingCam = new CameraShop(this, this->cam->getMouseHandler());
   Vector camLookDir = this->cam->getLookDirection();
   Vector camNormalDir = this->cam->getNormalDirection();
   Vector camPos = this->cam->getPosition();

   this->cam = loadingCam;

   this->cam->setPosition(camPos);
   this->cam->setCameraNormalDirection(camNormalDir);
   this->cam->setCameraLookDirection(camLookDir);
   this->cam->startCameraBehavior();

   mainGui->showShopText = true;
   mainGui->indicator = "Loading Terrain...";

}


GLViewFishGame::~GLViewFishGame()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewFishGame::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.

   if (wo1->isDone == true)
   {
       firstPerson->actor->controller->setPosition(PxExtendedVec3(1414, 2054, -51));
       wo1->isDone = false;
       mainGui->showShopText = false;
       mainGui->indicator = "";
       this->cam = firstPerson;

       std::string bgm(ManagerEnvironmentConfiguration::getLMM() + "sounds/Gloscien.mp3");
       irrklang::ISoundSource* enoMusic = soundEngine->addSoundSourceFromFile(bgm.c_str());
       enoMusic->setDefaultVolume(0.09f);
       
       fishtime->bgmReference = soundEngine->play2D(enoMusic, true, false, true);

   }

   if (this->cam == fishtime)
   {
       fishtime->spawnRod();
       firstPerson->despawnRod();

       if (fishtime->failGame == true)
       {
           fishtime->failGame = false;
           
           fishtime->resetGame();
           Vector camLookDir = this->cam->getLookDirection();
           Vector camNormalDir = this->cam->getNormalDirection();
           Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, -60);
           HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

           this->cam = firstPerson;
           this->cam->setMouseHandler(camMouseHandler);

           firstPerson->actor->controller->setPosition(PxExtendedVec3(camPos.x, camPos.y, camPos.z));
           this->cam->setCameraNormalDirection(camNormalDir);
           this->cam->setCameraLookDirection(camLookDir);
           this->cam->startCameraBehavior();
       }
   }
   else if (this->cam == firstPerson)
   {
       fishtime->despawnRod();
       firstPerson->spawnRod();

       if (cam->getPosition().distanceFrom(chairModel->getPosition()) <= 9.1f)
       {
           mainGui->indicator = "Press F to Fish";
           mainGui->showFish = true;
       }
       else
       {
           mainGui->showFish = false;
       }

       if (cam->getPosition().distanceFrom(boxModel->getPosition()) <= 15.1f)
       {
           mainGui->indicator = "Press F to Shop";
           mainGui->showShopText = true;
       }
       else
       {
           mainGui->showShopText = false;
       }

       if (keystates[SDL_SCANCODE_TAB])
       {
           mainGui->showInventory = true;
           SDL_GetRelativeMouseState(&invX, &invY);
           SDL_WarpMouseInWindow(ManagerWindowing::getCurrentWindow(), invX, invY);
       }
       else
       {
           mainGui->showInventory = false;
       }
       
   }
   else if (cam == shop)
   {
       firstPerson->despawnRod();
   }

   //std::cout << cam->getPosition().distanceFrom(chairModel->getPosition()) << std::endl;

   if (firstTest != nullptr && firstTest->controller != nullptr)
   {
       if (collisionFlags != PxControllerCollisionFlag::eCOLLISION_DOWN) collisionFlags = firstTest->controller->move(PxVec3(0, 0, -0.1), 0.01, 1 / 60, yo);
   }

   scene->simulate(1.0 / 60.0);
   {
       physx::PxU32 errorState = 0;
       scene->fetchResults(true);

       {
           physx::PxU32 numActors = 0;
           physx::PxActor** actors = scene->getActiveActors(numActors);
           //std::cout << "NUMBER OF ACTORS: " << scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC)<< std::endl;
           //std::cout << "NUMBER OF ACTIVE ACTORS: " << numActors << std::endl;
           //std::cout << "NUMBER OF LINKS: " << articulation->getNbLinks() << std::endl;
           //make sure you set physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true in your scene!
           //poses that have changed since the last update (scene->setFlag)
           for (physx::PxU32 i = 0; i < numActors; ++i)
           {
               physx::PxActor* actor = actors[i];

               WOPhysX* wo = static_cast<WOPhysX*>(actor->userData);

               wo->updatePoseFromPhysicsEngine();
           }

           //PxArticulationReducedCoordinate* test;
           //scene->getArticulations(&test, 1);
           //PxArticulationLink* links[3];
           //test->getLinks(links, 1, 0);

           //for (physx::PxU32 i = 0; i < test->getNbLinks(); ++i)
           //{
           //    physx::PxArticulationLink* actor = links[0];

           //    WOPxLink* wo = static_cast<WOPxLink*>(actor->userData);

           //    wo->updatePoseFromPhysicsEngine();
           //}
       }
   }
}

void GLViewFishGame::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewFishGame::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewFishGame::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewFishGame::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );

   //fishingRod[3]->getModel()->rotateAboutRelZ(0.5 * DEGtoRAD);
}


void GLViewFishGame::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );

   if (cam == firstPerson)
   {
       if (cam->getPosition().distanceFrom(chairModel->getPosition()) <= 9.1f && key.keysym.sym == SDLK_f)
       {
            mainGui->showFish = false;
            mainGui->showInventory = false;
            Vector camLookDir = this->cam->getLookDirection();
            Vector camNormalDir = this->cam->getNormalDirection();
            Vector camPos = this->cam->getPosition();
            HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

            this->cam = fishtime;
            this->cam->setMouseHandler(camMouseHandler);

            this->cam->setPosition(camPos);
            this->cam->setCameraNormalDirection(camNormalDir);
            this->cam->setCameraLookDirection(camLookDir);
            this->cam->startCameraBehavior();

            cam->rotateToIdentity();
            cam->setCameraLookDirection(Vector(0.6271, 0.692003, -0.263519));
            cam->setPosition(Vector(1707.334229, 1710.420288, -63.118397));

            fishtime->setBeginGame(true);
           
       }
       if (cam->getPosition().distanceFrom(boxModel->getPosition()) <= 15.1f && key.keysym.sym == SDLK_f)
       {
           mainGui->showShopText = false;
           mainGui->showShop = true;
           mainGui->showInventory = false;

           Vector camLookDir = this->cam->getLookDirection();
           Vector camNormalDir = this->cam->getNormalDirection();
           Vector camPos = this->cam->getPosition();

           shop->setReltoFalse();
           this->cam = shop;

           //this->cam->setPosition(camPos);
           this->cam->setCameraNormalDirection(Vector(0, 0, 1.0f));
           Vector look(-0.991407, -0.121337, 0.049104);
           this->cam->setCameraLookDirection(look);
           this->cam->startCameraBehavior();
           std::string shopSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/OPEN_SHOP.ogg");
           soundEngine->play2D(shopSound.c_str());

       }

   }
   else if (cam == fishtime)
   {
       if (fishtime->allowExit == true && fishtime->showVictory == false && key.keysym.sym == SDLK_f)
       {
            fishtime->resetGame();
            Vector camLookDir = this->cam->getLookDirection();
            Vector camNormalDir = this->cam->getNormalDirection();
            Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, -60);
            HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

            this->cam = firstPerson;
            this->cam->setMouseHandler(camMouseHandler);

            //this->cam->setPosition(camPos);
            firstPerson->actor->controller->setPosition(PxExtendedVec3(camPos.x, camPos.y, camPos.z));
            this->cam->setCameraNormalDirection(camNormalDir);
            this->cam->setCameraLookDirection(camLookDir);
            this->cam->startCameraBehavior();
       }
       else if (fishtime->showVictory == true && key.keysym.sym == SDLK_f)
       {
           fishtime->returnVictory = true;
           fishtime->showVictory = false;
       }
   }
   else if (cam == shop)
   {
       if (key.keysym.sym == SDLK_f)
       {
           mainGui->showShop = false;
           mainGui->resetDialog = true;
           mainGui->showDialog = false;
           mainGui->catDialog->mouthOpen = true;
           mainGui->catDialog->moveMouth();

           std::string shopSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/CLOSE_SHOP.ogg");
           soundEngine->play2D(shopSound.c_str());

           Vector camLookDir = this->cam->getLookDirection();
           Vector camNormalDir = this->cam->getNormalDirection();
           Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, this->cam->getPosition().z + 0.1f);
           HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

           this->shop->setReltoTrue();
           this->cam = firstPerson;
           this->cam->setMouseHandler(camMouseHandler);


           //this->cam->setPosition(camPos);
           firstPerson->actor->controller->setPosition(PxExtendedVec3(camPos.x + 7, camPos.y + 0.5, camPos.z));

           this->cam->setCameraNormalDirection(camNormalDir);
           this->cam->setCameraLookDirection(camLookDir);
           this->cam->startCameraBehavior();
           //firstPerson->setReltoTrue();
       }
   }
   if (key.keysym.sym == SDLK_f)
   {
       Vector camLookDir = this->cam->getLookDirection();
       Vector camNormalDir = this->cam->getNormalDirection();
       Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, this->cam->getPosition().z + 0.1f);
       HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

       this->cam = firstPerson;
       this->cam->setMouseHandler(camMouseHandler);

       this->cam->setPosition(camPos);
       firstPerson->actor->controller->setPosition(PxExtendedVec3(camPos.x + 7, camPos.y + 0.5, camPos.z));

       this->cam->setCameraNormalDirection(camNormalDir);
       this->cam->setCameraLookDirection(camLookDir);
       this->cam->startCameraBehavior();
   }


}


void GLViewFishGame::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewFishGame::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 3500.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   //this->cam->setPosition( 1414, 2054, -59 );
   this->cam->setPosition(0, 0, 5);
   this->cam->rotateAboutRelZ(-180 * DEGtoRAD);

   firstPerson = new CameraFirstPerson(this, this->cam->getMouseHandler());

   fishtime = new CameraFishing(this, this->cam->getMouseHandler());

   shop = new CameraShop(this, this->cam->getMouseHandler());
   shop->setPosition(1301.030396, 1839.195679, -51.920565);
   fishtime->player = firstPerson;
   //PxControllerManager* controllerManager = PxCreateControllerManager(*scene);
   {
       this->foundation = PxCreateFoundation(PX_PHYSICS_VERSION, a, e);
       this->physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale());


       physx::PxSceneDesc sc(physics->getTolerancesScale());
       sc.filterShader = physx::PxDefaultSimulationFilterShader;
       sc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

       this->scene = physics->createScene(sc);

       this->scene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
   }

   //scene->setGravity(physx::PxVec3(0, 0, -9.81f));
   //articulation = physics->createArticulationReducedCoordinate();

   //PxMaterial* gMaterial = physics->createMaterial(0.9f, 0.5f, 0.6f);
   //PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 0, 1, 0), *gMaterial);//good for the grass
   ////groundPlane->setGlobalPose(PxTransform(PxVec3(0, 0, -1000)));
   //scene->addActor(*groundPlane);

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );

   std::string pole(ManagerEnvironmentConfiguration::getLMM() + "models/poleG.obj");
   std::string skin(ManagerEnvironmentConfiguration::getLMM() + "models/uv_map.png");

   std::string hole(ManagerEnvironmentConfiguration::getLMM() + "models/hole2/hole.obj");
   std::string hole_skin(ManagerEnvironmentConfiguration::getLMM() + "models/hole2/TEXS.png");

   std::string chair(ManagerEnvironmentConfiguration::getLMM() + "models/chair/chair.fbx");
   std::string chair_skin(ManagerEnvironmentConfiguration::getLMM() + "models/chair/skin.png");


   controllerManager = PxCreateControllerManager(*scene);
    
   firstTest = WOPxController::New(shinyRedPlasticCube, physics, scene, controllerManager, Vector(1414, 2054, 0));
   worldLst->push_back(firstTest);
   firstTest->isVisible = false;
   firstPerson->actor = firstTest;

   //hello = WO::New(shinyRedPlasticCube, Vector(1, 1, 1));
   //hello->setPosition(0, 0, 5);
   //worldLst->push_back(hello);

   {
       //pole
       WO* wo = WO::New(pole, Vector(1, 1, 1));
       wo->upon_async_model_loaded([wo, skin, this]()
           {
               ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
               spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
               //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
               wo->getModel()->getSkins().push_back(std::move(spidey));
               wo->getModel()->useNextSkin();
               firstPerson->setRod(wo);
           });
       wo->setPosition(cam->getPosition());
       //wo->rotateAboutRelZ(45 * DEGtoRAD);
       worldLst->push_back(wo);
   }

   {
       // hole
       Vector displacement(1707.334229, 1710.420288, -63.118397);
       Vector old(120, 125, 6);

       Vector baitPosition(Vector(127.795, 133.145, 0.675) - old + displacement);
       WO* wo = WO::New(hole, Vector(2, 2, 2));
       wo->upon_async_model_loaded([wo, hole_skin, this]()
           {
               ModelMeshSkin spidey(ManagerTex::loadTexAsync(hole_skin).value());
               spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
               //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
               wo->getModel()->getSkins().push_back(std::move(spidey));
               wo->getModel()->useNextSkin();
           });
       wo->setPosition(baitPosition);
       //wo->rotateAboutRelZ(45 * DEGtoRAD);
       worldLst->push_back(wo);
   }

   {
       // chair
       Vector displacement(1707.334229, 1710.420288, -65.558397);
       Vector old(120, 125, 6);


       Vector baitPosition(Vector(127.795, 133.145, 0.675) - old + displacement);
       chairModel = WOPxStatic::New(chair, displacement, Vector(0.07, 0.07, 0.07), MESH_SHADING_TYPE::mstAUTO, physics, scene);
       //WO* wo = WO::New(chair, Vector(0.07, 0.07, 0.07));
       chairModel->upon_async_model_loaded([chair_skin, this]()
           {
               ModelMeshSkin spidey(ManagerTex::loadTexAsync(chair_skin).value());
               spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
               //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
               chairModel->getModel()->getSkins().push_back(std::move(spidey));
               chairModel->getModel()->useNextSkin();
           });
       chairModel->rotateAboutRelZ(137 * DEGtoRAD);
       chairModel->moveRelative(chairModel->getModel()->getRelYDir() * -0.2);
       chairModel->setPosition(displacement);

       worldLst->push_back(chairModel);
   }
    
   {
       std::string stool(ManagerEnvironmentConfiguration::getLMM() + "models/cardboard/Box.fbx");
       std::string skin(ManagerEnvironmentConfiguration::getLMM() + "models/cardboard/skin.jpg");

       std::string model(ManagerEnvironmentConfiguration::getLMM() + "models/Cat/cat.obj");
       std::string catSkin(ManagerEnvironmentConfiguration::getLMM() + "models/Cat/skin.jpg");

       {
           boxModel = WOPxStatic::New(stool, Vector(1294.24, 1841.5, -58.0), Vector(0.04, 0.04, 0.04), MESH_SHADING_TYPE::mstAUTO, physics, scene);
           boxModel->upon_async_model_loaded([skin, this]()
               {
                   ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
                   spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
                   //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
                   boxModel->getModel()->getSkins().push_back(std::move(spidey));
                   boxModel->getModel()->useNextSkin();
               });
           boxModel->getModel()->rotateAboutRelZ(54 * DEGtoRAD);
           boxModel->setPosition(Vector(1294.24, 1841.5, -58.0));
           worldLst->push_back(boxModel);
       }

       // CATT
       {
           cat = Cat::New(Vector(0.09, 0.09, 0.09));
           cat->setPosition(1294.24, 1841.5, -52.45);
           cat->getModel()->rotateAboutRelZ(76 * DEGtoRAD);
           cat->getModel()->rotateAboutRelX(-90 * DEGtoRAD);
           cat->getModel()->rotateAboutRelY(-4 * DEGtoRAD);

           cat->initalizeDialogue();

           worldLst->push_back(cat);
           worldLst->push_back(cat->frame);
       }
   }
   {
        std::string terrain_data2(ManagerEnvironmentConfiguration::getLMM() + "models/Snow2/Heightmap3.png"); //Mountain_Range.png alien4.png Ridge_NEW.png
        std::string world_skin(ManagerEnvironmentConfiguration::getLMM() + "models/Snow2/Normals.png");

        float top = 34.2072593790098f * 4.5;
        float bottom = 33.9980272592999f * 4.5;

        float left = 118.65234375f * 4.5;
        float right = 118.443603515625 * 4.5;

        float vert = top - bottom;
        float horz = right - left;

        VectorD offset(((top + bottom) / 2), (left + right) / 2, 0);

        VectorD scale = VectorD(0.05f, 0.05f, 0.05f);
        VectorD upperLeft(top, left, 0);
        VectorD lowerRight(bottom, right, 0);

        std::string nothing = "";

        wo1 = Terrain::New(terrain_data2, world_skin, physics, scene, upperLeft, lowerRight, 0, scale, offset);
        this->worldLst->push_back((WOGridECEFElevation*)wo1);
    }

   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getLMM() + "images/Areskutan.jpg");
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

   {
      //Create a light
      float ga = 0.1f; //Global Ambient Light level for this module
      ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
      WOLight* light = WOLight::New();
      light->isDirectionalLight( true );
      light->setPosition( Vector( 0, 0, 100 ) );
      //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
      //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
      light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
      light->setLabel( "Light" );
      worldLst->push_back( light );
   }

   {
      //Create the SkyBox
      WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->setLabel( "Sky Box" );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      worldLst->push_back( wo );
   }

   //{ 
   //   //Create the infinite grass plane (the floor)
   //   WO* wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //   wo->setPosition( Vector( 0, 0, 0 ) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->upon_async_model_loaded( [wo]()
   //      {
   //         ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
   //         grassSkin.getMultiTextureSet().at( 0 ).setTexRepeats( 5.0f );
   //         grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
   //         grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
   //         grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
   //         grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
   //      } );
   //   wo->setLabel( "Grass" );
   //   wo->isLockedWRTparent()
   //   wo->getChildren()
   //   worldLst->push_back( wo );
   //}

   //{
   //   //Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
   //   WO* wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
   //   ((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
   //   wo->setPosition( Vector(0,0,0) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   //Create the infinite grass plane that uses NVIDIAPhysX(the floor)
   //   WO* wo = WONVStaticPlane::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //   wo->setPosition( Vector( 0, 0, 0 ) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 ).getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   //Create the infinite grass plane (the floor)
   //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //   wo->setPosition( Vector( 0, 0, 50.0f ) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
   //   wo->setPosition( Vector( 0, 0.5f, 75.0f ) );
   //   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
   //   wo->setLabel( "Grass" );
   //   worldLst->push_back( wo );
   //}

   //{
   //   WO* wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue( "sharedmultimediapath" ) + "/models/sphereRp5.wrl", Vector( 1.0f, 1.0f, 1.0f ), mstSMOOTH );
   //   wo->setPosition( 0, 0, 100.0f );
   //   wo->setLabel( "Sphere" );
   //   this->worldLst->push_back( wo );
   //}

   //{
   //   WO* wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
   //   ((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
   //   ((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
   //   wo->setPosition( Vector( 20, 20, 20 ) );
   //   wo->setLabel( "Paladin" );
   //   worldLst->push_back( wo );
   //   actorLst->push_back( wo );
   //   netLst->push_back( wo );
   //   this->setActor( wo );
   //}
   //
   //{
   //   WO* wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
   //   wo->setPosition( Vector( 20, 10, 20 ) );
   //   wo->isVisible = false; //hide the WOHuman's bounding box
   //   ((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
   //   wo->setLabel( "Human Cyborg" );
   //   worldLst->push_back( wo );
   //   actorLst->push_back( wo ); //Push the WOHuman as an actor
   //   netLst->push_back( wo );
   //   this->setActor( wo ); //Start module where human is the actor
   //}

   //{
   //   //Create and insert the WOWheeledVehicle
   //   std::vector< std::string > wheels;
   //   std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
   //   wheels.push_back( wheelStr );
   //   wheels.push_back( wheelStr );
   //   wheels.push_back( wheelStr );
   //   wheels.push_back( wheelStr );
   //   WO* wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
   //   wo->setPosition( Vector( 5, -15, 20 ) );
   //   wo->setLabel( "Car 1970s Beater" );
   //   ((WOODE*)wo)->mass = 200;
   //   worldLst->push_back( wo );
   //   actorLst->push_back( wo );
   //   this->setActor( wo );
   //   netLst->push_back( wo );
   //}


   //Make a Dear Im Gui instance via the WOImGui in the engine... This calls
   //the default Dear ImGui demo that shows all the features... To create your own,

   mainGui = GuiText::New(nullptr);
   //WOImGui* gui = WOImGui::New(nullptr);
   //gui->setLabel( "My Gui" );
   //gui->subscribe_drawImGuiWidget(
   //   [this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
   //   {
   //        //WOImGui::draw_AftrImGui_Demo(gui);
   //        ////yo->drawImGui_for_this_frame();
   //        //if (ImGui::SliderFloat("Rel X Axis", &object_xyz[0], -50, 50)) {
   //        //    fishBait->moveRelative(fishBait->getModel()->getRelXDir() * (object_xyz[0] - object_xyz_prev[0]));
   //        //    object_xyz_prev[0] = object_xyz[0];
   //        //}

   //        //if (ImGui::SliderFloat("Rel Y Axis", &object_xyz[1], -180, 180)) {
   //        //    fishBait->getModel()->moveRelative(fishBait->getModel()->getRelYDir() * (object_xyz[1] - object_xyz_prev[1]));
   //        //    object_xyz_prev[1] = object_xyz[1];
   //        //}

   //        //if (ImGui::SliderFloat("Rel Z Axis", &object_xyz[2], -50, 50)) {
   //        //    fishBait->moveRelative(fishBait->getModel()->getRelZDir() * (object_xyz[2] - object_xyz_prev[2]));
   //        //    object_xyz_prev[2] = object_xyz[2];
   //        //}
   //   
   //   } );
   //this->worldLst->push_back( gui );
   this->worldLst->push_back(mainGui);
   fishtime->gui = mainGui;
   firstPerson->fishData = &fishtime->fishes;
   //yo->fishData = fishtime;
   mainGui->player = firstPerson;
   mainGui->catDialog = cat;
   createFishGameWayPoints();

   {
       std::string reelSound(ManagerEnvironmentConfiguration::getLMM() + "sounds/REEL_IN2.ogg");
       std::string reelSound2(ManagerEnvironmentConfiguration::getLMM() + "sounds/REEL_OUT2.ogg");
       std::string reelSound3(ManagerEnvironmentConfiguration::getLMM() + "sounds/FISH_STRUGGLE2.ogg");

       std::string helloTalk(ManagerEnvironmentConfiguration::getLMM() + "sounds/TALK.ogg");

       std::string winSource(ManagerEnvironmentConfiguration::getLMM() + "sounds/WIN.ogg");
       std::string victorySource(ManagerEnvironmentConfiguration::getLMM() + "sounds/Confetti.mp3");

       irrklang::ISoundSource* reelInSound = soundEngine->addSoundSourceFromFile(reelSound.c_str());
       irrklang::ISoundSource* reelOutSound = soundEngine->addSoundSourceFromFile(reelSound2.c_str());
       irrklang::ISoundSource* fishStruggleSound = soundEngine->addSoundSourceFromFile(reelSound3.c_str());

       irrklang::ISoundSource* talkSound = soundEngine->addSoundSourceFromFile(helloTalk.c_str());

       irrklang::ISoundSource* winSound = soundEngine->addSoundSourceFromFile(winSource.c_str());

       irrklang::ISoundSource* victorySong = soundEngine->addSoundSourceFromFile(victorySource.c_str());

       fishtime->playReelIn = soundEngine->play2D(reelInSound, true, true);
       fishtime->playReelOut = soundEngine->play2D(reelOutSound, true, true);
       fishtime->playFishStruggle = soundEngine->play2D(fishStruggleSound, true, true);

       mainGui->playTalk = soundEngine->play2D(talkSound, true, true);
       mainGui->playWinSound = soundEngine->play2D(winSound, true, true);

       fishtime->playReelIn->setVolume(0.11f);
       fishtime->playReelOut->setVolume(0.11f);
       fishtime->playFishStruggle->setVolume(0.12f);
       mainGui->playTalk->setVolume(0.15f);
       mainGui->playWinSound->setVolume(0.13f);
       firstPerson->soundPlayer = soundEngine;
       mainGui->soundPlayer = soundEngine;

       fishtime->playVictory = soundEngine->play2D(victorySong, true, true);
       fishtime->playVictory->setVolume(0.15f);
   }
}


void GLViewFishGame::createFishGameWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}
