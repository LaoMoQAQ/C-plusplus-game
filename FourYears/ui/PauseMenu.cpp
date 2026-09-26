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



    for(int i=0;i<ITEM_COUNT;i++)
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

            MENU_X,

            MENU_Y+i*MENU_GAP

        );

    }




    // 返回按钮（点击 = ESC）
    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );

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

            choice=ITEM_COUNT-1;

        }


    }



    else if(key==2)
    {


        choice++;



        if(choice>=ITEM_COUNT)
        {

            choice=0;

        }


    }

}





void PauseMenu::HandleMouseMove(
    int x,
    int y
)
{

    for(int i=0;i<ITEM_COUNT;i++)
    {

        int ix = MENU_X;
        int iy = MENU_Y + i * MENU_GAP;

        if(
            x >= ix &&
            x <  ix + ITEM_W &&
            y >= iy &&
            y <  iy + ITEM_H
        )
        {
            choice = i;
            return;
        }

    }

}





MenuMouseResult PauseMenu::HandleMouseClick(
    int x,
    int y
)
{

    // 先检查返回按钮
    if(
        x >= UILayout::BACK_X &&
        x <  UILayout::BACK_X + UILayout::BACK_W &&
        y >= UILayout::BACK_Y &&
        y <  UILayout::BACK_Y + UILayout::BACK_H
    )
    {
        return MenuMouseResult::BACK;
    }


    // 再检查菜单项
    for(int i=0;i<ITEM_COUNT;i++)
    {

        int ix = MENU_X;
        int iy = MENU_Y + i * MENU_GAP;

        if(
            x >= ix &&
            x <  ix + ITEM_W &&
            y >= iy &&
            y <  iy + ITEM_H
        )
        {
            choice = i;
            return MenuMouseResult::ACTIVATE;
        }

    }


    return MenuMouseResult::NONE;

}





int PauseMenu::GetChoice() const
{

    return choice;

}





void PauseMenu::Reset()
{

    choice=0;

}