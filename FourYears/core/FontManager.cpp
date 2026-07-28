#include <iostream>

#include "FontManager.h"


FontManager::FontManager()
{

    font=nullptr;

}



FontManager::~FontManager()
{

    if(font)
    {

        TTF_CloseFont(font);

        font=nullptr;

    }

}



bool FontManager::Load(
    const std::string& path
)
{


    font =
    TTF_OpenFont(
        path.c_str(),
        32
    );


    if(!font)
    {

        std::cout
        <<
        "字体加载失败:"
        <<
        path
        <<
        std::endl;


        std::cout
        <<
        TTF_GetError()
        <<
        std::endl;


        return false;

    }


    std::cout
    <<
    "字体加载成功"
    <<
    std::endl;


    return true;

}



TTF_Font* FontManager::GetFont()
{

    return font;

}