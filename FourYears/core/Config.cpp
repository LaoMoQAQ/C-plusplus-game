#include "Config.h"


#include <fstream>

#include <iostream>





Config::Config()
{

    width = 1280;


    height = 720;


    bgmVolume = 64;


    seVolume = 64;


    textSpeed = 30;


    fullscreen = false;


    autoPlay = false;


}








bool Config::Load(const std::string& path)
{
    std::ifstream file(path);

    if(!file.is_open())
        return false;

    std::string line;

    while(std::getline(file,line))
    {
        if(line.empty())
            continue;

        if(line[0]=='[')
            continue;

        size_t pos=line.find('=');

        if(pos==std::string::npos)
            continue;

        std::string key=line.substr(0,pos);
        std::string value=line.substr(pos+1);

        if(key=="width")
            width=std::stoi(value);

        else if(key=="height")
            height=std::stoi(value);

        else if(key=="bgm")
            bgmVolume=std::stoi(value);

        else if(key=="se")
            seVolume=std::stoi(value);

        else if(key=="textSpeed")
            textSpeed=std::stoi(value);

        else if(key=="fullscreen")
            fullscreen=(value=="1"||value=="true");

        else if(key=="autoPlay")
            autoPlay=(value=="1"||value=="true");
    }

    return true;
}









bool Config::Save(

    const std::string& path

)
{


    std::ofstream file(
        path
    );



    if(!file.is_open())
    {

        return false;

    }




    file
    <<"width "
    <<width
    <<"\n";



    file
    <<"height "
    <<height
    <<"\n";



    file
    <<"bgm "
    <<bgmVolume
    <<"\n";



    file
    <<"se "
    <<seVolume
    <<"\n";



    file
    <<"textSpeed "
    <<textSpeed
    <<"\n";



    file
    <<"fullscreen "
    <<fullscreen
    <<"\n";



    file
    <<"autoPlay "
    <<autoPlay
    <<"\n";



    file.close();



    return true;


}








int Config::GetWidth()
{

    return width;

}



int Config::GetHeight()
{

    return height;

}




int Config::GetBGMVolume()
{

    return bgmVolume;

}



int Config::GetSEVolume()
{

    return seVolume;

}




int Config::GetTextSpeed()
{

    return textSpeed;

}





bool Config::IsFullscreen()
{

    return fullscreen;

}




bool Config::IsAutoPlay()
{

    return autoPlay;

}








void Config::SetWidth(
    int value
)
{

    width=value;

}





void Config::SetHeight(
    int value
)
{

    height=value;

}





void Config::SetBGMVolume(
    int value
)
{

    bgmVolume=value;

}





void Config::SetSEVolume(
    int value
)
{

    seVolume=value;

}





void Config::SetTextSpeed(
    int value
)
{

    textSpeed=value;

}





void Config::SetFullscreen(
    bool value
)
{

    fullscreen=value;

}





void Config::SetAutoPlay(
    bool value
)
{

    autoPlay=value;

}