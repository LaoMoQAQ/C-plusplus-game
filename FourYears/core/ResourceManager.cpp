#include "ResourceManager.h"

#include "Renderer.h"


#include <iostream>



ResourceManager::ResourceManager()
{

}





ResourceManager::~ResourceManager()
{

    Destroy();

}







bool ResourceManager::Init()
{


    int flags = IMG_INIT_PNG;



    if(!(IMG_Init(flags) & flags))
    {

        std::cout
        << "SDL_image Init Failed: "
        << IMG_GetError()
        << std::endl;


        return false;

    }


    imageInited = true;

    return true;


}







SDL_Texture* ResourceManager::LoadTexture(

    SDL_Renderer* renderer,

    const std::string& path

)
{


    auto it = textures.find(path);



    if(it != textures.end())
    {

        return it->second;

    }






    SDL_Surface* surface =
        IMG_Load(path.c_str());



    if(!surface)
    {

        std::cout
        << "Image Load Failed: "
        << path
        << std::endl;

        std::cout
        << "IMG Error: "
        << IMG_GetError()
        << std::endl;

        return nullptr;

    }






    SDL_Texture* texture = nullptr;

    // GL 路径：交给 Renderer 上传，拿到假句柄。
    // 旧的 renderer 参数已废弃，只在 targetRenderer 没绑定时做兜底。
    if(targetRenderer)
    {
        texture = targetRenderer->CreateTextureFromSurface(surface);
    }
    else if(renderer)
    {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
    }

    SDL_FreeSurface(surface);






    if(!texture)
    {

        std::cout
        << "Texture Create Failed: "
        << path
        << std::endl;


        return nullptr;

    }






    // GL 路径下 texture 是假句柄，不能传给任何 SDL_* 函数。
    // 混合模式在 Renderer 里已经固定为 SRC_ALPHA / ONE_MINUS_SRC_ALPHA，
    // 这里什么都不用做。
    if(!targetRenderer)
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    textures[path] = texture;

    return texture;


}







SDL_Texture* ResourceManager::GetTexture(

    const std::string& path

)
{


    auto it = textures.find(path);



    if(it == textures.end())
    {

        return nullptr;

    }



    return it->second;


}







void ResourceManager::Destroy()
{
    if(!imageInited)
        return;

    for(auto& pair : textures)
    {
        if(pair.second)
        {
            if(targetRenderer)
                targetRenderer->DestroyTexture(pair.second);
            else
                SDL_DestroyTexture(pair.second);
        }
    }

    textures.clear();

    IMG_Quit();
    imageInited = false;
}