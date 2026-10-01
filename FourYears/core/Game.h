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



    // ==========================================================
    // [新增] 切换输入模式
    // ==========================================================
    //
    // enabled = true  -> 启用 IME + SDL_TEXTINPUT（输入框用）
    // enabled = false -> 禁用 IME + SDL_TEXTINPUT（快捷键用）
    //
    // 只在状态切换时调用一次，不要每帧调。

    void SetInputMode(bool enabled);

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

    Config configBackup;



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



    // ==========================================================
    // [新增] 输入法句柄
    // ==========================================================
    //
    // 用 void* 存，避免 Game.h 依赖 <windows.h>。
    // 实际使用时在 Game.cpp 里强制转成 HWND / HIMC。
    //
    // winOldHimc 是 Init 时把 IME 从窗口解除时返回的旧句柄，
    // 恢复 IME 时要用它。

    void* winHwnd = nullptr;

    void* winOldHimc = nullptr;

    // 上一帧的输入框状态，用来检测边界
    bool lastRenaming = false;

    // ==========================================================



};



#endif