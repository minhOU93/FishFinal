#pragma once
#include "WO.h"
#include <cstdlib>
#include "Model.h"
#include "AftrManagers.h"

using namespace Aftr;

class Cat : public WO
{
public:

    Cat() : WO(), IFace(this)
    {

    }

    static Cat* New(Vector scale = Vector(1, 1, 1))
    {
        Cat* cat = new Cat();

        std::string model(ManagerEnvironmentConfiguration::getLMM() + "models/Cat/cat.obj");
        std::string catSkin(ManagerEnvironmentConfiguration::getLMM() + "models/Cat/skin.jpg");

        cat->onCreate(model, scale);

        cat->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;

        cat->upon_async_model_loaded([cat, catSkin]()
            {
                ModelMeshSkin spidey(ManagerTex::loadTexAsync(catSkin).value());
                spidey.setMeshShadingType(MESH_SHADING_TYPE::mstAUTO);
                //spidey.getMultiTextureSet().at(0).setTexRepeats(3.0f);
                cat->getModel()->getSkins().push_back(std::move(spidey));
                cat->getModel()->useNextSkin();
            });

        return cat;
    }

    //WO* frame;

    void initalizeDialogue()
    {
        //redDialog.resize(3);
        //blueDialog.resize(3);
        //carpDialog.resize(3);
        //longDialog.resize(3);

        redDialog.push_back("A red snapper!Now, this is a catch worth celebrating.You've outdone yourself today!This beauty will be the talk of the market.Here's your well-deserved payment!");
        redDialog.push_back("A red snapper, you say?Quite the bold choice!This one's got character, just like you.I'll take it off your hands and turn it into something delightful.");
        redDialog.push_back("Ah, a red snapper!Now this is a fish that knows how to make an entrance.You've got a good eye for quality, and I'm more than happy to reward you for it.Keep bringing me treasures like this!");

        carpDialog.push_back("Ah, a carp!Sturdy and reliable-just like you, my friend.This will make a fine addition to my stock.Let's get you paid and ready for the next haul!");
        carpDialog.push_back("A carp!Now there's a fish with some substance.You've brought me exactly what I needed today.I'll take it with pleasure--here's your coin, well-earned!");
        carpDialog.push_back("Carp, eh?Reminds me of the good old days, when a catch like this was all we needed.You've done well, I'll make sure this one gets the attention it deserves.Thanks for bringing it by!");

        blueDialog.push_back("A bluefish!Now that's a rare catch around here.You've really outdone yourself this time.I'll make sure this one gets the attention it deserves.Here's your payment--well earned!");
        blueDialog.push_back("Well, well, a bluefish!Quite the lively one, isn't it?You've got a knack for finding the unexpected.I'll gladly take it off your hands and give it a good home.");
        blueDialog.push_back("A bluefish--such a vibrant catch!I appreciate you bringing me something a bit different.It'll add a splash of color to my shop.Let's get you paid and ready for your next adventure!");

        longDialog.push_back("A small fish, but every bite counts!Even the little ones have their charm.I'll take it off your hands, and who knows--might be the perfect snack for a picky customer!");
        longDialog.push_back("Well, it may be small, but it's got spirit!Not every day's a big catch, but I'll make the best of it.Here's your payment--every fish matters in my shop!");
        longDialog.push_back("A small fish today?No worries, they all have their place.I'll make sure this one finds a good use.Keep at it--you never know what tomorrow will bring!");
    }
    std::vector<std::string> redDialog;
    std::vector<std::string> blueDialog;
    std::vector<std::string> carpDialog;
    std::vector<std::string> longDialog;
};