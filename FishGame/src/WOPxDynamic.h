#pragma once
#include "WOPhysX.h"

class WOPxDynamic : public WOPhysX
{
public:

	static WOPxDynamic* New(const std::string& path, const Aftr::Vector& scale, const Aftr::Vector& pos, Aftr::MESH_SHADING_TYPE mst, physx::PxPhysics* p, physx::PxScene* s)
	{
		WOPxDynamic* wo = new WOPxDynamic();
		wo->onCreate(path, scale, pos, mst, p);
		return wo;
	}

	void createActor(physx::PxPhysics* p, physx::PxScene* s)
	{
		Aftr::Vector bb = this->getModel()->getBoundingBox().getlxlylz() * 0.5; // bb[0], bb[2], bb[1]);
	    //Aftr::Vector bb(1, 1, 1);

		physx::PxMaterial * gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
		physx::PxShape* shape = nullptr;

		if (shape_type == "circle")
		{
			gMaterial = p->createMaterial(0.6f, 0.6f, 0.6f);
			shape = p->createShape(physx::PxSphereGeometry(bb[0]), *gMaterial, true);
		}
		else
			shape = p->createShape(physx::PxBoxGeometry(bb[0], bb[1], bb[2]), *gMaterial, true);

		physx::PxTransform t({ 0,0,0 });

		a = p->createRigidDynamic(t);

		a->attachShape(*shape);
		
		a->userData = this;
		s->addActor(*a);

	}


	void set_shape_type(std::string input) {
		shape_type = input;
	}

	void createActor(physx::PxPhysics* p, physx::PxArticulationReducedCoordinate* a, physx::PxMat44 pose, physx::PxArticulationLink* parent = nullptr) {
		return;
	}

protected:
	WOPxDynamic() : WOPhysX(), Aftr::IFace(this)
	{
	}

	std::string shape_type;

};