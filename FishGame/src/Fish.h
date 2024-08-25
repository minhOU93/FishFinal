#pragma once
#include "WO.h"
#include <cstdlib>
#include "Model.h"

using namespace Aftr;

class Fish : public WO
{
public:

    Fish() : WO(), IFace(this)
    {
        //price = 0;
        //name = "";
    }

    static Fish* New(std::string model, std::string skin, Vector scale = Vector(1, 1, 1))
    {
        Fish* fish = new Fish();

        fish->onCreate(model, scale);

        fish->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;

        fish->upon_async_model_loaded([fish, skin]()
            {
                ModelMeshSkin spidey(ManagerTex::loadTexAsync(skin).value());
                spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
                //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
                fish->getModel()->getSkins().push_back(std::move(spidey));
                fish->getModel()->useNextSkin();
            });

        return fish;
    }

    std::string getName() { return name; }
    int getPrice() { return price; }

    std::pair<int, int> struggleRange;
    int price;
    std::string name;
    float difficulty;

};