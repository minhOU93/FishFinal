#pragma once

#include "GLView.h"
#include "CameraFirstPerson.h"
#include "PxPhysicsAPI.h"
#include "WOPxLink.h"
#include "WOPhysX.h"
#include "WOPxObj.h"
#include "WOPxStatic.h"
#include "WOString.h"
#include "GuiText.h"
#include "CameraFishing.h"
#include "WOPxKinematic.h"
#include "Terrain.h"
#include "WOPxController.h"
#include "CameraShop.h"
#include "Cat.h"
#include "irrKlang.h"

namespace Aftr
{
   class Camera;

/**
   \class GLViewFishGame
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewFishGame : public GLView
{
public:
   static GLViewFishGame* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewFishGame();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createFishGameWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   void createFishingRod();
   const Uint8* keystates = SDL_GetKeyboardState(NULL);
    

   WO* vendor = nullptr;

   WOPxObj* holder = nullptr;
   //WOPxLink* test = nullptr;

   WOPxStatic* anchor = nullptr;
   WOPxStatic* anchor2 = nullptr;
   physx::PxReal mass = 0;

   physx::PxD6Joint* joint = nullptr;
   physx::PxSphericalJoint* Stringjoint = nullptr;
   physx::PxControllerManager* controllerManager = nullptr;

   physx::PxController* controller;
   physx::PxControllerCollisionFlags collisionFlags;
   physx::PxControllerFilters yo;
   
   std::string reelSound = ManagerEnvironmentConfiguration::getLMM() + "sounds/REEL_IN2.ogg";
   std::string reelSound2 = ManagerEnvironmentConfiguration::getLMM() + "sounds/REEL_OUT2.ogg";
   std::string reelSound3 = ManagerEnvironmentConfiguration::getLMM() + "sounds/FISH_STRUGGLE.ogg";

   std::string helloTalk = ManagerEnvironmentConfiguration::getLMM() + "sounds/TALK.ogg";
   std::string walkingSound = ManagerEnvironmentConfiguration::getLMM() + "sounds/WALKING.ogg";
   std::string bgm = ManagerEnvironmentConfiguration::getLMM() + "sounds/Gloscien.mp3";

   std::string winSource = ManagerEnvironmentConfiguration::getLMM() + "sounds/WIN.ogg";

   irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
   irrklang::ISoundSource* reelInSound = soundEngine->addSoundSourceFromFile(reelSound.c_str());
   irrklang::ISoundSource* reelOutSound = soundEngine->addSoundSourceFromFile(reelSound2.c_str());
   irrklang::ISoundSource* fishStruggleSound = soundEngine->addSoundSourceFromFile(reelSound3.c_str());

   irrklang::ISoundSource* talkSound = soundEngine->addSoundSourceFromFile(helloTalk.c_str());
   irrklang::ISoundSource* walkSound = soundEngine->addSoundSourceFromFile(walkingSound.c_str());

   irrklang::ISoundSource* enoMusic = soundEngine->addSoundSourceFromFile(bgm.c_str());
   irrklang::ISoundSource* winSound = soundEngine->addSoundSourceFromFile(winSource.c_str());

   irrklang::ISound* playSong;

   Vector rayOutput;
   WO* hello;

   Terrain* wo1;

   AftrGeometricTerm occulude;
   Cat* cat;
   WOPxStatic* chairModel;
   WOPxStatic* boxModel;
   GuiText* mainGui;

   WO* blocker = nullptr;
   WOPxController* firstTest = nullptr;

   WOString* pressF = nullptr;
   float object_xyz[3] = { 0, 0, 0 };
   float global_xyz[3] = { 0, 0, 0 };

   float global_xyz_prev[3] = { 0, 0, 0 };
   float object_xyz_prev[3] = { 0, 0, 0 };

   float pos = 40.0f;

   //physx::PxFixedJoint* joint = nullptr;

   CameraFishing* fishtime;
   CameraFirstPerson* firstPerson;
   CameraShop* shop;

   CameraShop* loadingCam;

   //////////////////////////////////////////////////

   std::vector<WO*> fishingRod;
   std::vector<WO*> fishingLines;

   float rel_x;

protected:
   GLViewFishGame( const std::vector< std::string >& args );
   virtual void onCreate(); 

   physx::PxDefaultAllocator a;
   physx::PxDefaultErrorCallback e;
   physx::PxArticulationReducedCoordinate* articulation;

};

/** \} */

} //namespace Aftr
