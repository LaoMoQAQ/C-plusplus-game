#ifndef GAME_H
#define GAME_H


#include <SDL.h>


#include "Renderer.h"
#include "ResourceManager.h"
#include "TextSystem.h"
#include "ScriptPlayer.h"
#include "SaveSystem.h"
#include "AudioManager.h"
#include "FontManager.h"
#include "Config.h"


#include "../story/Story.h"


#include "../ui/UIManager.h"



class Game
{


public:


    Game();



    ~Game();



    // 初始化
    bool Init();



    // 游戏循环
    void Run();



    // 退出
    void Quit();




private:


    void HandleEvents();



    void Update();



    void Render();




private:


    bool running;



    SDL_Window* window;



    SDL_Event event;




    Renderer renderer;


    ResourceManager resourceManager;


    TextSystem textSystem;


    ScriptPlayer scriptPlayer;



    SaveSystem saveSystem;



    AudioManager audioManager;



    FontManager fontManager;



    Config config;




    Story story;



    UIManager ui;




};



#endif