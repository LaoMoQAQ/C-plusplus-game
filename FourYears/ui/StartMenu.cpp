#include "StartMenu.h"



StartMenu::StartMenu()
{


    choice = 0;



    items[0]="开始游戏";


    items[1]="读取存档";


    items[2]="设置";


    items[3]="退出游戏";


}








void StartMenu::Render(
    Renderer& renderer
)
{
    // 游戏标题（左上）
    renderer.DrawText(
        "Four Years",
        80,
        60
    );

    // 左下菜单
    for(int i=0;i<4;i++)
    {
        std::string text;

        if(i==choice)
            text="> "+items[i];
        else
            text=items[i];

        renderer.DrawText(
            text,
            90,
            650+i*55
        );
    }
}








void StartMenu::HandleInput(

    int key

)
{


    /*
    
    key:

    1 上
    2 下
    3 确认

    */


    if(key==1)
    {


        choice--;


        if(choice<0)
        {

            choice=3;

        }


    }



    else if(key==2)
    {


        choice++;


        if(choice>3)
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