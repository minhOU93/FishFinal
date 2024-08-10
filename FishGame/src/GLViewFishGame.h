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

   const Uint8* keystates = SDL_GetKeyboardState(NULL);

   WO* vendor = nullptr;

   WOPxObj* holder = nullptr;
   WOPxLink* test = nullptr;

   WOPxStatic* anchor = nullptr;
   WOPxStatic* anchor2 = nullptr;
   physx::PxReal mass = 0;

   physx::PxD6Joint* joint = nullptr;
   physx::PxSphericalJoint* Stringjoint = nullptr;

   std::vector<WOPxObj*> sticks;
   std::vector<WOPxObj*> strings;

   Vector rayOutput;

   AftrGeometricTerm occulude;

   WO* blocker = nullptr;

   WOString* pressF = nullptr;
   float object_xyz[3] = { 0, 0, 0 };
   float global_xyz[3] = { 0, 0, 0 };

   float global_xyz_prev[3] = { 0, 0, 0 };
   float object_xyz_prev[3] = { 0, 0, 0 };

   float pos = 40.0f;

   //physx::PxFixedJoint* joint = nullptr;

   CameraFishing* fishtime;

protected:
   GLViewFishGame( const std::vector< std::string >& args );
   virtual void onCreate(); 

   physx::PxDefaultAllocator a;
   physx::PxDefaultErrorCallback e;
   physx::PxArticulationReducedCoordinate* articulation;

};

/** \} */

} //namespace Aftr
