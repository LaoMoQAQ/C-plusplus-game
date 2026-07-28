#include "SaveMenu.h"



SaveMenu::SaveMenu()
{

    slot = 1;

}








void SaveMenu::Render(

    Renderer& renderer

)
{


    renderer.DrawText(
        "存档",
        500,
        120
    );



    for(int i=1;i<=3;i++)
    {


        std::string text;



        if(i==slot)
        {

            text =
            "> 存档 "
            +
            std::to_string(i);

        }

        else
        {

            text =
            "存档 "
            +
            std::to_string(i);

        }



        renderer.DrawText(
            text,
            500,
            250+(i-1)*60
        );


    }



}








void SaveMenu::HandleInput(

    int key

)
{


    /*
    
    1 上
    2 下

    */


    if(key==1)
    {


        slot--;


        if(slot<1)
        {

            slot=3;

        }


    }



    else if(key==2)
    {


        slot++;


        if(slot>3)
        {

            slot=1;

        }


    }


}








int SaveMenu::GetSlot() const
{

    return slot;

}








bool SaveMenu::Save(

    SaveSystem& system,

    SaveData& data

)
{


    return system.Save(

        slot,

        data

    );


}








bool SaveMenu::Load(

    SaveSystem& system,

    SaveData& data

)
{


    return system.Load(

        slot,

        data

    );


}