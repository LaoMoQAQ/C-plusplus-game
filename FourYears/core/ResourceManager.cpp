#include "ResourceManager.h"


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






    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(

            renderer,

            surface

        );



    SDL_FreeSurface(surface);






    if(!texture)
    {

        std::cout
        << "Texture Create Failed: "
        << path
        << std::endl;


        return nullptr;

    }






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


    for(auto& pair : textures)
    {


        if(pair.second)
        {

            SDL_DestroyTexture(
                pair.second
            );


        }


    }



    textures.clear();



    IMG_Quit();


}