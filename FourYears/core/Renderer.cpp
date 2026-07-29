#include <iostream>

#include "Renderer.h"


#include <SDL_ttf.h>



Renderer::Renderer()
{

    renderer=nullptr;

    font=nullptr;

}



Renderer::~Renderer()
{

    if(renderer)
    {

        SDL_DestroyRenderer(renderer);

    }

}




bool Renderer::Init(
    SDL_Window* window
)
{


    renderer =
    SDL_CreateRenderer(

        window,

        -1,

        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC

    );


    if(!renderer)
    {

        std::cout
        <<
        "Renderer创建失败:"
        <<
        SDL_GetError()
        <<
        std::endl;


        return false;

    }


    return true;

}






void Renderer::Clear()
{

    SDL_SetRenderDrawColor(

        renderer,

        0,

        0,

        0,

        255

    );


    SDL_RenderClear(
        renderer
    );

}






void Renderer::Present()
{

    SDL_RenderPresent(
        renderer
    );

}






void Renderer::SetFont(
    TTF_Font* font
)
{

    this->font=font;

}







void Renderer::DrawText(

    const std::string& text,

    int x,

    int y

)
{


    if(!font)
    {

        return;

    }



    SDL_Color color;

    color.r=255;
    color.g=255;
    color.b=255;
    color.a=255;



    SDL_Surface* surface =
    TTF_RenderUTF8_Blended(

        font,

        text.c_str(),

        color

    );



    if(!surface)
    {

        return;

    }



    SDL_Texture* texture =
    SDL_CreateTextureFromSurface(

        renderer,

        surface

    );



    SDL_Rect rect;


    rect.x=x;

    rect.y=y;

    rect.w=surface->w;

    rect.h=surface->h;



    SDL_RenderCopy(

        renderer,

        texture,

        nullptr,

        &rect

    );



    SDL_FreeSurface(surface);


    SDL_DestroyTexture(texture);


}






void Renderer::DrawTexture(
    SDL_Texture* texture,
    int x,
    int y
)
{
    if(!texture)
        return;

    SDL_Rect dst;

    dst.x = x;
    dst.y = y;

    SDL_QueryTexture(
        texture,
        nullptr,
        nullptr,
        &dst.w,
        &dst.h
    );

    SDL_RenderCopy(
        renderer,
        texture,
        nullptr,
        &dst
    );
}






SDL_Renderer* Renderer::GetSDLRenderer()
{

    return renderer;

}