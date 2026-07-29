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



    void DrawTexture(
        SDL_Texture* texture,
        int x,
        int y
    );


    
    SDL_Renderer* GetSDLRenderer();



    void SetFont(
        TTF_Font* font
    );



private:


    SDL_Renderer* renderer;


    TTF_Font* font;



};



#endif