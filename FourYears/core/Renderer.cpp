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
    Shutdown();
}




// ==========================================================
// Shutdown
// ==========================================================
//
// 顺序很重要：先销毁离屏纹理，再销毁 renderer。
// 都要赶在 SDL_Quit() 之前，否则是未定义行为。

void Renderer::Shutdown()
{
    // 文字缓存必须先清，它们也是 renderer 创建出来的纹理
    ClearTextCache();

    ReleaseBlurTargets();

    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    font = nullptr;
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
    if(!f || text.empty() || !renderer)
        return;


    // ---- 查缓存 ----

    TextKey key;
    key.text  = text;
    key.font  = f;
    key.color =
        ((Uint32)color.r << 24) |
        ((Uint32)color.g << 16) |
        ((Uint32)color.b << 8)  |
        ((Uint32)color.a);

    auto it = textCache.find(key);

    if(it == textCache.end())
    {
        // 缓存满了就整体清空。菜单文字高度重复，重建成本可接受。
        if(textCache.size() >= TEXT_CACHE_LIMIT)
            ClearTextCache();

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

        CachedText entry;
        entry.texture = texture;
        entry.w = surface->w;
        entry.h = surface->h;

        SDL_FreeSurface(surface);

        it = textCache.emplace(key, entry).first;
    }


    // ---- 绘制 ----

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = it->second.w;
    rect.h = it->second.h;

    SDL_RenderCopy(renderer, it->second.texture, nullptr, &rect);
}




// ==========================================================
// ClearTextCache
// ==========================================================

void Renderer::ClearTextCache()
{
    for(auto& pair : textCache)
    {
        if(pair.second.texture)
            SDL_DestroyTexture(pair.second.texture);
    }

    textCache.clear();
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





// ==========================================================
// 毛玻璃：多级降采样
// ==========================================================
//
// 老做法是「整张缩到 1/4，再拉回原尺寸」。
// 一次线性采样只平均 4x4 像素，放大后能看出方块感。
//
// 新做法：每级只缩 2 倍，连做 BLUR_STEPS 级。
// 连续多次盒式滤波在数学上趋近高斯模糊，
// 而且每级只做 2x 缩放时双线性采样的误差最小。
//
// 例如 1600x900：
//   1600 -> 800 -> 400 -> 200 -> 100
// 最后一级的 100x56 放大回 1600x900，过渡非常平滑。

bool Renderer::EnsureTarget(
    SDL_Texture*& tex,
    int& texW,
    int& texH,
    int w,
    int h
)
{
    if(w < 1) w = 1;
    if(h < 1) h = 1;

    if(tex && texW == w && texH == h)
        return true;

    if(tex)
    {
        SDL_DestroyTexture(tex);
        tex = nullptr;
    }

    tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h
    );

    if(!tex)
    {
        std::cout
            << "Blur target 创建失败:"
            << SDL_GetError()
            << std::endl;
        texW = texH = 0;
        return false;
    }

    // 放大时用线性插值，缩小同理
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(tex, SDL_ScaleModeLinear);

    texW = w;
    texH = h;
    return true;
}




void Renderer::DownsampleInto(
    SDL_Texture* src, int srcW, int srcH,
    SDL_Texture* dst, int dstW, int dstH
)
{
    SDL_SetRenderTarget(renderer, dst);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect full = { 0, 0, dstW, dstH };
    SDL_RenderCopy(renderer, src, nullptr, &full);

    SDL_SetRenderTarget(renderer, nullptr);
}




void Renderer::ReleaseBlurTargets()
{
    if(blurPing) { SDL_DestroyTexture(blurPing); blurPing = nullptr; }
    if(blurPong) { SDL_DestroyTexture(blurPong); blurPong = nullptr; }

    pingW = pingH = 0;
    pongW = pongH = 0;
}




// ==========================================================
// 把纹理模糊后铺满 (x, y, w, h)
// ==========================================================
//
// srcRect 是源纹理里要模糊的区域（nullptr 表示整张）。
// 结果按 dstW x dstH 绘制。

void Renderer::BlurRegionTo(
    SDL_Texture* texture,
    const SDL_Rect* srcRect,
    int dstX, int dstY,
    int dstW, int dstH,
    Uint8 overlayAlpha
)
{
    if(!texture || !renderer || dstW <= 0 || dstH <= 0)
        return;


    // ---- 1. 把源区域画到 blurPing（原始尺寸） ----

    SDL_Rect s;
    if(srcRect)
    {
        s = *srcRect;
    }
    else
    {
        int tw = 0, th = 0;
        SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);
        if(tw <= 0 || th <= 0) return;
        s = { 0, 0, tw, th };
    }

    if(s.w <= 0 || s.h <= 0)
        return;


    if(!EnsureTarget(blurPing, pingW, pingH, s.w, s.h))
        return;

    SDL_SetRenderTarget(renderer, blurPing);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect dstFull = { 0, 0, s.w, s.h };
    SDL_RenderCopy(renderer, texture, &s, &dstFull);
    SDL_SetRenderTarget(renderer, nullptr);


    // ---- 2. 多级降采样，ping <-> pong 交替 ----
    //
    // 两张离屏纹理各有自己的尺寸记录，不能共用变量，
    // 否则 EnsureTarget 会误判「尺寸没变」而不重建。

    SDL_Texture* cur = blurPing;
    int curW = s.w;
    int curH = s.h;

    for(int i = 0; i < BLUR_STEPS; i++)
    {
        int nw = curW / 2;
        int nh = curH / 2;

        // 太小就停，避免降到 0
        if(nw < 2 || nh < 2)
            break;

        bool usePong = (cur == blurPing);

        SDL_Texture* next = usePong ? blurPong : blurPing;

        if(!EnsureTarget(next, pongW, pongH, nw, nh))
            break;

        // 两个尺寸记录同步（哪张在前不重要，重建后都会对齐）
        if(usePong) { blurPong = next; } else { blurPing = next; }

        DownsampleInto(cur, curW, curH, next, nw, nh);

        cur = next;
        curW = nw;
        curH = nh;
    }


    // ---- 3. 放大画到目标位置 ----

    SDL_Rect out = { dstX, dstY, dstW, dstH };
    SDL_RenderCopy(renderer, cur, nullptr, &out);


    // ---- 4. 叠一层半透明白，做出"磨砂"质感 ----

    if(overlayAlpha > 0)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, overlayAlpha);

        SDL_RenderFillRect(renderer, &out);
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

    // 整屏模糊：不要额外白色蒙版，否则背景会发灰
    BlurRegionTo(texture, nullptr, x, y, w, h, 0);
}




// ==========================================================
// DrawFrostedTexture
// ==========================================================
//
// 毛玻璃：模糊 + 白色磨砂叠加。
//
// 纯模糊只是"看不清"，加上一层半透明白才有玻璃质感
// （类似 iOS / Windows 11 的 acrylic）。

void Renderer::DrawFrostedTexture(
    SDL_Texture* texture,
    int x, int y,
    Uint8 overlayAlpha
)
{
    if(!texture || !renderer) return;

    int w = 0, h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    if(w <= 0 || h <= 0) return;

    BlurRegionTo(texture, nullptr, x, y, w, h, overlayAlpha);
}




// ==========================================================
// DrawBlurredRegion
// ==========================================================
//
// 只模糊纹理的一块区域，画到目标矩形。
// 用于对话框"背景先模糊"。

void Renderer::DrawBlurredRegion(
    SDL_Texture* texture,
    int x, int y, int w, int h
)
{
    if(!texture || !renderer || w <= 0 || h <= 0) return;

    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
    if(texW <= 0 || texH <= 0) return;


    // 源区域要按纹理与屏幕的比例换算。
    // 背景图通常和屏幕同尺寸，但不假设这一点。
    int screenW = 0, screenH = 0;
    SDL_RenderGetLogicalSize(renderer, &screenW, &screenH);

    if(screenW <= 0 || screenH <= 0)
    {
        SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
    }

    if(screenW <= 0 || screenH <= 0)
    {
        screenW = texW;
        screenH = texH;
    }


    SDL_Rect src;
    src.x = (int)((long long)x * texW / screenW);
    src.y = (int)((long long)y * texH / screenH);
    src.w = (int)((long long)w * texW / screenW);
    src.h = (int)((long long)h * texH / screenH);

    if(src.x < 0) src.x = 0;
    if(src.y < 0) src.y = 0;
    if(src.x + src.w > texW) src.w = texW - src.x;
    if(src.y + src.h > texH) src.h = texH - src.y;
    if(src.w <= 0 || src.h <= 0) return;


    BlurRegionTo(texture, &src, x, y, w, h, 0);
}





// ==========================================================
// DrawBlurredRegion
// ==========================================================
//
// 只模糊纹理的一块区域，画到目标矩形。
// 用于对话框"背景先模糊"。
//
// 新的多级降采样实现在上面（BlurRegionTo / DrawBlurredRegion），
// 这里保留旧实现会导致重复定义，已删除。





SDL_Renderer* Renderer::GetSDLRenderer()
{
    return renderer;
}