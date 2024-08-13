#pragma once

#include "WOPxObj.h"

using namespace Aftr;
using namespace physx;

class WOPxKinematic : public WOPxDynamic
{
public:
	static WOPxKinematic* New(std::string objpath, physx::PxPhysics* p, physx::PxScene* s, Vector size = Vector(1, 1, 1), std::string type = "box")
	{
		WOPxKinematic* wo = new WOPxKinematic();

		wo->WO::onCreate(objpath, size);

		wo->set_shape_type(type);

		wo->WO::upon_async_model_loaded([wo, p, s]()
			{
				wo->createActor(p, s);
				wo->WOPxDynamic::setPosition(wo->WO::getPosition());

				wo->a->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			});

		return wo;

	}

	void setPose(const Aftr::Mat4& pose) noexcept override
	{
		WO::setPose(pose);

		physx::PxMat44 m;
		for (int i = 0; i < 16; i++) m(i % 4, i / 4) = pose[i];

		if (a != nullptr)
			a->setKinematicTarget(physx::PxTransform(m));

	}

	void setPosition(const Aftr::Vector& p) override
	{
		WO::setPosition(p);

		if (a != nullptr)
		{
			physx::PxTransform m = a->getGlobalPose();
			m.p = physx::PxVec3(p[0], p[1], p[2]);
			a->setKinematicTarget(m);
		}

	}

	void moveRelative(Vector p)
	{
		WO::moveRelative(p);

		if (a != nullptr)
		{
			physx::PxTransform m = a->getGlobalPose();
			m.p = physx::PxVec3(p[0], p[1], p[2]);
			a->setKinematicTarget(m);
		}
	}
protected:
	WOPxKinematic() : Aftr::IFace(this)
	{
	}
};