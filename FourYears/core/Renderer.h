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

    // [新增] 把纹理的一块区域模糊后画到目标矩形
    // 用于对话框这类"背景模糊"效果
    void DrawBlurredRegion(
        SDL_Texture* texture,
        int x, int y, int w, int h
    );



    SDL_Renderer* GetSDLRenderer();

    void SetFont(TTF_Font* font);



private:

    static constexpr int BLUR_DIVISOR = 4;


    SDL_Renderer* renderer;
    TTF_Font*     font;

    SDL_Texture* blurTarget = nullptr;
    int blurW = 0;
    int blurH = 0;



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