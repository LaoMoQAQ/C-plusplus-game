#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H


#include <SDL.h>
#include <SDL_image.h>

#include <string>
#include <unordered_map>



class ResourceManager
{

public:


    ResourceManager();


    ~ResourceManager();



    bool Init();



    SDL_Texture* LoadTexture(
        SDL_Renderer* renderer,
        const std::string& path
    );



    SDL_Texture* GetTexture(
        const std::string& path
    );



    void Destroy();



private:


    std::unordered_map<
        std::string,
        SDL_Texture*
    > textures;



};



#endif