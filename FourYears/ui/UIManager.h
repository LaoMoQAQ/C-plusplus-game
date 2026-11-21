#ifndef UI_MANAGER_H
#define UI_MANAGER_H


#include "DialogueUI.h"
#include "StartMenu.h"
#include "PauseMenu.h"
#include "SaveMenu.h"
#include "ConfigMenu.h"
#include "AffectionMenu.h"
#include "ChapterTitle.h"
#include "ChapterMenu.h"

#include "MenuCommon.h"



enum class UIState
{
    START,
    DIALOGUE,
    PAUSE,
    SAVE,
    CONFIG,
    AFFECTION,
    CHAPTER
};



// ==========================================================
// UIManager
// ==========================================================
//
// 状态机分发。
//
// 设计说明（重要）：
//   早期版本每次切状态都做「全屏压黑 -> 切换 -> 淡入」，
//   alpha 从 1 降到 0 再升回 1。问题有两个：
//     1. 中间必然有一帧纯黑，玩家看到的就是闪屏
//     2. galgame 的常规做法不是全屏黑幕，菜单直接出现即可
//
//   现在改为：状态立即切换，不做全屏黑幕。
//   菜单自身的过渡感由各菜单内部的动画提供
//   （高亮块滑动、对话框高度缓动等）。

class UIManager
{

public:

    UIManager();

    // 切换状态。立即生效，不做全屏黑幕。
    void SetState(UIState state);

    // 保留这个名字兼容旧调用，行为和 SetState 一样。
    void SetStateImmediate(UIState state);

    UIState GetState() const;

    // 过渡已移除，恒为 false。保留接口供 Game 判断。
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
    ChapterTitle&  GetChapterTitle();
    ChapterMenu&   GetChapterMenu();



private:

    UIState currentState;


    DialogueUI    dialogueUI;
    StartMenu     startMenu;
    PauseMenu     pauseMenu;
    SaveMenu      saveMenu;
    ConfigMenu    configMenu;
    AffectionMenu affectionMenu;
    ChapterTitle  chapterTitle;
    ChapterMenu   chapterMenu;

};


#endif
