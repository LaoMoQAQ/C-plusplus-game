#include "PauseMenu.h"



PauseMenu::PauseMenu()
{


    choice = 0;



    items[0]="继续游戏";


    items[1]="保存游戏";


    items[2]="读取存档";


    items[3]="历史记录";


    items[4]="设置";


    items[5]="返回标题";


}








void PauseMenu::Render(

    Renderer& renderer

)
{


    renderer.DrawText(
        "暂停",
        520,
        120
    );



    for(int i=0;i<6;i++)
    {


        std::string text;



        if(i==choice)
        {

            text =
            "> "
            +
            items[i];

        }

        else
        {

            text =
            items[i];

        }



        renderer.DrawText(

            text,

            500,

            230+i*55

        );


    }


}








void PauseMenu::HandleInput(

    int key

)
{


    /*
    
    1 上
    2 下

    */


    if(key==1)
    {


        choice--;



        if(choice<0)
        {

            choice=5;

        }


    }



    else if(key==2)
    {


        choice++;



        if(choice>5)
        {

            choice=0;

        }


    }


}








int PauseMenu::GetChoice() const
{

    return choice;

}








void PauseMenu::Reset()
{

    choice=0;

}