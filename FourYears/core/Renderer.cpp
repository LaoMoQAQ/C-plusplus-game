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

    // [新增] 释放模糊缓存纹理
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
        // [新增] 允许把渲染目标切到纹理上，
        // 这是 DrawBlurTexture 的前置条件。
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



    // 高质量缩放
    // 这个 hint 对模糊缩放的插值质量也生效。
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









// 原接口：白色 + 默认字体
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









// 指定字体和颜色
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








// [新增] 模糊绘制
//
// 步骤：
//   1. 查原纹理尺寸。
//   2. 计算缩小后尺寸（原尺寸 / BLUR_DIVISOR）。
//   3. 如果缓存纹理尺寸不匹配，重建。
//   4. 把渲染目标切到 blurTarget。
//   5. 把原纹理缩放到 blurTarget 上。
//   6. 把渲染目标切回主屏。
//   7. 把 blurTarget 放大画到 (x, y)。
//
// 关键点：
//   - SDL_SetRenderTarget 必须成对使用：
//     画完 blurTarget 后一定要切回 nullptr，
//     否则后续所有绘制都会跑到小纹理里。
//   - blurTarget 是缓存，不每帧新建，
//     避免频繁分配 / 释放。
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




    // 缩小后尺寸
    int sw = w / BLUR_DIVISOR;
    int sh = h / BLUR_DIVISOR;

    if(sw < 1) sw = 1;
    if(sh < 1) sh = 1;




    // 缓存纹理尺寸不匹配时重建
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




    // 切到小纹理
    SDL_SetRenderTarget(
        renderer,
        blurTarget
    );



    // 清空（避免上一帧残影）
    SDL_SetRenderDrawColor(
        renderer,
        0, 0, 0, 255
    );

    SDL_RenderClear(
        renderer
    );



    // 缩小画进去
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




    // 切回主屏幕
    SDL_SetRenderTarget(
        renderer,
        nullptr
    );




    // 放大画回原尺寸
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