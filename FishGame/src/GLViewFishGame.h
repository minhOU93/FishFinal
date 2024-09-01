#pragma once

#include "GLView.h"
#include "CameraFirstPerson.h"
#include "PxPhysicsAPI.h"
#include "WOPhysX.h"
#include "WOPxObj.h"
#include "WOPxStatic.h"
#include "WOString.h"
#include "GuiText.h"
#include "CameraFishing.h"
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

   //virtual int handleEvent(const SDL_Event& sdlEvent);

   const Uint8* keystates = SDL_GetKeyboardState(NULL);
    
   //WOPxLink* test = nullptr;

   physx::PxReal mass = 0;

   physx::PxControllerManager* controllerManager = nullptr;

   physx::PxController* controller;
   physx::PxControllerCollisionFlags collisionFlags;
   physx::PxControllerFilters yo;

   irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

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

   int invX, invY;
   callback_func_on_SDL_Event getMouseEvent;
   float pos = 40.0f;

   //physx::PxFixedJoint* joint = nullptr;

   CameraFishing* fishtime;
   CameraFirstPerson* firstPerson;
   CameraShop* shop;

   CameraShop* loadingCam;

   //////////////////////////////////////////////////

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
