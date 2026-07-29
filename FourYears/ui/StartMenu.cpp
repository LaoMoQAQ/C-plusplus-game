#include "StartMenu.h"



StartMenu::StartMenu()
{

    choice=0;


    items[0]="开始游戏";

    items[1]="存档管理";

    items[2]="读取存档";

    items[3]="设置";

    items[4]="退出游戏";


}







void StartMenu::Render(
    Renderer& renderer
)
{


    // 左上角游戏标题

    renderer.DrawText(
        "Four Years",
        80,
        60
    );



    // 左下角菜单

    for(int i=0;i<5;i++)
    {

        std::string text;


        if(i==choice)
        {
            text="> "+items[i];
        }
        else
        {
            text=items[i];
        }



        renderer.DrawText(
            text,
            100,
            600+i*55
        );


    }


}








void StartMenu::HandleInput(
    int key
)
{

    // 上

    if(key==1)
    {

        choice--;


        if(choice<0)
        {
            choice=4;
        }

    }



    // 下

    else if(key==2)
    {

        choice++;


        if(choice>4)
        {
            choice=0;
        }

    }


}








int StartMenu::GetChoice() const
{

    return choice;

}








void StartMenu::Reset()
{

    choice=0;

}