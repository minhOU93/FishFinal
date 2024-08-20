#pragma once
#include "PxPhysicsAPI.h"
#include "WO.h"
#include "Mat4.h"
#include "Model.h"
#include <iostream>

class WOPhysX : public virtual Aftr::WO
{
	public:

		physx::PxRigidDynamic* a = nullptr;
		physx::PxRigidStatic* b = nullptr;
		physx::PxController* controller = nullptr;

		void onCreate(const std::string& path, const Aftr::Vector& scale, const Aftr::Vector& pos, Aftr::MESH_SHADING_TYPE mst, physx::PxPhysics* p)
		{
			WO::onCreate(path, scale, mst);
			this->upon_async_model_loaded([this, p, pos]()
			{
				// Aftr::Vector bb = this->getModel()->getBoundingBox().getlxlylz() * 0.5;
				//createActor(p, s);
				this->setPosition(this->getPosition());
				//this->setPosition(pos);
			}
			);
			//this->setPosition(pos);
			
		}

		virtual void updatePoseFromPhysicsEngine()
		{
			physx::PxMat44 m;

			if (a != nullptr) m = a->getGlobalPose();
			else if (b != nullptr) m = b->getGlobalPose();
			else if (controller != nullptr) m = controller->getActor()->getGlobalPose();
			

			Aftr::Mat4 m2;
			
			for (int i = 0; i < 16; i++) m2[i] = m(i % 4, i / 4);

			WO::setPose(m2);
			//setPose(m2);
		}

		void setPose(const Aftr::Mat4& pose) noexcept override
		{
			WO::setPose(pose);

			physx::PxMat44 m;
			for (int i = 0; i < 16; i++) m(i % 4, i / 4) = pose[i];

			if (a != nullptr)
				a->setGlobalPose(physx::PxTransform(m));
			else if (b != nullptr)
				b->setGlobalPose(physx::PxTransform(m));
	
		}

		physx::PxMat44 getPxPose() // this could be wrong
		{
			Aftr::Mat4 pose = WO::getPose();

			physx::PxMat44 m;
			for (int i = 0; i < 16; i++) m(i % 4, i / 4) = pose[i];

			return m;
		}

		void setPosition(const Aftr::Vector& p) override
		{
			WO::setPosition(p);

			if (a != nullptr)
			{
				physx::PxTransform m = a->getGlobalPose();
				m.p = physx::PxVec3(p[0], p[1], p[2]);
				a->setGlobalPose(m);
			}
			else if (b != nullptr)
			{
				physx::PxTransform m = b->getGlobalPose();
				m.p = physx::PxVec3(p[0], p[1], p[2]);
				b->setGlobalPose(m);
			}
		
		}
		
		//void moveRelative(const Aftr::Vector& dXdYdZ) override
		//{
		//	WO::moveRelative(dXdYdZ);

		//	//if (a != nullptr) {
		//	//	physx::PxTransform m = a->getGlobalPose();
		//	//	m.p = physx::PxVec3(dXdYdZ[0], dXdYdZ[1], dXdYdZ[2]);
		//	//	a->setGlobalPose(m);
		//	//}
		//}

		virtual void createActor(physx::PxPhysics* p, physx::PxScene* s) = 0;

		virtual void createActor(physx::PxPhysics* p, physx::PxScene* s, physx::PxControllerManager* c, Aftr::Vector pos) = 0;

		~WOPhysX()
		{
		}
	protected:
		WOPhysX() : Aftr::IFace(this)
		{
		}

};