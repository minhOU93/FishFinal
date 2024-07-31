#pragma once
#include "WOPhysX.h"

namespace physx
{
	class WOPxLink : public WOPhysX
	{
	public:
		static WOPxLink* New(const std::string& path, physx::PxPhysics* p, physx::PxArticulationReducedCoordinate* a, Aftr::Mat4 pose, const Aftr::Vector& scale = Aftr::Vector(1, 1, 1))
		{
			WOPxLink* wo = new WOPxLink();

			wo->WO::onCreate(path, scale);

			wo->setPose(pose);

			wo->WO::upon_async_model_loaded([wo, p, a, pose]()
				{
					//wo->WOPhysX::setPose(wo->WO::getPose());
					wo->createActor(p, a, wo->getPxPose());
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
		}

		void createActor(physx::PxPhysics* p, physx::PxScene* s)
		{
			return;
		}

		physx::PxArticulationLink* link;

	protected:
		WOPxLink() : WOPhysX(), Aftr::IFace(this)
		{
		}
	};
}