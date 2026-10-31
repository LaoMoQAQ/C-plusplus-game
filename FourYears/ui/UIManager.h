#ifndef UI_MANAGER_H
#define UI_MANAGER_H


#include "DialogueUI.h"
#include "StartMenu.h"
#include "PauseMenu.h"
#include "SaveMenu.h"
#include "ConfigMenu.h"
#include "AffectionMenu.h"

#include "MenuCommon.h"



enum class UIState
{
    START,
    DIALOGUE,
    PAUSE,
    SAVE,
    CONFIG,
    AFFECTION
};



class UIManager
{

public:

    UIManager();

    void SetState(UIState state);

    // 立即切换，不走黑幕过渡。
    //
    // 用于「主菜单 -> 剧情」这种场景：此时背景和立绘已经准备好，
    // 再走一遍「压黑 -> 切换 -> 淡入」会让玩家看到黑一下，
    // 体感像是闪屏。
    void SetStateImmediate(UIState state);

    UIState GetState() const;
    bool IsTransitioning() const;

    void Update(float delta);

    void Render(Renderer& renderer);

    void HandleInput(int key);
    void HandleMouseMove(int x, int y);
    MenuMouseResult HandleMouseClick(int x, int y);
    
    // [新增] 鼠标滚轮分发
    void HandleMouseWheel(int delta);



    DialogueUI&    GetDialogueUI();
    StartMenu&     GetStartMenu();
    PauseMenu&     GetPauseMenu();
    SaveMenu&      GetSaveMenu();
    ConfigMenu&    GetConfigMenu();
    AffectionMenu& GetAffectionMenu();



private:

    UIState currentState;
    UIState pendingState;


    enum class TransState
    {
        NONE,
        OUT,
        IN
    };

    TransState transition = TransState::NONE;

    float alpha = 1.0f;

    // [修改] 0.08 -> 0.18，单程约 0.1 秒
    static constexpr float FADE_SPEED = 0.18f;



    DialogueUI    dialogueUI;
    StartMenu     startMenu;
    PauseMenu     pauseMenu;
    SaveMenu      saveMenu;
    ConfigMenu    configMenu;
    AffectionMenu affectionMenu;

};


#endif