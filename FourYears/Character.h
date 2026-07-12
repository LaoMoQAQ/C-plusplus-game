#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>


class Character
{

public:

    SDL_Texture* texture;


    Character()
    {
        texture=nullptr;
    }



    bool Load(SDL_Renderer* renderer,std::string path)
    {

        SDL_Surface* surface=
        IMG_Load(path.c_str());


        if(!surface)
        {
            return false;
        }


        texture=
        SDL_CreateTextureFromSurface(
            renderer,
            surface
        );


        SDL_FreeSurface(surface);


        return true;
    }



    void Draw(
        SDL_Renderer* renderer,
        int x,
        int y,
        int w,
        int h
    )
    {

        SDL_Rect dst;

        dst.x=x;
        dst.y=y;
        dst.w=w;
        dst.h=h;


        SDL_RenderCopy(
            renderer,
            texture,
            NULL,
            &dst
        );

    }



};