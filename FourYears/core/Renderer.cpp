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

    if(blurTarget)
    {

        SDL_DestroyTexture(
            blurTarget
        );

        blurTarget=nullptr;

    }



    if(renderer)
    {

        SDL_DestroyRenderer(
            renderer
        );

        renderer=nullptr;

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
        SDL_RENDERER_PRESENTVSYNC |
        SDL_RENDERER_TARGETTEXTURE

    );



    if(!renderer)
    {

        std::cout
        <<"Renderer创建失败:"
        <<SDL_GetError()
        <<std::endl;


        return false;

    }



    SDL_SetHint(
        SDL_HINT_RENDER_SCALE_QUALITY,
        "best"
    );



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

    SDL_Color white;
    white.r = 255;
    white.g = 255;
    white.b = 255;
    white.a = 255;


    DrawText(
        text,
        x,
        y,
        font,
        white
    );

}









void Renderer::DrawText(

    const std::string& text,

    int x,

    int y,

    TTF_Font* f,

    SDL_Color color

)
{


    if(
        !f ||
        text.empty()
    )
    {

        return;

    }




    SDL_Surface* surface =
    TTF_RenderUTF8_Blended(

        f,

        text.c_str(),

        color

    );




    if(!surface)
    {


        std::cout
        <<"文字渲染失败:"
        <<text
        <<"\n"
        <<TTF_GetError()
        <<std::endl;



        return;

    }








    SDL_Texture* texture =
    SDL_CreateTextureFromSurface(

        renderer,

        surface

    );





    if(!texture)
    {

        SDL_FreeSurface(
            surface
        );


        return;

    }






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







    SDL_DestroyTexture(
        texture
    );

    SDL_FreeSurface(
        surface
    );

}









void Renderer::DrawTexture(

    SDL_Texture* texture,

    int x,

    int y

)
{


    if(
        !texture
    )
    {

        return;

    }






    SDL_Rect dst;



    dst.x=x;

    dst.y=y;







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








void Renderer::DrawTexture(

    SDL_Texture* texture,

    int x,

    int y,

    int w,

    int h

)
{


    if(
        !texture ||
        w <= 0 ||
        h <= 0
    )
    {

        return;

    }




    SDL_Rect dst;

    dst.x = x;

    dst.y = y;

    dst.w = w;

    dst.h = h;



    SDL_RenderCopy(

        renderer,

        texture,

        nullptr,

        &dst

    );

}








// [新增] 填充半透明矩形
void Renderer::DrawFilledRect(

    int x,

    int y,

    int w,

    int h,

    Uint8 r,

    Uint8 g,

    Uint8 b,

    Uint8 a

)
{

    if(
        !renderer ||
        w <= 0 ||
        h <= 0
    )
    {
        return;
    }



    // 必须先打开 alpha 混合，
    // 否则 a < 255 时不会透明，
    // 会直接盖成纯色块。
    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );



    SDL_SetRenderDrawColor(
        renderer,
        r,
        g,
        b,
        a
    );



    SDL_Rect rect;

    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;



    SDL_RenderFillRect(
        renderer,
        &rect
    );

}








void Renderer::DrawBlurTexture(
    SDL_Texture* texture,
    int x,
    int y
)
{

    if(
        !texture ||
        !renderer
    )
    {
        return;
    }



    int w = 0;
    int h = 0;

    SDL_QueryTexture(
        texture,
        nullptr,
        nullptr,
        &w,
        &h
    );



    if(w <= 0 || h <= 0)
    {
        return;
    }




    int sw = w / BLUR_DIVISOR;
    int sh = h / BLUR_DIVISOR;

    if(sw < 1) sw = 1;
    if(sh < 1) sh = 1;




    if(
        !blurTarget ||
        blurW != sw ||
        blurH != sh
    )
    {

        if(blurTarget)
        {

            SDL_DestroyTexture(
                blurTarget
            );

            blurTarget = nullptr;

        }



        blurTarget =
        SDL_CreateTexture(

            renderer,

            SDL_PIXELFORMAT_RGBA8888,

            SDL_TEXTUREACCESS_TARGET,

            sw,

            sh

        );



        if(!blurTarget)
        {

            std::cout
            <<"Blur target 创建失败:"
            <<SDL_GetError()
            <<std::endl;


            return;

        }



        blurW = sw;
        blurH = sh;

    }




    SDL_SetRenderTarget(
        renderer,
        blurTarget
    );



    SDL_SetRenderDrawColor(
        renderer,
        0, 0, 0, 255
    );

    SDL_RenderClear(
        renderer
    );



    SDL_Rect smallDst;
    smallDst.x = 0;
    smallDst.y = 0;
    smallDst.w = sw;
    smallDst.h = sh;

    SDL_RenderCopy(
        renderer,
        texture,
        nullptr,
        &smallDst
    );




    SDL_SetRenderTarget(
        renderer,
        nullptr
    );




    SDL_Rect backDst;
    backDst.x = x;
    backDst.y = y;
    backDst.w = w;
    backDst.h = h;

    SDL_RenderCopy(
        renderer,
        blurTarget,
        nullptr,
        &backDst
    );

}









SDL_Renderer*
Renderer::GetSDLRenderer()
{

    return renderer;

}