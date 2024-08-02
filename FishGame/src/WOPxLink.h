#pragma once
#include "WOPhysX.h"

using namespace physx;

class WOPxLink : public WOPhysX
{
public:
	static WOPxLink* New(const std::string& path, physx::PxPhysics* p, physx::PxArticulationReducedCoordinate* a, Aftr::Mat4 pose, physx::PxArticulationLink* parent = nullptr, const Aftr::Vector& scale = Aftr::Vector(1, 1, 1))
	{
		WOPxLink* wo = new WOPxLink();

		wo->WO::onCreate(path, scale);

		wo->setPose(pose);

		wo->WO::upon_async_model_loaded([wo, p, a, pose, parent]()
			{
				//wo->WOPhysX::setPose(wo->WO::getPose());
				wo->createActor(p, a, wo->getPxPose(), parent);
			});

		return wo;
	}
	void createActor(physx::PxPhysics* p, physx::PxArticulationReducedCoordinate* a, PxMat44 pose, physx::PxArticulationLink* parent = nullptr)
	{
		link = a->createLink(parent, PxTransform(pose));

		Aftr::Vector bb = this->getModel()->getBoundingBox().getlxlylz() * 0.5;
		physx::PxMaterial* gMaterial = p->createMaterial(0.6f, 0.6f, 0.5f);
		physx::PxShape* shape = p->createShape(physx::PxBoxGeometry(bb[0], bb[1], bb[2]), *gMaterial, true);

		link->attachShape(*shape);
		PxRigidBodyExt::updateMassAndInertia(*link, 1.0f);

		if (parent != nullptr)
		{
			PxVec3 parentOffset(-4.0, 0.0, 20.0);
			PxVec3 childOffset(4.0, 0.0, 20.0);
			PxArticulationJointReducedCoordinate* joint = link->getInboundJoint();
			joint->setParentPose(PxTransform(childOffset));
			joint->setChildPose(PxTransform(childOffset));
			joint->setJointType(PxArticulationJointType::eREVOLUTE);
			joint->setMotion(PxArticulationAxis::eSWING2, PxArticulationMotion::eFREE);
		}

		link->userData = this;
	}

	void createActor(physx::PxPhysics* p, physx::PxScene* s)
	{
		return;
	}

	void updatePoseFromPhysicsEngine()
	{
		physx::PxMat44 m;
			
		m = link->getGlobalPose();

		Aftr::Mat4 m2;

		for (int i = 0; i < 16; i++) m2[i] = m(i % 4, i / 4);

		WO::setPose(m2);
	}

	physx::PxArticulationLink* link;

protected:
	WOPxLink() : Aftr::IFace(this)
	{
	}
};
