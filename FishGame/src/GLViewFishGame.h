#pragma once

#include "GLView.h"
#include "CameraFirstPerson.h"
#include "PxPhysicsAPI.h"
#include "WOPxLink.h"
#include "WOPhysX.h"
#include "WOPxObj.h"

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

protected:
   GLViewFishGame( const std::vector< std::string >& args );
   virtual void onCreate(); 

   physx::PxDefaultAllocator a;
   physx::PxDefaultErrorCallback e;
};

/** \} */

} //namespace Aftr
