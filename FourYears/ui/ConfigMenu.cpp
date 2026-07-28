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

        return;

    }



    for(int i=0;i<6;i++)
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

            420,

            180+i*55

        );


    }



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