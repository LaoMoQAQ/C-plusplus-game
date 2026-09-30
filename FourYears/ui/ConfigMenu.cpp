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

}







void ConfigMenu::SetConfig(

    Config* config

)
{

    this->config = config;

    dirty = false;

    confirmSave = false;

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
            )
            + " 字/秒";

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




    // ==========================================================
    // 底部提示
    // ==========================================================
    //
    // [修改] 去掉 ESC 部分，避免和右下角"返回 [ESC]"重复。

    if(confirmSave)
    {

        renderer.DrawText(
            "设置已修改，是否保存？",
            420,
            730
        );

        renderer.DrawText(
            "Y 保存并返回  /  N 不保存返回  /  ESC 取消",
            420,
            780
        );

    }
    else
    {

        renderer.DrawText(
            "← → 调整   S 保存",
            540,
            750
        );

    }


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


    if(confirmSave)
    {
        return;
    }



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
        {

            int old = config->GetBGMVolume();
            int v = old - 5;
            if(v < 0) v = 0;

            if(v != old)
            {
                config->SetBGMVolume(v);
                dirty = true;
            }

        }
        break;



        case 1:
        {

            int old = config->GetSEVolume();
            int v = old - 5;
            if(v < 0) v = 0;

            if(v != old)
            {
                config->SetSEVolume(v);
                dirty = true;
            }

        }
        break;



        case 2:
        {

            int old = config->GetTextSpeed();
            int v = old - 5;
            if(v < 5) v = 5;

            if(v != old)
            {
                config->SetTextSpeed(v);
                dirty = true;
            }

        }
        break;



        case 3:
        {

            if(config->IsFullscreen())
            {
                config->SetFullscreen(false);
                dirty = true;
            }

        }
        break;



        case 4:
        {

            if(config->IsAutoPlay())
            {
                config->SetAutoPlay(false);
                dirty = true;
            }

        }
        break;

        }

    }



    else if(key==4)
    {


        switch(choice)
        {


        case 0:
        {

            int old = config->GetBGMVolume();
            int v = old + 5;
            if(v > 100) v = 100;

            if(v != old)
            {
                config->SetBGMVolume(v);
                dirty = true;
            }

        }
        break;



        case 1:
        {

            int old = config->GetSEVolume();
            int v = old + 5;
            if(v > 100) v = 100;

            if(v != old)
            {
                config->SetSEVolume(v);
                dirty = true;
            }

        }
        break;



        case 2:
        {

            int old = config->GetTextSpeed();
            int v = old + 5;
            if(v > 100) v = 100;

            if(v != old)
            {
                config->SetTextSpeed(v);
                dirty = true;
            }

        }
        break;



        case 3:
        {

            if(!config->IsFullscreen())
            {
                config->SetFullscreen(true);
                dirty = true;
            }

        }
        break;



        case 4:
        {

            if(!config->IsAutoPlay())
            {
                config->SetAutoPlay(true);
                dirty = true;
            }

        }
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

        dirty = false;

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

    if(confirmSave)
    {
        return;
    }


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

    if(
        x >= UILayout::BACK_X &&
        x <  UILayout::BACK_X + UILayout::BACK_W &&
        y >= UILayout::BACK_Y &&
        y <  UILayout::BACK_Y + UILayout::BACK_H
    )
    {
        return MenuMouseResult::BACK;
    }


    if(confirmSave)
    {
        return MenuMouseResult::NONE;
    }


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





bool ConfigMenu::TryExit()
{

    if(!dirty)
    {
        return true;
    }


    confirmSave = true;

    return false;

}





void ConfigMenu::CancelConfirm()
{

    confirmSave = false;

}





bool ConfigMenu::IsConfirming() const
{

    return confirmSave;

}





// ==========================================================
// [修改] 返回值改为 int
// ==========================================================

int ConfigMenu::HandleConfirmKey(
    int sym
)
{

    if(!confirmSave)
    {
        return -1;
    }


    // Y 保存并返回
    if(sym == SDLK_y)
    {

        Save();

        confirmSave = false;

        return 1;

    }


    // N 不保存返回
    if(sym == SDLK_n)
    {

        confirmSave = false;

        return 0;

    }


    // ESC 取消
    if(sym == SDLK_ESCAPE)
    {

        confirmSave = false;

        return -1;

    }


    return -1;

}