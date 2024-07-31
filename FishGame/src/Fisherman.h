#pragma once

#include "WO.h"
#include "ModelMeshSkin.h"
#include "Model.h"

namespace Aftr
{
	class Fisherman : public WO
	{
	public:
		static Fisherman* New()
		{
			std::string pole(ManagerEnvironmentConfiguration::getLMM() + "/models/spider2.obj");
		}
	protected:
		int money;
		int inventory;

		WO* pole;
		WO* string;
		WO* spinnyThing;

		std::vector<WO*> fishingParts;

	};
}