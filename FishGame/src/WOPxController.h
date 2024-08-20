#pragma once

#include "WOPhysX.h"

using namespace Aftr;
using namespace physx;

class WOPxController : public WOPhysX
{
public:
	static WOPxController* New(std::string path, physx::PxPhysics* p, physx::PxScene* s, physx::PxControllerManager* c, Aftr::Vector pos = Vector(0, 0, 2), Vector scale = Vector(1, 1, 1), MESH_SHADING_TYPE mst = MESH_SHADING_TYPE::mstAUTO)
	{
		WOPxController* wo = new WOPxController();

        wo->onCreate(path, scale, pos, mst, p);

        wo->WO::setPosition(pos);

        wo->WO::upon_async_model_loaded([wo, p, s, c, pos]()
            {
                wo->createActor(p, s, c, pos);
                wo->setPosition(wo->WO::getPosition());
            });

        //wo->upon_async_model_loaded([p, s, wo, pos]() {
        //	wo->createActor(p, s);
        //	wo->setPosition(pos);
        //	});
        return wo;
	}

	void createActor(physx::PxPhysics* p, physx::PxScene* s, physx::PxControllerManager* c, Aftr::Vector pos)
	{
        physx::PxMaterial* gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
        PxCapsuleControllerDesc controllerDesc;
        controllerDesc.height = 12.0f;
        controllerDesc.radius = 0.5f;
        controllerDesc.climbingMode = PxCapsuleClimbingMode::eEASY;
        controllerDesc.slopeLimit = cosf(PxPi / 4);
        controllerDesc.stepOffset = 1.5f;
        controllerDesc.position = PxExtendedVec3(pos.x, pos.y, pos.z);
        controllerDesc.material = gMaterial;
        controllerDesc.upDirection = PxVec3(0, 0, 1);

        controller = c->createController(controllerDesc);
        controller->getActor()->userData = this;
	}

    void createActor(physx::PxPhysics* p, physx::PxScene* s)
    {
        return;
    }

    //physx::PxController* controller;

protected:
    WOPxController() : WOPhysX(), Aftr::IFace(this)
    {
    }
};