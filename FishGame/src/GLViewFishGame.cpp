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

   Vector camLookDir = this->cam->getLookDirection();
   Vector camNormalDir = this->cam->getNormalDirection();
   Vector camPos = this->cam->getPosition();

   firstPerson = new CameraFirstPerson(this, this->cam->getMouseHandler());

   this->cam = firstPerson;

   this->cam->setPosition(camPos);
   this->cam->setCameraNormalDirection(camNormalDir);
   this->cam->setCameraLookDirection(camLookDir);
   this->cam->startCameraBehavior();
   this->cam->setLabel("Camera"); 
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

   if (this->cam == fishtime)
   {
       fishtime->spawnRod();
       firstPerson->despawnRod();

       if (fishtime->failGame == true)
       {
           fishtime->failGame = false;
           Vector camLookDir = this->cam->getLookDirection();
           Vector camNormalDir = this->cam->getNormalDirection();
           Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, 10);
           HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

           this->cam = firstPerson;
           this->cam->setMouseHandler(camMouseHandler);

           this->cam->setPosition(camPos);
           this->cam->setCameraNormalDirection(camNormalDir);
           this->cam->setCameraLookDirection(camLookDir);
           this->cam->startCameraBehavior();
       }
   }
   else
   {
       fishtime->despawnRod();
       firstPerson->spawnRod();
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
       if (key.keysym.sym == SDLK_f)
       {
            std::cout << "FISH ACTIVATE" << std::endl;

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
            cam->setPosition(Vector(120, 125, 6));

            fishtime->setBeginGame(true);
       }
   }
   else if (cam == fishtime)
   {
       if (fishtime->allowExit == true && fishtime->showVictory == false && key.keysym.sym == SDLK_f)
       {
            fishtime->resetGame();
            Vector camLookDir = this->cam->getLookDirection();
            Vector camNormalDir = this->cam->getNormalDirection();
            Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, 10);
            HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

            this->cam = firstPerson;
            this->cam->setMouseHandler(camMouseHandler);

            this->cam->setPosition(camPos);
            this->cam->setCameraNormalDirection(camNormalDir);
            this->cam->setCameraLookDirection(camLookDir);
            this->cam->startCameraBehavior();
       }
       else if (fishtime->showVictory == true && key.keysym.sym == SDLK_f)
       {
           fishtime->returnVictory = true;
           fishtime->showVictory = false;
           std::cout << "HELLO??" << std::endl;
       }
   }

   //if (key.keysym.sym == SDLK_0)
   //{
   //}

   //if (key.keysym.sym == SDLK_9)
   //{
   //    pos += 0.5f;
   //    anchor2->setPosition(Vector(37, 0, pos));
   //}

   //if( key.keysym.sym == SDLK_1 )
   //{
   //    //std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");

   //    //Vector camStuff = this->cam->getCameraLookAtPoint() + (this->cam->getLookDirection() * 5);

   //    //int distance = this->cam->getPosition().distanceFrom(vendor->getPosition());

   //    ////WO* wo = WO::New(shinyRedPlasticCube);
   //    ////wo->setPosition(camStuff);
   //    ////worldLst->push_back(wo);
   //    //occulude = blocker->getNearestPointWhereLineIntersectsMe(camStuff, vendor->getPosition(), rayOutput);

   //    //std::cout << "(" << rayOutput.x << ", " << rayOutput.y << ", " << rayOutput.z << ")" << std::endl;
   //    //std::cout << "Distance between Vendor and Cam: " << distance << std::endl;
   //    //if (occulude == AftrGeometricTerm::geoSUCCESS)
   //    //{
   //    //    std::cout << "POINT FOUND" << std::endl;
   //    //}
   //    //else
   //    //{
   //    //    std::cout << "NO POINT FOUND" << std::endl;
   //    //}


   //}
   //if (key.keysym.sym == SDLK_2)
   //{   
   //    std::cout << rel_x << std::endl;
   //    fishingLines[1]->moveRelative(Vector(0, 0, -rel_x * 0.1));
   //    fishingLines[2]->moveRelative(Vector(0, 0, -rel_x * 0.1));
   //    fishingRod[2]->moveRelative(Vector(0, 0, -rel_x * 0.1));

   //    fishingRod[1]->getModel()->rotateAboutRelX(-5 * DEGtoRAD);

   //}
   //if (key.keysym.sym == SDLK_3)
   //{

   //    for (int i = 0; i < fishingLines.size(); i++)
   //    {
   //        fishingLines[i]->getModel()->rotateAboutRelY(2 * DEGtoRAD);
   //    }
   //    for (int i = 0; i < fishingRod.size(); i++)
   //    {
   //        fishingRod[i]->getModel()->rotateAboutRelZ(2 * DEGtoRAD);
   //    }

   //}
   //if (key.keysym.sym == SDLK_SPACE)
   //{

   //}

   //if (key.keysym.sym == SDLK_4)
   //{
   //    std::cout << "FISH ACTIVATE" << std::endl;

   //    Vector camLookDir = this->cam->getLookDirection();
   //    Vector camNormalDir = this->cam->getNormalDirection();
   //    Vector camPos = this->cam->getPosition();
   //    HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

   //    this->cam = fishtime;
   //    this->cam->setMouseHandler(camMouseHandler);

   //    this->cam->setPosition(camPos);
   //    this->cam->setCameraNormalDirection(camNormalDir);
   //    this->cam->setCameraLookDirection(camLookDir);
   //    this->cam->startCameraBehavior();
   //    this->cam->setLabel("FishCamera");

   //    cam->rotateToIdentity();
   //    cam->setCameraLookDirection(Vector(0.6271, 0.692003, -0.263519));
   //    cam->setPosition(Vector(120, 125, 6));
   //}

   //if (key.keysym.sym == SDLK_5)
   //{
   //    Vector camLookDir = this->cam->getLookDirection();
   //    Vector camNormalDir = this->cam->getNormalDirection();
   //    Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, 10);
   //    HandlerMouseState* camMouseHandler = this->cam->getMouseHandler();

   //    this->cam = firstPerson;
   //    this->cam->setMouseHandler(camMouseHandler);

   //    this->cam->setPosition(camPos);
   //    this->cam->setCameraNormalDirection(camNormalDir);
   //    this->cam->setCameraLookDirection(camLookDir);
   //    this->cam->startCameraBehavior();


   //    

   //    //this->setActorChaseType(STANDARDEZNAV);
   //}

   //if (key.keysym.sym == SDLK_6)
   //{
   //    fishtime->setBeginGame(true);
   //}


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

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 5,17, 7 );
   this->cam->rotateAboutRelZ(-180 * DEGtoRAD);

   fishingRod.resize(3);
   fishingLines.resize(3);

   fishtime = new CameraFishing(this, this->cam->getMouseHandler());

   {
       this->foundation = PxCreateFoundation(PX_PHYSICS_VERSION, a, e);
       this->physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale());


       physx::PxSceneDesc sc(physics->getTolerancesScale());
       sc.filterShader = physx::PxDefaultSimulationFilterShader;
       sc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

       this->scene = physics->createScene(sc);

       this->scene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
   }

   //scene->setGravity(physx::PxVec3(0, 0, -2.81f));
   articulation = physics->createArticulationReducedCoordinate();

   WOCameraSink* hello;

   PxMaterial* gMaterial = physics->createMaterial(0.9f, 0.5f, 0.6f);
   PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 0, 1, 0), *gMaterial);//good for the grass
   groundPlane->setGlobalPose(PxTransform(PxVec3(0, 0, -1000)));
   scene->addActor(*groundPlane);

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );

   std::string pole(ManagerEnvironmentConfiguration::getLMM() + "models/pole.fbx");
   std::string bait(ManagerEnvironmentConfiguration::getLMM() + "models/bait.fbx");
   std::string reel (ManagerEnvironmentConfiguration::getLMM() + "models/reel.fbx");
   std::string line(ManagerEnvironmentConfiguration::getLMM() + "models/line.fbx");
   std::string line2(ManagerEnvironmentConfiguration::getLMM() + "models/line2.fbx");

   std::string hand(ManagerEnvironmentConfiguration::getLMM() + "models/hand.obj");

   std::string skin(ManagerEnvironmentConfiguration::getLMM() + "models/uv_map.png");
   std::string beachball(ManagerEnvironmentConfiguration::getSMM() + "/models/beachball.3ds");
   std::string fontArial(ManagerEnvironmentConfiguration::getSMM() + "/fonts/arial.ttf");

   std::string blue_fish(ManagerEnvironmentConfiguration::getLMM() + "models/Blue_Fish/blue_fish.obj");
   std::string fish(ManagerEnvironmentConfiguration::getLMM() + "models/common_fish/fish.obj");
   std::string bigfish(ManagerEnvironmentConfiguration::getLMM() + "models/bigfish/Fish.fbx");
   std::string long_fin(ManagerEnvironmentConfiguration::getLMM() + "models/long_fin/long_fin.obj");
   std::string redfish(ManagerEnvironmentConfiguration::getLMM() + "models/redfish/fish.dae");
   //std::string goldfish(ManagerEnvironmentConfiguration::getLMM() + "models/goldfish/goldfish.fbx");

   std::string fish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/common_fish/fish_texture.png");
   std::string blue_fish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/Blue_Fish/blue_fish_skin.jpg");
   std::string long_fin_skin(ManagerEnvironmentConfiguration::getLMM() + "models/long_fin/long_fin_skin.jpg");
   std::string redfish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/redfish/fish.png");
   std::string bigfish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/bigfish/FishTex.jpg");
   //std::string goldfish_skin(ManagerEnvironmentConfiguration::getLMM() + "models/goldfish/goldfishTex.png");

   //vendor = WO::New(shinyRedPlasticCube, Vector(1, 1, 1));
   //vendor->setPosition(0, 0, 10);
   //worldLst->push_back(vendor);

   //blocker = WO::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5));
   //blocker->setPosition(0, 8, 10);
   ////blocker->isVisible = false;
   //worldLst->push_back(blocker);

   anchor = WOPxStatic::New(shinyRedPlasticCube, Vector(0, 0, 50), Vector(1, 0.5, 0.5), MESH_SHADING_TYPE::mstAUTO, physics, scene);
   worldLst->push_back(anchor);

   {
       //pole
       WO* wo = WO::New(pole, Vector(0.1, 0.1, 0.1));
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

   //{
   //    //fish
   //    WO* wo = WO::New(fish, Vector(1, 1, 1));
   //    wo->upon_async_model_loaded([wo, fish_skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(fish_skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(cam->getPosition());
   //    wo->rotateAboutRelX(-90 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //}

   //{
   //    // blue fish
   //    WO* wo = WO::New(blue_fish, Vector(1, 1, 1));
   //    wo->upon_async_model_loaded([wo, blue_fish_skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(blue_fish_skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 0, 10);
   //    wo->rotateAboutRelX(180 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //}

   //{
   //    // long fin
   //    WO* wo = WO::New(long_fin, Vector(1.1, 1.1, 1.1));
   //    wo->upon_async_model_loaded([wo, long_fin_skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(long_fin_skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 5, 10);
   //    wo->rotateAboutRelY(90 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //}

   //{
   //    // red
   //    WO* wo = WO::New(redfish, Vector(1, 1, 1));
   //    wo->upon_async_model_loaded([wo, redfish_skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(redfish_skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 10, 10);
   //    wo->rotateAboutRelX(-90 * DEGtoRAD);
   //    wo->rotateAboutRelZ(-90 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //}

   //{
   //    // big
   //    WO* wo = WO::New(bigfish, Vector(0.8, 0.8, 0.8));
   //    wo->upon_async_model_loaded([wo, bigfish_skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(bigfish_skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 30, 10);
   //    wo->rotateAboutRelX(-90 * DEGtoRAD);
   //    wo->rotateAboutRelZ(-125 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //}

   //{
   //    // goldFish
   //    WO* wo = WO::New(goldfish, Vector(1, 1, 1));
   //    wo->upon_async_model_loaded([wo, goldfish_skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(goldfish_skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 25, 10);
   //    //wo->rotateAboutRelZ(45 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //}

   //{
   //    // 120, 125, 6
   //    //pole
   //    WO* wo = WO::New(pole, Vector(0.07, 0.07, 0.07));
   //    wo->upon_async_model_loaded([wo, skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(124, 128, 6);
   //    wo->rotateAboutRelZ(144 * DEGtoRAD);
   //    wo->rotateAboutRelX(25 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //    fishingRod[0] = wo;
   //}

   //{
   //    // 120, 125, 6
   //    //Reel
   //    WO* wo = WO::New(reel, Vector(0.07, 0.07, 0.07));
   //    wo->upon_async_model_loaded([wo, skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(122.15, 125.77, 4.635);
   //    wo->rotateAboutRelZ(144 * DEGtoRAD);
   //    wo->rotateAboutRelX(25 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //    fishingRod[1] = wo;
   //}

   //{
   //    // 120, 125, 6
   //    //line
   //    for (int i = 0; i < 3; i++)
   //    {
   //        WO* wo = WO::New(line2, Vector(0.07, 0.07, 0.07));
   //        wo->upon_async_model_loaded([wo, skin, this]()
   //            {
   //                rel_x = wo->getModel()->getBoundingBox().getlxlylz().z;
   //                ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //                spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //                //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //                wo->getModel()->getSkins().push_back(std::move(spidey));
   //                wo->getModel()->useNextSkin();
   //            });
   //        wo->setPosition(127.78, 133.17, 6.57);
   //        wo->rotateAboutRelZ(144 * DEGtoRAD);
   //        //wo->rotateAboutRelX(55 *DEGtoRAD);
   //        worldLst->push_back(wo);
   //        fishingLines[i] = wo;
   //    }
   //}

   //{
   //    // 120, 125, 6
   //    //bait
   //    WO* wo = WO::New(bait, Vector(0.07, 0.07, 0.07));
   //    wo->upon_async_model_loaded([wo, skin, this]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(127.795, 133.145, 4.655);
   //    wo->rotateAboutRelZ(144 * DEGtoRAD);
   //    wo->rotateAboutRelZ(25 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //    fishingRod[2] = wo;
   //}

   //{
   //    // 120, 125, 6
   //    //Hand
   //    WO* wo = WO::New(hand, Vector(0.08, 0.08, 0.08));
   //    //wo->upon_async_model_loaded([wo, skin, this]()
   //    //    {
   //    //        ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //    //        spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //    //        //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //    //        wo->getModel()->getSkins().push_back(std::move(spidey));
   //    //        wo->getModel()->useNextSkin();
   //    //    });
   //    wo->setPosition(122.15, 125.77, 4.635);
   //    wo->rotateAboutRelZ(240 * DEGtoRAD);
   //    //wo->rotateAboutRelX(25 * DEGtoRAD);
   //    worldLst->push_back(wo);
   //    fishingRod[1] = wo;
   //}

   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
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

   { 
      //Create the infinite grass plane (the floor)
      WO* wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      wo->upon_async_model_loaded( [wo]()
         {
            ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
            grassSkin.getMultiTextureSet().at( 0 ).setTexRepeats( 5.0f );
            grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
            grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
            grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
            grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
         } );
      wo->setLabel( "Grass" );
      //wo->isLockedWRTparent()
      //wo->getChildren()
      worldLst->push_back( wo );
   }

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

   GuiText* yo = GuiText::New(nullptr);
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
   this->worldLst->push_back(yo);
   fishtime->gui = yo;
   createFishGameWayPoints();
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
