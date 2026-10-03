#include <iostream>
#include <cmath>

#include "Renderer.h"

#include <SDL_ttf.h>




Renderer::Renderer()
{
    renderer = nullptr;
    font = nullptr;
}



Renderer::~Renderer()
{
    if(blurTarget)
    {
        SDL_DestroyTexture(blurTarget);
        blurTarget = nullptr;
    }

    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
}





bool Renderer::Init(SDL_Window* window)
{
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC |
        SDL_RENDERER_TARGETTEXTURE
    );

    if(!renderer)
    {
        std::cout
            << "Renderer创建失败:"
            << SDL_GetError()
            << std::endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

    return true;
}





void Renderer::Clear()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}



void Renderer::Present()
{
    SDL_RenderPresent(renderer);
}



void Renderer::SetFont(TTF_Font* font)
{
    this->font = font;
}





void Renderer::DrawText(
    const std::string& text,
    int x, int y
)
{
    SDL_Color white = { 255, 255, 255, 255 };
    DrawText(text, x, y, font, white);
}



void Renderer::DrawText(
    const std::string& text,
    int x, int y,
    TTF_Font* f,
    SDL_Color color
)
{
    if(!f || text.empty())
        return;


    SDL_Surface* surface =
        TTF_RenderUTF8_Blended(f, text.c_str(), color);

    if(!surface)
    {
        std::cout
            << "文字渲染失败:" << text
            << "\n" << TTF_GetError()
            << std::endl;
        return;
    }


    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(renderer, surface);

    if(!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }


    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}





void Renderer::DrawTexture(
    SDL_Texture* texture,
    int x, int y
)
{
    if(!texture) return;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    SDL_QueryTexture(texture, nullptr, nullptr, &dst.w, &dst.h);

    SDL_RenderCopy(renderer, texture, nullptr, &dst);
}



void Renderer::DrawTexture(
    SDL_Texture* texture,
    int x, int y, int w, int h
)
{
    if(!texture || w <= 0 || h <= 0) return;

    SDL_Rect dst = { x, y, w, h };

    SDL_RenderCopy(renderer, texture, nullptr, &dst);
}





void Renderer::DrawFilledRect(
    int x, int y, int w, int h,
    Uint8 r, Uint8 g, Uint8 b, Uint8 a
)
{
    if(!renderer || w <= 0 || h <= 0) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    SDL_Rect rect = { x, y, w, h };

    SDL_RenderFillRect(renderer, &rect);
}





// ==========================================================
// DrawFilledRoundRect
// ==========================================================
//
// 圆角矩形。SDL2 没有原生 API，手工画：
//   中间大矩形 + 上下两条窄矩形 + 4 个角（逐行像素）

void Renderer::DrawFilledRoundRect(
    int x, int y, int w, int h,
    int radius,
    Uint8 r, Uint8 g, Uint8 b, Uint8 a
)
{
    if(!renderer || w <= 0 || h <= 0) return;

    if(radius < 0) radius = 0;
    if(radius * 2 > w) radius = w / 2;
    if(radius * 2 > h) radius = h / 2;


    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);


    // ---- 中间大矩形 ----

    SDL_Rect center = { x, y + radius, w, h - 2 * radius };
    SDL_RenderFillRect(renderer, &center);


    // ---- 上下两条窄矩形（不含圆角部分） ----

    SDL_Rect topBar = { x + radius, y, w - 2 * radius, radius };
    SDL_Rect botBar = { x + radius, y + h - radius, w - 2 * radius, radius };
    SDL_RenderFillRect(renderer, &topBar);
    SDL_RenderFillRect(renderer, &botBar);


    // ---- 4 个圆角，逐行画 ----

    for(int dy = 0; dy < radius; dy++)
    {
        // 到圆心的垂直距离
        double vy = (double)(radius - dy);

        // 该行的水平半宽
        int dx = (int)sqrt(
            (double)(radius * radius) - vy * vy
        );


        // 左上
        SDL_Rect q = { x + radius - dx, y + dy, dx, 1 };
        SDL_RenderFillRect(renderer, &q);

        // 右上
        q.x = x + w - radius;
        SDL_RenderFillRect(renderer, &q);

        // 左下
        q.x = x + radius - dx;
        q.y = y + h - 1 - dy;
        SDL_RenderFillRect(renderer, &q);

        // 右下
        q.x = x + w - radius;
        SDL_RenderFillRect(renderer, &q);
    }
}





void Renderer::DrawBlurTexture(
    SDL_Texture* texture,
    int x, int y
)
{
    if(!texture || !renderer) return;

    int w = 0, h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    if(w <= 0 || h <= 0) return;


    int sw = w / BLUR_DIVISOR;
    int sh = h / BLUR_DIVISOR;
    if(sw < 1) sw = 1;
    if(sh < 1) sh = 1;


    if(!blurTarget || blurW != sw || blurH != sh)
    {
        if(blurTarget) SDL_DestroyTexture(blurTarget);

        blurTarget = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            sw, sh
        );

        if(!blurTarget)
        {
            std::cout
                << "Blur target 创建失败:"
                << SDL_GetError()
                << std::endl;
            return;
        }

        blurW = sw;
        blurH = sh;
    }


    SDL_SetRenderTarget(renderer, blurTarget);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect smallDst = { 0, 0, sw, sh };
    SDL_RenderCopy(renderer, texture, nullptr, &smallDst);

    SDL_SetRenderTarget(renderer, nullptr);


    SDL_Rect backDst = { x, y, w, h };
    SDL_RenderCopy(renderer, blurTarget, nullptr, &backDst);
}





// ==========================================================
// DrawBlurredRegion
// ==========================================================
//
// 只模糊纹理的一块区域，画到目标矩形。
// 用于对话框"背景先模糊"。
//
// 实现：
//   1. 整张纹理缩到 blurTarget（快）
//   2. 从 blurTarget 取对应区域，放大画到 (x, y, w, h)

void Renderer::DrawBlurredRegion(
    SDL_Texture* texture,
    int x, int y, int w, int h
)
{
    if(!texture || !renderer || w <= 0 || h <= 0) return;


    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
    if(texW <= 0 || texH <= 0) return;


    int sw = texW / BLUR_DIVISOR;
    int sh = texH / BLUR_DIVISOR;
    if(sw < 1) sw = 1;
    if(sh < 1) sh = 1;


    if(!blurTarget || blurW != sw || blurH != sh)
    {
        if(blurTarget) SDL_DestroyTexture(blurTarget);

        blurTarget = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            sw, sh
        );

        if(!blurTarget) return;

        blurW = sw;
        blurH = sh;
    }


    // 整张缩小
    SDL_SetRenderTarget(renderer, blurTarget);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect fullDst = { 0, 0, sw, sh };
    SDL_RenderCopy(renderer, texture, nullptr, &fullDst);

    SDL_SetRenderTarget(renderer, nullptr);


    // 取对应区域
    SDL_Rect srcRect;
    srcRect.x = x / BLUR_DIVISOR;
    srcRect.y = y / BLUR_DIVISOR;
    srcRect.w = w / BLUR_DIVISOR;
    srcRect.h = h / BLUR_DIVISOR;

    if(srcRect.x < 0) srcRect.x = 0;
    if(srcRect.y < 0) srcRect.y = 0;
    if(srcRect.x + srcRect.w > sw) srcRect.w = sw - srcRect.x;
    if(srcRect.y + srcRect.h > sh) srcRect.h = sh - srcRect.y;
    if(srcRect.w <= 0 || srcRect.h <= 0) return;


    SDL_Rect dstRect = { x, y, w, h };
    SDL_RenderCopy(renderer, blurTarget, &srcRect, &dstRect);
}





SDL_Renderer* Renderer::GetSDLRenderer()
{
    return renderer;
}