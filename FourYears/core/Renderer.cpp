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

    // 关键：渲染目标必须用 NONE 混合。
    //
    // 之前这里设了 BLENDMODE_BLEND，结果往它上面 RenderCopy 时
    // 内容会和清屏的黑色混合，整块变黑（背景、对话框模糊全黑）。
    // 需要透明叠加的地方在绘制时临时设 BLEND 即可。
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);
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
    if(roundA)   { SDL_DestroyTexture(roundA);   roundA   = nullptr; }
    if(roundB)   { SDL_DestroyTexture(roundB);   roundB   = nullptr; }

    pingW = pingH = 0;
    pongW = pongH = 0;
    roundAW = roundAH = 0;
    roundBW = roundBH = 0;
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


    // ---- 2. 多级降采样 ----
    //
    // 每张纹理必须用自己的尺寸记录。
    // 之前无论哪张都写 pongW/pongH，导致 blurPing 的记录被污染，
    // EnsureTarget 误判尺寸没变而不重建，最后拿到的是错尺寸的旧纹理，
    // 放大到全屏就成了"只截了一小块"。

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

        // 用各自独立的尺寸记录
        int& nW = usePong ? pongW : pingW;
        int& nH = usePong ? pongH : pingH;

        if(!EnsureTarget(next, nW, nH, nw, nh))
            break;

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
// DrawFrostedRegionRound
// ==========================================================
//
// 圆角矩形区域内的模糊背景。
//
// 之前对话框的做法是「先画矩形模糊区，再叠圆角灰底」，
// 结果四个角露出方形的模糊块，看起来框是方的。
//
// 这里改成按圆角形状裁剪：
//   1. 先把模糊结果画到离屏纹理（和区域同尺寸）
//   2. 再逐行只把圆角内的部分 copy 到屏幕
//
// 逐行裁剪用 SDL_RenderCopy 的 src 矩形实现，
// 不需要 stencil buffer，也不需要 OpenGL。

void Renderer::DrawFrostedRegionRound(
    SDL_Texture* texture,
    int x, int y, int w, int h,
    int radius,
    Uint8 overlayAlpha
)
{
    if(!texture || !renderer || w <= 0 || h <= 0)
        return;

    if(radius < 0) radius = 0;
    if(radius * 2 > w) radius = w / 2;
    if(radius * 2 > h) radius = h / 2;


    // ---- 1. 取源区域（纹理坐标） ----

    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
    if(texW <= 0 || texH <= 0) return;


    int screenW = 0, screenH = 0;
    SDL_RenderGetLogicalSize(renderer, &screenW, &screenH);

    if(screenW <= 0 || screenH <= 0)
        SDL_GetRendererOutputSize(renderer, &screenW, &screenH);

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


    // ==========================================================
    // 用专用缓冲，不要复用 blurPing/blurPong
    // ==========================================================
    //
    // 之前的实现复用 blurPing 做中间结果，最后又把它设成 BLEND
    // 去画到屏幕。结果下次它再当渲染目标时，
    // RenderCopy 会把内容混合到清屏的黑色上，整块变黑——
    // 表现为背景和对话框全黑。
    //
    // 这里用两个专属纹理：
    //   roundA  : 区域尺寸，先放源区域，最后放放大后的模糊结果
    //   roundB  : 降采样中间级
    // roundA 只在"当渲染目标"和"当纹理源"两种身份间切换，
    // 且每次切换前都重置混合模式，不会残留状态。

    if(!EnsureTarget(roundA, roundAW, roundAH, w, h))
        return;

    if(!EnsureTarget(roundB, roundBW, roundBH, (w + 1) / 2, (h + 1) / 2))
        return;


    // ---- 2. 源区域 -> roundA ----

    {
        SDL_SetRenderTarget(renderer, roundA);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect dstFull = { 0, 0, w, h };
        SDL_RenderCopy(renderer, texture, &src, &dstFull);

        SDL_SetRenderTarget(renderer, nullptr);
    }


    // ---- 3. 多级降采样：roundA -> roundB -> roundA -> ... ----
    //
    // 注意：不能在同一张纹理上原地缩放，
    // 所以每级交替，且每次都按目标尺寸重建。

    SDL_Texture* cur = roundA;
    int curW = w, curH = h;

    for(int i = 0; i < BLUR_STEPS; i++)
    {
        int nw = curW / 2;
        int nh = curH / 2;
        if(nw < 2 || nh < 2) break;

        bool toB = (cur == roundA);

        SDL_Texture* next = toB ? roundB : roundA;

        // 各自独立的尺寸记录，不能共用
        int& nW = toB ? roundBW : roundAW;
        int& nH = toB ? roundBH : roundAH;

        if(!EnsureTarget(next, nW, nH, nw, nh))
            break;

        DownsampleInto(cur, curW, curH, next, nw, nh);

        cur = next;
        curW = nw;
        curH = nh;
    }


    // ---- 4. 放大回区域尺寸 ----

    // 找一个和 cur 不同的缓冲放大
    SDL_Texture* big = (cur == roundA) ? roundB : roundA;
    int& bigW = (cur == roundA) ? roundBW : roundAW;
    int& bigH = (cur == roundA) ? roundBH : roundAH;

    if(!EnsureTarget(big, bigW, bigH, w, h))
        return;

    {
        SDL_SetRenderTarget(renderer, big);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect full = { 0, 0, w, h };
        SDL_RenderCopy(renderer, cur, nullptr, &full);

        SDL_SetRenderTarget(renderer, nullptr);
    }


    // ---- 5. 按圆角形状逐行画到屏幕 ----
    //
    // 画之前临时设 BLEND，画完立刻恢复 NONE。
    // 不恢复的话，这张纹理下次当渲染目标会出问题。

    SDL_SetTextureBlendMode(big, SDL_BLENDMODE_BLEND);

    for(int row = 0; row < h; row++)
    {
        int inset = 0;

        if(row < radius)
        {
            double vy = (double)(radius - row);
            inset = radius - (int)sqrt(
                (double)(radius * radius) - vy * vy
            );
        }
        else if(row >= h - radius)
        {
            double vy = (double)(row - (h - radius) + 1);
            if(vy > radius) vy = radius;
            inset = radius - (int)sqrt(
                (double)(radius * radius) - vy * vy
            );
        }

        if(inset < 0) inset = 0;
        if(inset * 2 >= w) continue;

        SDL_Rect s = { inset, row, w - inset * 2, 1 };
        SDL_Rect d = { x + inset, y + row, w - inset * 2, 1 };

        SDL_RenderCopy(renderer, big, &s, &d);
    }

    SDL_SetTextureBlendMode(big, SDL_BLENDMODE_NONE);


    // ---- 6. 白色磨砂叠加（同样按圆角裁剪） ----

    if(overlayAlpha > 0)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, overlayAlpha);

        for(int row = 0; row < h; row++)
        {
            int inset = 0;

            if(row < radius)
            {
                double vy = (double)(radius - row);
                inset = radius - (int)sqrt(
                    (double)(radius * radius) - vy * vy
                );
            }
            else if(row >= h - radius)
            {
                double vy = (double)(row - (h - radius) + 1);
                if(vy > radius) vy = radius;
                inset = radius - (int)sqrt(
                    (double)(radius * radius) - vy * vy
                );
            }

            if(inset < 0) inset = 0;
            if(inset * 2 >= w) continue;

            SDL_Rect d = { x + inset, y + row, w - inset * 2, 1 };
            SDL_RenderFillRect(renderer, &d);
        }
    }
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