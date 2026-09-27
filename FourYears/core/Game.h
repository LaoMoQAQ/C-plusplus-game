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



    // ==========================================================
    // [新增] 确认选择
    // ==========================================================
    //
    // 从 ui.GetDialogueUI() 取出选中的 index，
    // 查 currentChoiceTargets 得到目标脚本，
    // 加载并切到新章节。

    void ConfirmChoice();

    // ==========================================================




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



    // ==========================================================
    // [新增] 当前选择事件的跳转目标列表
    // ==========================================================
    //
    // 与 DialogueUI 里的选项列表一一对应。
    // 在 OnAdvanceDialogue 进入选择事件时同步。
    // 确认选择后清空。

    std::vector<std::string> currentChoiceTargets;

    // ==========================================================

};



#endif