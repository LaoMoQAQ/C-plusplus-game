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


#include "../GameState.h"



class Game
{


public:


    Game();



    ~Game();



    bool Init();



    void Run();



    void Quit();



private:


    void HandleEvents();

    void Update();

    void Render();

    void UpdateScene();



    // 激活当前状态的菜单项。
    // 键盘 Enter 和鼠标点击菜单项都走这里。
    void OnActivateCurrentState();



    // 推进对话。
    // 键盘 Space 和鼠标左键（DIALOGUE 状态）都走这里。
    void OnAdvanceDialogue();



    // [新增] 返回上一级。
    // 键盘 ESC 和鼠标点击"返回"按钮都走这里。
    void OnBack();




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



    SDL_Texture* currentBackground=nullptr;

    SDL_Texture* currentCharacter=nullptr;



    
    Story story;



    UIManager ui;



    UIState lastState;

};



#endif