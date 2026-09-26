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



    void DrawTexture(
        SDL_Texture* texture,
        int x,
        int y
    );



    // 模糊绘制（缩小 -> 放大近似）
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


    // [修改] 从 6 降到 4。
    //
    // 数值越大越糊、像素感越重。
    // 4 是一个比较平衡的值：
    //   - 3：轻微失焦
    //   - 4：明显失焦但保留形状（当前）
    //   - 6：非常糊，容易出方块
    static constexpr int BLUR_DIVISOR = 4;


    SDL_Renderer* renderer;

    TTF_Font* font;



    SDL_Texture* blurTarget = nullptr;

    int blurW = 0;

    int blurH = 0;



};


#endif