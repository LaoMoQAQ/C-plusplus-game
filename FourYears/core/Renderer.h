#ifndef RENDERER_H
#define RENDERER_H


#include <SDL.h>
#include <SDL_ttf.h>

#include <string>



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

};


#endif