#ifndef GAME_H
#define GAME_H


#include <SDL.h>

#include <string>
#include <vector>


#include "Renderer.h"
#include "ResourceManager.h"
#include "TextSystem.h"
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

    bool Init();
    void Run();
    void Quit();



private:

    // ---- 主循环 ----

    void HandleEvents();
    void Update();
    void Render();
    void UpdateScene();



    // ---- 事件分发 ----

    void HandleTextInput();
    void HandleKeyDown();
    void HandleMouseEvent();



    // ---- 输入处理 ----

    void OnActivateCurrentState();
    void OnAdvanceDialogue();
    void OnBack();
    void ConfirmChoice();



    // ---- 输入法 ----

    void SetInputMode(bool enabled);



    // ---- 内部辅助 ----

    void EnterSavePage(SavePage page);
    bool IsRenaming();

    // [新增] 设置某个位置的立绘目标
    void SetSprite(int slot, const std::string& path);



private:

    // ==========================================================
    // [新增] 活动立绘
    // ==========================================================
    //
    // 三个位置（左/中/右）各自维护纹理和透明度。
    // 换立绘时 alpha 从 0 开始淡入到 1，实现淡入效果。

    struct ActiveSprite
    {
        SDL_Texture* texture = nullptr;
        float alpha = 0.0f;         // 当前透明度 0~1
        float targetAlpha = 0.0f;   // 目标透明度
    };

    // 0=左 1=中 2=右
    ActiveSprite sprites[3];

    // 每帧 alpha 靠近 targetAlpha 的比例
    static constexpr float SPRITE_FADE_SPEED = 0.12f;

    // ==========================================================



    // ---- 核心 ----

    bool running;
    SDL_Window* window;
    SDL_Event event;



    // ---- 子系统 ----

    Renderer        renderer;
    ResourceManager resourceManager;
    TextSystem      textSystem;
    SaveSystem      saveSystem;
    AudioManager    audioManager;
    FontManager     fontManager;
    Config          config;
    Config          configBackup;



    // ---- 场景状态 ----

    SDL_Texture* currentBackground = nullptr;



    // ---- 剧情 / UI ----

    Story      story;
    UIManager  ui;
    UIState    lastState;



    // ---- 选择事件 ----

    std::vector<std::string> currentChoiceTargets;
    std::vector<std::vector<AffectionChange>> currentChoiceAffection;



    // ---- 音频 ----

    std::string lastBgmPath;



    // ---- 配置同步缓存 ----

    int  lastBgmVolume  = -1;
    int  lastSeVolume   = -1;
    int  lastTextSpeed  = -1;
    bool lastFullscreen = false;



    // ---- 自动播放 ----

    float autoPlayTimer = 0.0f;
    static constexpr float AUTO_PLAY_DELAY = 1.5f;



    // ---- 输入法 ----

    void* winHwnd    = nullptr;
    void* winOldHimc = nullptr;

    bool lastRenaming = false;

};


#endif