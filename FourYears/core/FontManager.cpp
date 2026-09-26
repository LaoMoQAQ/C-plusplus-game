#include <iostream>

#include "FontManager.h"



FontManager::FontManager()
{

    font=nullptr;

}



FontManager::~FontManager()
{

    // 关闭默认字体
    if(font)
    {

        TTF_CloseFont(font);

        font=nullptr;

    }


    // [新增] 关闭缓存中的所有字体
    for(auto& pair : cache)
    {

        if(pair.second)
        {

            TTF_CloseFont(pair.second);

        }

    }

    cache.clear();

}



bool FontManager::Load(
    const std::string& path
)
{

    // 记住路径，供 GetFont(size) 使用
    defaultPath = path;


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



// [新增] 用默认路径加载指定字号
TTF_Font* FontManager::GetFont(
    int size
)
{

    return GetFont(
        defaultPath,
        size
    );

}



// [新增] 按路径 + 字号加载，内部缓存
TTF_Font* FontManager::GetFont(
    const std::string& path,
    int size
)
{

    FontKey key;
    key.path = path;
    key.size = size;


    auto it = cache.find(key);

    if(it != cache.end())
    {

        return it->second;

    }


    TTF_Font* f =
    TTF_OpenFont(
        path.c_str(),
        size
    );


    if(!f)
    {

        std::cout
        <<
        "字体加载失败:"
        <<
        path
        <<
        " size="
        <<
        size
        <<
        std::endl;


        std::cout
        <<
        TTF_GetError()
        <<
        std::endl;


        return nullptr;

    }


    cache[key] = f;

    return f;

}