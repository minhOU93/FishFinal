#pragma once

#include "WOPhysX.h"

class WOPxStatic : public WOPhysX
{
public:
	static WOPxStatic* New(const std::string& path, const Aftr::Vector& pos, const Aftr::Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxPhysics* p, physx::PxScene* s)
	{
		WOPxStatic* wo = new WOPxStatic();

		//wo->WO::onCreate(path);

		wo->onCreate(path, scale, pos, mst, p);

		wo->WO::setPosition(pos);

		wo->WO::upon_async_model_loaded([wo, p, s]()
			{
				wo->createActor(p, s);
				wo->setPosition(wo->WO::getPosition());
			});

		//wo->upon_async_model_loaded([p, s, wo, pos]() {
		//	wo->createActor(p, s);
		//	wo->setPosition(pos);
		//	});
		return wo;

	}

	void createActor(physx::PxPhysics* p, physx::PxScene* s)
	{
		Aftr::Vector bb = this->getModel()->getBoundingBox().getlxlylz() * 0.5; // bb[0], bb[2], bb[1]);
		//Aftr::Vector bb(1, 1, 1);

		physx::PxMaterial* gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
		physx::PxShape* shape = p->createShape(physx::PxBoxGeometry(bb[0], bb[1], bb[2]), *gMaterial, true);


		physx::PxTransform t({ 0,0,0 });

		b = p->createRigidStatic(t);

		b->attachShape(*shape);

		b->userData = this;
		s->addActor(*b);
	}

	physx::PxTransform getTransform() const
	{
		Aftr::Mat4 pose = this->getPose();
		physx::PxMat44 m;

		for (int i = 0; i < 16; i++) m(i % 4, i / 4) = pose[i];

		return physx::PxTransform(m);
	}

	void createActor(physx::PxPhysics* p, physx::PxArticulationReducedCoordinate* a, physx::PxMat44 pose, physx::PxArticulationLink* parent = nullptr) {
		return;
	}
protected:
	WOPxStatic() : WOPhysX(), Aftr::IFace(this)
	{
	}
};