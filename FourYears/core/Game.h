#ifndef GAME_H
#define GAME_H


#include <SDL.h>

#include <string>
#include <vector>


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



    void OnActivateCurrentState();


    void OnAdvanceDialogue();


    void OnBack();


    void ConfirmChoice();




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


    // ==========================================================
    // [新增] 配置备份
    // ==========================================================
    //
    // 进入设置页面时保存一份快照。
    // 如果玩家选择"不保存返回"，就用它还原 Config，
    // 避免"未保存的改动下次进设置还在"。

    Config configBackup;

    // ==========================================================



    SDL_Texture* currentBackground=nullptr;

    SDL_Texture* currentCharacter=nullptr;



    
    Story story;



    UIManager ui;



    UIState lastState;



    std::vector<std::string> currentChoiceTargets;


    std::vector<std::vector<AffectionChange>> currentChoiceAffection;



    int lastBgmVolume = -1;
    int lastSeVolume  = -1;
    int lastTextSpeed = -1;
    bool lastFullscreen = false;



    float autoPlayTimer = 0.0f;

    static constexpr float AUTO_PLAY_DELAY = 1.5f;

};



#endif