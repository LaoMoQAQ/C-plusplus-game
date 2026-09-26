#include "ConfigMenu.h"



ConfigMenu::ConfigMenu()
{

    config = nullptr;


    choice = 0;



    items[0]="BGM音量";


    items[1]="SE音量";


    items[2]="文字速度";


    items[3]="全屏";


    items[4]="自动播放";


    items[5]="保存设置";

}







void ConfigMenu::SetConfig(

    Config* config

)
{

    this->config = config;

}







void ConfigMenu::Render(

    Renderer& renderer

)
{


    renderer.DrawText(
        "设置",
        520,
        80
    );



    if(config==nullptr)
    {

        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );

        return;

    }



    for(int i=0;i<ITEM_COUNT;i++)
    {


        std::string value;



        if(i==0)
        {

            value =
            std::to_string(
                config->GetBGMVolume()
            );

        }

        else if(i==1)
        {

            value =
            std::to_string(
                config->GetSEVolume()
            );

        }

        else if(i==2)
        {

            value =
            std::to_string(
                config->GetTextSpeed()
            );

        }

        else if(i==3)
        {

            value =
            config->IsFullscreen()
            ?
            "开启"
            :
            "关闭";

        }


        else if(i==4)
        {

            value =
            config->IsAutoPlay()
            ?
            "开启"
            :
            "关闭";

        }

        else
        {

            value="";

        }




        std::string text;



        if(i==choice)
        {

            text =
            "> ";

        }



        text += items[i];



        if(!value.empty())
        {

            text +=
            " : "
            +
            value;

        }



        renderer.DrawText(

            text,

            MENU_X,

            MENU_Y+i*MENU_GAP

        );

    }




    renderer.DrawText(
        "← → 调整   Enter 保存",
        420,
        750
    );


    // 返回按钮
    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );

}







void ConfigMenu::HandleInput(

    int key

)
{


    if(config==nullptr)
    {

        return;

    }



    /*

    1 上
    2 下
    3 左
    4 右

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



    else if(key==3)
    {


        switch(choice)
        {


        case 0:

            config->SetBGMVolume(

                config->GetBGMVolume()-5

            );

            break;



        case 1:

            config->SetSEVolume(

                config->GetSEVolume()-5

            );

            break;



        case 2:

            config->SetTextSpeed(

                config->GetTextSpeed()-5

            );

            break;



        case 3:

            config->SetFullscreen(
                false
            );

            break;



        case 4:

            config->SetAutoPlay(
                false
            );

            break;

        }

    }



    else if(key==4)
    {


        switch(choice)
        {


        case 0:

            config->SetBGMVolume(

                config->GetBGMVolume()+5

            );

            break;



        case 1:

            config->SetSEVolume(

                config->GetSEVolume()+5

            );

            break;



        case 2:

            config->SetTextSpeed(

                config->GetTextSpeed()+5

            );

            break;



        case 3:

            config->SetFullscreen(
                true
            );

            break;



        case 4:

            config->SetAutoPlay(
                true
            );

            break;

        }

    }



}







void ConfigMenu::Save()
{

    if(config)
    {

        config->Save(
            "config.ini"
        );

    }

}







int ConfigMenu::GetChoice() const
{

    return choice;

}





void ConfigMenu::HandleMouseMove(
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





MenuMouseResult ConfigMenu::HandleMouseClick(
    int x,
    int y
)
{

    // 返回按钮
    if(
        x >= UILayout::BACK_X &&
        x <  UILayout::BACK_X + UILayout::BACK_W &&
        y >= UILayout::BACK_Y &&
        y <  UILayout::BACK_Y + UILayout::BACK_H
    )
    {
        return MenuMouseResult::BACK;
    }


    // 菜单项
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

            // 点"保存设置"直接保存
            if(i == 5)
            {
                Save();
            }

            return MenuMouseResult::ACTIVATE;
        }

    }


    return MenuMouseResult::NONE;

}