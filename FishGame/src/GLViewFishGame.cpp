#include "GLViewFishGame.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"
#include "AftrUtilities.h"

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

   //Vector camLookDir = this->cam->getLookDirection();
   //Vector camNormalDir = this->cam->getNormalDirection();
   //Vector camPos = this->cam->getPosition();

   //this->cam = new CameraFirstPerson(this, this->cam->getMouseHandler());

   //this->cam->setPosition(camPos);
   //this->cam->setCameraNormalDirection(camNormalDir);
   //this->cam->setCameraLookDirection(camLookDir);
   //this->cam->startCameraBehavior();
   //this->cam->setLabel("Camera"); 

   this->setActorChaseType(STANDARDEZNAV);
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
}


void GLViewFishGame::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if( key.keysym.sym == SDLK_1 )
   {
       occulude = blocker->getNearestPointWhereLineIntersectsMe(vendor->getPosition(), this->cam->getPosition(), rayOutput);

       std::cout << "(" << rayOutput.x << ", " << rayOutput.y << ", " << rayOutput.z << ")" << std::endl;
       if (occulude == AftrGeometricTerm::geoSUCCESS)
       {
           std::cout << "POINT FOUND" << std::endl;
       }
       else
       {
           std::cout << "NO POINT FOUND" << std::endl;
       }
   }
   if (key.keysym.sym == SDLK_2)
   {
       int offset = sticks[0]->getModel()->getBoundingBox().getlxlylz().x;
       //joint->drive

       joint = PxD6JointCreate(*physics, anchor->b, PxTransform(PxVec3(offset - 0.9, 0, 0)), sticks[0]->a, PxTransform(PxVec3(-offset + 0.9, 0, 0)));
       //joint->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
       //joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);

       for (int i = 0; i < sticks.size() - 1; i++)
       {
           std::cout << offset << std::endl;
           joint = PxD6JointCreate(*physics, sticks[i]->a, PxTransform(PxVec3(offset - 0.9, 0, 0)), sticks[i + 1]->a, PxTransform(PxVec3(-offset + 0.9, 0, 0)));
           //sticks[i]->a->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
           //joint->setDrive(PxD6Drive::eX, PxD6JointDrive(1000, 100, FLT_MAX, true));
           //joint->setDrive(PxD6Drive::eY, PxD6JointDrive(1000, 100, FLT_MAX, true));
           //joint->setDrive(PxD6Drive::eZ, PxD6JointDrive(1000, 100, FLT_MAX, true));
           //joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
       }
       joint = PxD6JointCreate(*physics, sticks[9]->a, PxTransform(PxVec3(0, 0, -2)), strings[0]->a, PxTransform(PxVec3(0, 0, 2)));

       //physx::PxFixedJoint* joint = nullptr;

       //joint = PxFixedJointCreate(*physics, anchor->b, PxTransform(PxVec3(offset - 1, 0, 0)), sticks[0]->a, PxTransform(PxVec3(-offset + 1, 0, 0)));
       //joint->setBreakForce(1000, 100000);
       //joint->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
       //joint->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);

       //for (int i = 0; i < sticks.size() - 1; i++)
       //{
       //    std::cout << offset << std::endl;
       //    sticks[i]->a->setMass(0.1f);
       //    joint = PxFixedJointCreate(*physics, sticks[i]->a, PxTransform(PxVec3(offset - 1, 0, 0)), sticks[i + 1]->a, PxTransform(PxVec3(-offset + 1, 0, 0)));
       //    joint->setBreakForce(10000000, 100000);
       //    joint->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
       //    joint->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);
       //}
   }
   if (key.keysym.sym == SDLK_3)
   {
       joint->setDrive(PxD6Drive::eY, PxD6JointDrive(0, 1000, FLT_MAX, true));
       joint->setDriveVelocity(PxVec3(0.0f, -50.0f, 0.0f), PxVec3(0.0f), true);
   }
   if (key.keysym.sym == SDLK_SPACE)
   {
       std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");

       mass += 1;
       strings[0]->a->addForce(PxVec3(0, 0, -500));
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

   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );


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
   articulation = physics->createArticulationReducedCoordinate();

   PxMaterial* gMaterial = physics->createMaterial(0.9f, 0.5f, 0.6f);
   PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 0, 1, 0), *gMaterial);//good for the grass
   scene->addActor(*groundPlane);

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );

   std::string pole(ManagerEnvironmentConfiguration::getLMM() + "models/pole.fbx");
   std::string reel(ManagerEnvironmentConfiguration::getLMM() + "models/reel.fbx");
   std::string line(ManagerEnvironmentConfiguration::getLMM() + "models/line.fbx");
   std::string bait(ManagerEnvironmentConfiguration::getLMM() + "models/bait.fbx");


   std::string skin(ManagerEnvironmentConfiguration::getLMM() + "models/uv_map.png");
   std::string beachball(ManagerEnvironmentConfiguration::getSMM() + "/models/beachball.3ds");
   std::string fontArial(ManagerEnvironmentConfiguration::getSMM() + "/fonts/arial.ttf");

   //pressF = WOString::New("Press F to Shop", fontArial, 240, 10);
   //worldLst->push_back(pressF);

   //pressF->setPosition(0, 5, 5);

   //WOPxObj* box2 = WOPxObj::New(beachball, physics, scene, Vector(10, 10, 10), "circle");
   //box2->setPosition(Vector(0, 0, 50));
   //worldLst->push_back(box2);

   vendor = WO::New(shinyRedPlasticCube, Vector(1, 1, 1));
   vendor->setPosition(0, 0, 10);
   worldLst->push_back(vendor);

   //blocker = WO::New(shinyRedPlasticCube, Vector(1, 1, 1));
   //blocker->setPosition(0, 8, 10);
   ////blocker->isVisible = false;
   //worldLst->push_back(blocker);

   anchor = WOPxStatic::New(shinyRedPlasticCube, Vector(0, 0, 50), Vector(1, 0.5, 0.5), MESH_SHADING_TYPE::mstAUTO, physics, scene);
   worldLst->push_back(anchor);

   sticks.resize(10);
   strings.resize(10);
   int offset = 4;
   for (int i = 0; i < sticks.size(); i++)
   {
       sticks[i] = WOPxObj::New(shinyRedPlasticCube, physics, scene, Vector(1, 0.5, 0.5));
       sticks[i]->setPosition(Vector(offset, 0, 50));
       worldLst->push_back(sticks[i]);
       offset += 4;
   }
   
   strings[0] = WOPxObj::New(shinyRedPlasticCube, physics, scene, Vector(0.5, 0.5, 1.5));
   strings[0]->setPosition(Vector(offset - 4, 0, 45));
   worldLst->push_back(strings[0]);
   holder = strings[0];


   //{
   //    //pole
   //    WO* wo = WO::New(pole, Vector(0.1, 0.1, 0.1));
   //    wo->upon_async_model_loaded([wo, skin]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 0, 20);
   //    worldLst->push_back(wo);
   //}

   //{
   //    //bait
   //    WO* wo = WO::New(bait, Vector(0.1, 0.1, 0.1));
   //    wo->upon_async_model_loaded([wo, skin]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 0, 20);
   //    worldLst->push_back(wo);
   //}

   //{
   //    //line
   //    WO* wo = WO::New(line, Vector(0.1, 0.1, 0.1));
   //    wo->upon_async_model_loaded([wo, skin]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 0, 20);
   //    worldLst->push_back(wo);
   //}

   //{
   //    //reel
   //    WO* wo = WO::New(reel, Vector(0.1, 0.1, 0.1));
   //    wo->upon_async_model_loaded([wo, skin]()
   //        {
   //            ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
   //            spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
   //            //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
   //            wo->getModel()->getSkins().push_back(std::move(spidey));
   //            wo->getModel()->useNextSkin();
   //        });
   //    wo->setPosition(0, 0, 20);
   //    worldLst->push_back(wo);
   //}
   //
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
      ////Create the infinite grass plane (the floor)
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
   //gui->setLabel( "My Gui" );
   //gui->subscribe_drawImGuiWidget(
   //   [this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
   //   {
   //        //WOImGui::draw_AftrImGui_Demo(gui);
   //        //yo->drawImGui_for_this_frame();
   //        //if (ImGui::SliderAngle("Rel X Axis", &object_xyz[0], -180, 180)) {
   //        //    pressF->getModel()->rotateAboutRelX(object_xyz[0] - object_xyz_prev[0]);
   //        //    object_xyz_prev[0] = object_xyz[0];
   //        //}

   //        //if (ImGui::SliderAngle("Rel Y Axis", &object_xyz[1], -180, 180)) {
   //        //    pressF->getModel()->rotateAboutRelY(object_xyz[1] - object_xyz_prev[1]);
   //        //    object_xyz_prev[1] = object_xyz[1];
   //        //}

   //        //if (ImGui::SliderAngle("Rel Z Axis", &object_xyz[2], -180, 180)) {
   //        //    pressF->getModel()->rotateAboutRelZ(object_xyz[2] - object_xyz_prev[2]);
   //        //    object_xyz_prev[2] = object_xyz[2];
   //        //}
   //    

   //        // if (ImGui::SliderAngle("Global X Axis", &global_xyz[0], -180, 180)) {
   //        //     pressF->rotateAboutGlobalX(global_xyz[0] - global_xyz_prev[0]);
   //        //     global_xyz_prev[0] = global_xyz[0];
   //        // }

   //        // if (ImGui::SliderAngle("Global Y Axis", &global_xyz[1], -180, 180)) {
   //        //     pressF->rotateAboutGlobalY(global_xyz[1] - global_xyz_prev[1]);
   //        //     global_xyz_prev[1] = global_xyz[1];
   //        // }

   //        // if (ImGui::SliderAngle("Global Z Axis", &global_xyz[2], -180, 180)) {
   //        //     pressF->rotateAboutGlobalZ(global_xyz[2] - global_xyz_prev[2]);
   //        //     global_xyz_prev[2] = global_xyz[2];
   //        // }
   //   } );
   //this->worldLst->push_back( gui );
   this->worldLst->push_back(yo);
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
