#include "UIManager.h"



UIManager::UIManager()
{
    currentState = UIState::START;
}




// ==========================================================
// SetState
// ==========================================================
//
// 立即切状态，不做全屏黑幕。
//
// 早期版本走「压黑 -> 切换 -> 淡入」：
//   alpha 1 -> 0 -> 1，中间必然出现一帧纯黑，
//   玩家看到的就是闪屏，而且在 galgame 里很出戏。
//
// 现在直接切。菜单切换的观感由各菜单自己的动画承担
// （高亮块滑动、对话框高度缓动等）。

void UIManager::SetState(UIState state)
{
    currentState = state;
}




void UIManager::SetStateImmediate(UIState state)
{
    currentState = state;
}




UIState UIManager::GetState() const
{
    return currentState;
}




bool UIManager::IsTransitioning() const
{
    return false;
}




// ==========================================================
// Update
// ==========================================================
//
// 推进各菜单内部动画（高亮块滑动、对话框高度缓动等）。

void UIManager::Update(float delta)
{
    chapterTitle.Update(delta);

    startMenu.Update(delta);
    pauseMenu.Update(delta);
    configMenu.Update(delta);
    saveMenu.Update(delta);
}




void UIManager::Render(Renderer& renderer)
{
    switch(currentState)
    {
    case UIState::START:
        startMenu.Render(renderer);
        break;

    case UIState::DIALOGUE:
        dialogueUI.Render(renderer);
        break;

    case UIState::PAUSE:
        pauseMenu.Render(renderer);
        break;

    case UIState::SAVE:
        saveMenu.Render(renderer);
        break;

    case UIState::CONFIG:
        configMenu.Render(renderer);
        break;

    case UIState::AFFECTION:
        affectionMenu.Render(renderer);
        break;
    }


    // 章节标题卡叠在所有界面之上
    if(chapterTitle.IsActive())
        chapterTitle.Render(renderer);
}




void UIManager::HandleInput(int key)
{
    switch(currentState)
    {
    case UIState::START:
        startMenu.HandleInput(key);
        break;

    case UIState::PAUSE:
        pauseMenu.HandleInput(key);
        break;

    case UIState::SAVE:
        saveMenu.HandleInput(key);
        break;

    case UIState::CONFIG:
        configMenu.HandleInput(key);
        break;

    // [新增] 好感度页面：↑↓ 翻页
    case UIState::AFFECTION:
        affectionMenu.HandleInput(key);
        break;

    default:
        break;
    }
}




void UIManager::HandleMouseMove(int x, int y)
{
    switch(currentState)
    {
    case UIState::START:  startMenu.HandleMouseMove(x, y);  break;
    case UIState::PAUSE:  pauseMenu.HandleMouseMove(x, y);  break;
    case UIState::SAVE:   saveMenu.HandleMouseMove(x, y);   break;
    case UIState::CONFIG: configMenu.HandleMouseMove(x, y); break;
    default: break;
    }
}




MenuMouseResult UIManager::HandleMouseClick(int x, int y)
{
    switch(currentState)
    {
    case UIState::START:
        return startMenu.HandleMouseClick(x, y);

    case UIState::PAUSE:
        return pauseMenu.HandleMouseClick(x, y);

    case UIState::SAVE:
        return saveMenu.HandleMouseClick(x, y);

    case UIState::CONFIG:
        return configMenu.HandleMouseClick(x, y);

    case UIState::AFFECTION:
        return affectionMenu.HandleMouseClick(x, y);

    default:
        return MenuMouseResult::NONE;
    }
}




void UIManager::HandleMouseWheel(int delta)
{
    switch(currentState)
    {
    case UIState::AFFECTION:
        affectionMenu.HandleMouseWheel(delta);
        break;

    default:
        break;
    }
}




DialogueUI& UIManager::GetDialogueUI()
{
    return dialogueUI;
}

StartMenu& UIManager::GetStartMenu()
{
    return startMenu;
}

PauseMenu& UIManager::GetPauseMenu()
{
    return pauseMenu;
}

SaveMenu& UIManager::GetSaveMenu()
{
    return saveMenu;
}

ConfigMenu& UIManager::GetConfigMenu()
{
    return configMenu;
}

AffectionMenu& UIManager::GetAffectionMenu()
{
    return affectionMenu;
}

ChapterTitle& UIManager::GetChapterTitle()
{
    return chapterTitle;
}
