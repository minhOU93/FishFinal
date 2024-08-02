#pragma once

#include "WOPxDynamic.h"

using namespace Aftr;

class WOPxObj : public WOPxDynamic
{
public:
	static WOPxObj* New(std::string objpath, physx::PxPhysics* p, physx::PxScene* s, Vector size = Vector(1, 1, 1), std::string type = "box")
	{
		WOPxObj* wo = new WOPxObj();

		wo->WO::onCreate(objpath, size);

		wo->set_shape_type(type);

		wo->WO::upon_async_model_loaded([wo, p, s]()
		{
			wo->createActor(p, s);
			wo->WOPxDynamic::setPosition(wo->WO::getPosition());
		});

		return wo;

	}

	void jump()
	{
		a->addForce(physx::PxVec3(0, 0, 1000));
	}
	
	void set_mass(physx::PxReal value)
	{
		a->setMass(value);
	}

	physx::PxTransform getTransform(Aftr::Mat4 pose) const
	{
		physx::PxMat44 m;

		for (int i = 0; i < 16; i++) m(i % 4, i / 4) = pose[i];

		return physx::PxTransform(m);
	}

protected:

	WOPxObj() : Aftr::IFace(this)
	{
	}
};

//void jump()
//{
//	a->addForce(physx::PxVec3(0, 0, 1000));
//}
//
//void set_mass(physx::PxReal value)
//{
//	a->setMass(value);
//}

