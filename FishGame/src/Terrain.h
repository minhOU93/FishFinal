#pragma once

#include "WOGridECEFElevation.h"
#include "PxPhysicsAPI.h"
#include "Model.h"
#include "WOString.h"

using namespace Aftr;

class Terrain : public Aftr::WOGridECEFElevation
{
public:
	static Terrain* New(std::string& path, std::string& texture, physx::PxPhysics* p, physx::PxScene* s, Aftr::VectorD upperLeft, Aftr::VectorD lowerRight, unsigned int grainularity,
		Aftr::VectorD scale, Aftr::VectorD offset)
	{
		Terrain* wo = new Terrain();

		wo->onCreate(path, texture, p, s, upperLeft, lowerRight, grainularity, scale, offset);

		return wo;

	}

	void onCreate(std::string& path, std::string& texture, physx::PxPhysics* p, physx::PxScene* s, Aftr::VectorD upperLeft, Aftr::VectorD lowerRight, unsigned int grainularity,
		 Aftr::VectorD scale, Aftr::VectorD offset)
	{
		WOGridECEFElevation::onCreate(upperLeft, lowerRight, grainularity, offset, scale, path, 2, 0, true);

		this->WO::upon_async_model_loaded([this, texture, offset, p, s](){
				size_t vertexListSize = this->getModel()->getModelDataShared()->getCompositeVertexList().size();
				size_t indexListSize = this->getModel()->getModelDataShared()->getCompositeIndexList().size();

				this->vertexListCopy = new float[vertexListSize * 3];//might be a better way to do this without making a copy
				this->indicesCopy = new unsigned int[indexListSize];//assuming the composite lists are stored in contiguous memory

				for (size_t i = 0; i < vertexListSize; i++)
				{
					this->vertexListCopy[i * 3 + 0] = this->getModel()->getModelDataShared()->getCompositeVertexList().at(i).x;
					this->vertexListCopy[i * 3 + 1] = this->getModel()->getModelDataShared()->getCompositeVertexList().at(i).y;
					this->vertexListCopy[i * 3 + 2] = this->getModel()->getModelDataShared()->getCompositeVertexList().at(i).z;
				}

				for (size_t i = 0; i < indexListSize; i++)
					this->indicesCopy[i] = this->getModel()->getModelDataShared()->getCompositeIndexList().at(i);

				physx::PxTriangleMeshDesc meshDesc;
				meshDesc.points.count = vertexListSize;
				meshDesc.points.stride = sizeof(float) * 3;//tightly packaged
				meshDesc.points.data = this->vertexListCopy;

				meshDesc.triangles.count = indexListSize / 3;
				meshDesc.triangles.stride = 3 * sizeof(unsigned int);//aside about index lists here
				meshDesc.triangles.data = this->indicesCopy;

				physx::PxDefaultMemoryOutputStream writeBuffer;
				physx::PxTriangleMeshCookingResult::Enum result;
				physx::PxCookingParams param = physx::PxCookingParams(physx::PxTolerancesScale());

				bool status = PxCookTriangleMesh(param, meshDesc, writeBuffer, &result);
				if (!status)
				{
					std::cout << "Failed to create Triangular mesh" << std::endl;
					std::cin.get();
				}
				
				physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
				physx::PxTriangleMesh* mesh = p->createTriangleMesh(readBuffer);

				physx::PxMaterial* gMaterial = p->createMaterial(0.5f, 0.5f, 0.6f);
				physx::PxShape* shape = p->createShape(physx::PxTriangleMeshGeometry(mesh), *gMaterial, true);
				physx::PxTransform t({ 0,0,0 });

				a = p->createRigidStatic(t);
				a->attachShape(*shape);

				a->userData = this;
				s->addActor(*a);

			makeItNormal(offset);

			if (texture != "")
			{
				for (size_t i = 0; i < this->WO::getModel()->getModelDataShared()->getModelMeshes().size(); i++)
				{
					this->WO::getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkin().getMultiTextureSet().at(0) = *Aftr::ManagerTex::loadTexAsync(texture);

				}

			}

			isDone = true;

		});
	}

	bool isDone;

	void setPose(const Aftr::Mat4& pose) noexcept override
	{
		WO::setPose(pose);

		physx::PxMat44 m;
		for (int i = 0; i < 16; i++) m(i % 4, i / 4) = pose[i];

		if (a != nullptr)
			a->setGlobalPose(physx::PxTransform(m));

	}

	void makeItNormal(Aftr::VectorD offset)
	{
		float hello[16];
		AftrCoordinateTransforms::getLocalBodySpaceToLTPSpace<float>(offset, hello);

		this->getModel()->setDisplayMatrix(Mat4(hello).transposeUpperLeft3x3());

		this->setPosition(Aftr::Vector(0, 0, -80 - 283));

		//this->rotateAboutRelZ(90 * DEGtoRAD);

		this->setPose(this->getPose());

	}

	float* vertexListCopy = nullptr;
	unsigned int* indicesCopy = nullptr;
	physx::PxRigidActor* a = nullptr;

protected:

	Terrain() : Aftr::IFace(this)
	{
		isDone = false;
	}

};
