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
       if(this->cam == firstPerson) firstPerson->spawnRod();
   }

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

   if(wo1->isDone)
        firstPerson->doneTerrain = wo1->isDone;
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
            cam->setPosition(Vector(1707.334229, 1710.420288, -63.118397));

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
            Vector camPos = Vector(this->cam->getPosition().x, this->cam->getPosition().y, 7);
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

   if( key.keysym.sym == SDLK_1 )
   {

       //std::map<std::string, int>::iterator it = firstPerson->inventory.begin();

       //// Iterate through the map and print the elements
       //while (it != firstPerson->inventory.end()) {
       //    std::cout << it->first << ": " << it->second << std::endl;
       //    ++it;
       //}

       std::string beachball(ManagerEnvironmentConfiguration::getSMM() + "/models/beachball.3ds");
       WOPxObj* box2 = WOPxObj::New(beachball, physics, scene, Vector(10, 10, 10), "circle");

       box2->setPosition(Vector(this->cam->getPosition().x, this->cam->getPosition().y, this->cam->getPosition().z + 10));
       this->worldLst->push_back(box2);


   }
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

   if (key.keysym.sym == SDLK_4)
   {
       if (wo1->isDone)
       {
           std::cout << "ASYNC DONE" << std::endl;
       }
       else
       {
           std::cout << "NOT DONE" << std::endl;
       }
   }

   if (key.keysym.sym == SDLK_5)
   {
       firstPerson->actor->controller->setPosition(PxExtendedVec3(1414, 2054, 9));
       //firstTest->setPosition(Vector(1414, 2054, -59));
        //hello->moveRelative(Vector(1, 0, 0));
   }

   if (key.keysym.sym == SDLK_6)
   {
        
       std::cout << firstTest->controller->getActor()->getGlobalPose().p.x << " , " << firstTest->controller->getActor()->getGlobalPose().p.y << " , " << firstTest->controller->getActor()->getGlobalPose().p.z << std::endl;

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

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1500.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   //this->cam->setPosition( 1414, 2054, -59 );
   this->cam->setPosition(0, 0, 5);
   this->cam->rotateAboutRelZ(-180 * DEGtoRAD);

   fishingRod.resize(3);
   fishingLines.resize(3);

   firstPerson = new CameraFirstPerson(this, this->cam->getMouseHandler());

   fishtime = new CameraFishing(this, this->cam->getMouseHandler());
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

   scene->setGravity(physx::PxVec3(0, 0, -9.81f));
   //articulation = physics->createArticulationReducedCoordinate();

   //PxMaterial* gMaterial = physics->createMaterial(0.9f, 0.5f, 0.6f);
   //PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 0, 1, 0), *gMaterial);//good for the grass
   ////groundPlane->setGlobalPose(PxTransform(PxVec3(0, 0, -1000)));
   //scene->addActor(*groundPlane);

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


   //std::string terrain(ManagerEnvironmentConfiguration::getLMM() + "models/Terrain/Snow.obj");
   //std::string terrain_skin(ManagerEnvironmentConfiguration::getLMM() + "models/Terrain/SM_DiffJPG.jpg");

   std::string terrain(ManagerEnvironmentConfiguration::getLMM() + "models/Snow2/Snow.obj");
   std::string terrain_skin(ManagerEnvironmentConfiguration::getLMM() + "models/Snow2/Normals.png");

   //std::string terrain(ManagerEnvironmentConfiguration::getLMM() + "models/SnowTerrain/SnowTerrain.3ds");
   //std::string terrain_skin(ManagerEnvironmentConfiguration::getLMM() + "models/SnowTerrain/686.jpg");

   //vendor = WO::New(shinyRedPlasticCube, Vector(1, 1, 1));
   //vendor->setPosition(0, 0, 10);
   //worldLst->push_back(vendor);

   //blocker = WO::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5));
   //blocker->setPosition(0, 8, 10);
   ////blocker->isVisible = false;
   //worldLst->push_back(blocker);

   {
       WOPxStatic* yes = WOPxStatic::New(shinyRedPlasticCube, Vector(10, 0, 0.5), Vector(1, 2, 0.5), MESH_SHADING_TYPE::mstAUTO, physics, scene);
       worldLst->push_back(yes);
   }

   {
       WOPxStatic* yes = WOPxStatic::New(shinyRedPlasticCube, Vector(12, 0, 0.9), Vector(1, 2, 0.5), MESH_SHADING_TYPE::mstAUTO, physics, scene);
       worldLst->push_back(yes);
   }

   {
       WOPxStatic* yes = WOPxStatic::New(shinyRedPlasticCube, Vector(14, 0, 1.1), Vector(1, 2, 0.5), MESH_SHADING_TYPE::mstAUTO, physics, scene);
       worldLst->push_back(yes);
   }

   {
       WOPxStatic* yes = WOPxStatic::New(shinyRedPlasticCube, Vector(14, 0, 1.9), Vector(1, 2, 0.5), MESH_SHADING_TYPE::mstAUTO, physics, scene);
       worldLst->push_back(yes);
   }
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
       WO* wo = WO::New(pole, Vector(0.01, 0.01, 0.01));
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
       //psudeo player

       //std::string beachball(ManagerEnvironmentConfiguration::getSMM() + "/models/beachball.3ds");
       //WOPxObj* box2 = WOPxObj::New(beachball, physics, scene, Vector(10, 10, 10), "sphere");

       //hello = WO::New(shinyRedPlasticCube, Vector(1, 1, 1));

       //box2->setPose(this->cam->getPose());
       //hello->setPose(this->cam->getPose());
       ////box2->setPosition(Vector(this->cam->getPosition().x, this->cam->getPosition().y, this->cam->getPosition().z));

       //worldLst->push_back(box2);
       //worldLst->push_back(hello);
       //box2->isVisible = false;
       //hello->isVisible = false;

       //firstPerson->actor = box2;
       //firstPerson->actorMover = hello;
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

   //{
   //    //mountain
   //    WO* wo = WO::New(terrain, Vector(100, 100, 100));
   //    //wo->rotateAboutRelX(-90 * DEGtoRAD);
   //    wo->setPosition(0, 0, 180);
   //    wo->upon_async_model_loaded([wo, terrain_skin, this]()
   //        {

   //            float* vertexListCopy = nullptr;
   //            unsigned int* indicesCopy = nullptr;
   //            physx::PxRigidActor* a = nullptr;

   //            size_t vertexListSize = wo->getModel()->getModelDataShared()->getCompositeVertexList().size();
   //            size_t indexListSize = wo->getModel()->getModelDataShared()->getCompositeIndexList().size();

   //            vertexListCopy = new float[vertexListSize * 3];//might be a better way to do this without making a copy
   //            indicesCopy = new unsigned int[indexListSize];//assuming the composite lists are stored in contiguous memory

   //            for (size_t i = 0; i < vertexListSize; i++)
   //            {
   //                vertexListCopy[i * 3 + 0] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).x;
   //                vertexListCopy[i * 3 + 1] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).y;
   //                vertexListCopy[i * 3 + 2] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).z;
   //            }

   //            for (size_t i = 0; i < indexListSize; i++)
   //                indicesCopy[i] = wo->getModel()->getModelDataShared()->getCompositeIndexList().at(i);

   //            physx::PxTriangleMeshDesc meshDesc;
   //            meshDesc.points.count = vertexListSize;
   //            meshDesc.points.stride = sizeof(float) * 3;//tightly packaged
   //            meshDesc.points.data = vertexListCopy;

   //            meshDesc.triangles.count = indexListSize / 3;
   //            meshDesc.triangles.stride = 3 * sizeof(unsigned int);//aside about index lists here
   //            meshDesc.triangles.data = indicesCopy;

   //            physx::PxDefaultMemoryOutputStream writeBuffer;
   //            physx::PxTriangleMeshCookingResult::Enum result;
   //            physx::PxCookingParams param = physx::PxCookingParams(physx::PxTolerancesScale());

   //            bool status = PxCookTriangleMesh(param, meshDesc, writeBuffer, &result);
   //            if (!status)
   //            {
   //                std::cout << "Failed to create Triangular mesh" << std::endl;
   //                std::cin.get();
   //            }

   //            physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
   //            physx::PxTriangleMesh* mesh = physics->createTriangleMesh(readBuffer);

   //            physx::PxMaterial* gMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);
   //            physx::PxShape* shape = physics->createShape(physx::PxTriangleMeshGeometry(mesh), *gMaterial, true);
   //            physx::PxTransform t({ 0,0,0 });

   //            a = physics->createRigidStatic(t);
   //            a->attachShape(*shape);

   //            a->userData = this;
   //            scene->addActor(*a);

   //            //ModelMeshSkin& tex1 = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(terrain_skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();

   //            wo->setPose(wo->getPose());

   //            physx::PxMat44 m;
   //            for (int i = 0; i < 16; i++) m(i % 4, i / 4) = wo->getPose().at(i);

   //            a->setGlobalPose(physx::PxTransform(m));

   //        });
   //    //wo->rotateAboutRelZ(45 * DEGtoRAD);
   //    worldLst->push_back(wo);
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
