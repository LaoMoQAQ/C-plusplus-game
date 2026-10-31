#ifndef RENDERER_H
#define RENDERER_H


#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <unordered_map>
#include <cstdint>



// ==========================================================
// Renderer
// ==========================================================
//
// 对 SDL_Renderer 的薄封装。

class Renderer
{

public:

    Renderer();
    ~Renderer();

    bool Init(SDL_Window* window);

    // 显式释放 SDL_Renderer 和模糊用的离屏纹理。
    // 必须在 SDL_Quit() 之前调用，并且要在所有纹理销毁之后。
    // 可重复调用。
    void Shutdown();

    void Clear();
    void Present();



    // ---- 文字 ----

    void DrawText(const std::string& text, int x, int y);

    void DrawText(
        const std::string& text,
        int x, int y,
        TTF_Font* font,
        SDL_Color color
    );



    // ---- 纹理 ----

    void DrawTexture(SDL_Texture* texture, int x, int y);
    void DrawTexture(SDL_Texture* texture, int x, int y, int w, int h);



    // ---- 矩形 ----

    void DrawFilledRect(
        int x, int y, int w, int h,
        Uint8 r, Uint8 g, Uint8 b, Uint8 a
    );

    // [新增] 圆角半透明矩形
    void DrawFilledRoundRect(
        int x, int y, int w, int h,
        int radius,
        Uint8 r, Uint8 g, Uint8 b, Uint8 a
    );



    // ---- 模糊 ----

    // 把整张纹理模糊后按原始尺寸画到 (x, y)
    void DrawBlurTexture(SDL_Texture* texture, int x, int y);

    // 毛玻璃：模糊整屏，再叠一层半透明白做出磨砂质感。
    // overlayAlpha 0~255，越大越白越"磨砂"。
    void DrawFrostedTexture(
        SDL_Texture* texture,
        int x, int y,
        Uint8 overlayAlpha
    );

    // 圆角裁剪版毛玻璃区域。
    //
    // 对话框要的是「圆角框里透出模糊背景」，
    // 直接画矩形模糊会露出四个方角。
    // 这里按圆角逐行裁剪，只画圆角内的部分。
    void DrawFrostedRegionRound(
        SDL_Texture* texture,
        int x, int y, int w, int h,
        int radius,
        Uint8 overlayAlpha
    );

    // [新增] 把纹理的一块区域模糊后画到目标矩形
    // 用于对话框这类"背景模糊"效果
    void DrawBlurredRegion(
        SDL_Texture* texture,
        int x, int y, int w, int h
    );



    SDL_Renderer* GetSDLRenderer();

    void SetFont(TTF_Font* font);



private:

    // ==========================================================
    // 毛玻璃
    // ==========================================================
    //
    // 做法：把源纹理逐级降到很小的尺寸，再一次性放大回去。
    //
    // 单纯「缩到 1/4 再放大」的问题是一次线性采样只平均了 4x4 个像素，
    // 边缘还是能看出一格一格的方块感。
    // 多级降采样让每一级只做 2x 缩放，等于连续多次盒式滤波，
    // 结果接近高斯模糊，放大后过渡平滑得多。
    //
    // 级别数由 BLUR_STEPS 控制，每级尺寸减半。
    // 1280x720 的图降 4 级后是 80x45，足够糊，开销也可以忽略。

    static constexpr int BLUR_STEPS = 4;

    SDL_Renderer* renderer = nullptr;
    TTF_Font*     font = nullptr;

    // 中间级缓冲。blurPing 和 blurPong 交替使用，各自记尺寸。
    SDL_Texture* blurPing = nullptr;
    SDL_Texture* blurPong = nullptr;
    int pingW = 0, pingH = 0;
    int pongW = 0, pongH = 0;

    // 圆角区域模糊专用缓冲。
    //
    // 不能复用 blurPing/blurPong：那张纹理最后要被设成 BLEND
    // 画到屏幕，下次再当渲染目标时会把内容混到黑底上，整块变黑。
    // 这里用独立的 roundA/roundB，画完立刻恢复 BLENDMODE_NONE。
    SDL_Texture* roundA = nullptr;
    SDL_Texture* roundB = nullptr;
    int roundAW = 0, roundAH = 0;
    int roundBW = 0, roundBH = 0;

    // 确保离屏纹理是 w*h 大小，返回是否成功
    bool EnsureTarget(SDL_Texture*& tex, int& texW, int& texH, int w, int h);

    // 把 src 降采样到 dst
    void DownsampleInto(
        SDL_Texture* src, int srcW, int srcH,
        SDL_Texture* dst, int dstW, int dstH
    );

    // 模糊一块源区域并画到目标矩形。内部函数。
    void BlurRegionTo(
        SDL_Texture* texture,
        const SDL_Rect* srcRect,
        int dstX, int dstY,
        int dstW, int dstH,
        Uint8 overlayAlpha
    );

    // 释放所有离屏纹理
    void ReleaseBlurTargets();



    // ==========================================================
    // 文字纹理缓存
    // ==========================================================
    //
    // 原来的实现每帧对每行文字都做一次
    //   TTF_RenderUTF8_Blended + SDL_CreateTextureFromSurface + DestroyTexture
    // 菜单页一帧要建几十张纹理，是整个渲染流程里最大的开销。
    //
    // 这里按 (文字, 字体指针, 颜色) 缓存成纹理，命中就直接 RenderCopy。
    // 缓存总量有上限，超过就整体清空重来（菜单文字重复率极高，
    // 简单粗暴的清空策略足够，不需要 LRU）。

    struct TextKey
    {
        std::string text;
        TTF_Font*   font;
        Uint32      color;

        bool operator==(const TextKey& o) const
        {
            return font  == o.font &&
                   color == o.color &&
                   text  == o.text;
        }
    };

    struct TextKeyHash
    {
        size_t operator()(const TextKey& k) const
        {
            size_t h = std::hash<std::string>()(k.text);
            h ^= (size_t)k.font + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= (size_t)k.color + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    struct CachedText
    {
        SDL_Texture* texture = nullptr;
        int w = 0;
        int h = 0;
    };

    std::unordered_map<TextKey, CachedText, TextKeyHash> textCache;

    // 缓存条目上限。超过就全部清空。
    static constexpr size_t TEXT_CACHE_LIMIT = 512;

    void ClearTextCache();

};


#endif