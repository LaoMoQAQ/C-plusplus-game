#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>


class Character
{

public:


    SDL_Texture* texture;


    std::string id;



    Character()
    {

        texture=nullptr;

        id="";

    }




    bool Load(
        SDL_Renderer* renderer,
        std::string path
    )
    {


        SDL_Surface* surface=
        IMG_Load(
            path.c_str()
        );


        if(!surface)
        {

            return false;

        }



        texture=
        SDL_CreateTextureFromSurface(
            renderer,
            surface
        );



        SDL_FreeSurface(
            surface
        );



        id=path;



        return true;


    }






    void Clear()
    {


        if(texture)
        {

            SDL_DestroyTexture(
                texture
            );


            texture=nullptr;

        }


        id="";


    }







    void Draw(
        SDL_Renderer* renderer,
        int x,
        int y,
        int w,
        int h
    )
    {



        if(!texture)
        {
            return;
        }



        SDL_Rect dst=
        {

            x,

            y,

            w,

            h

        };



        SDL_RenderCopy(
            renderer,
            texture,
            nullptr,
            &dst
        );


    }



};