#ifndef RENDERER_H
#define RENDERER_H


#include <SDL.h>

#include <SDL_ttf.h>

#include <string>



class Renderer
{


public:


    Renderer();

    ~Renderer();



    bool Init(
        SDL_Window* window
    );



    void Clear();

    void Present();



    void DrawText(
        const std::string& text,
        int x,
        int y
    );



    void DrawText(
        const std::string& text,
        int x,
        int y,
        TTF_Font* font,
        SDL_Color color
    );



    // 按纹理原始尺寸绘制
    void DrawTexture(
        SDL_Texture* texture,
        int x,
        int y
    );



    // 按指定矩形绘制
    void DrawTexture(
        SDL_Texture* texture,
        int x,
        int y,
        int w,
        int h
    );



    // [新增] 填充半透明矩形。
    // 用于对话框背景这类色块。
    // 需要 SDL_BLENDMODE_BLEND 支持 alpha。
    void DrawFilledRect(
        int x,
        int y,
        int w,
        int h,
        Uint8 r,
        Uint8 g,
        Uint8 b,
        Uint8 a
    );



    void DrawBlurTexture(
        SDL_Texture* texture,
        int x,
        int y
    );



    SDL_Renderer* GetSDLRenderer();



    void SetFont(
        TTF_Font* font
    );



private:


    static constexpr int BLUR_DIVISOR = 4;


    SDL_Renderer* renderer;

    TTF_Font* font;



    SDL_Texture* blurTarget = nullptr;

    int blurW = 0;

    int blurH = 0;



};


#endif