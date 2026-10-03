#include "UIManager.h"



UIManager::UIManager()
{
    currentState = UIState::START;
    pendingState = UIState::START;

    transition = TransState::NONE;
    alpha = 1.0f;
}





void UIManager::SetState(UIState state)
{
    if(state == currentState || state == pendingState)
        return;

    pendingState = state;
    transition = TransState::OUT;
}





UIState UIManager::GetState() const
{
    if(transition != TransState::NONE)
        return pendingState;

    return currentState;
}





bool UIManager::IsTransitioning() const
{
    return transition != TransState::NONE;
}





// ==========================================================
// Update
// ==========================================================
//
// 1. 推进过渡动画
// 2. 推进各菜单内部动画（高亮块滑动）

void UIManager::Update()
{
    // ---- 过渡 ----

    if(transition == TransState::OUT)
    {
        alpha -= FADE_SPEED;

        if(alpha <= 0.0f)
        {
            alpha = 0.0f;
            currentState = pendingState;
            transition = TransState::IN;
        }
    }
    else if(transition == TransState::IN)
    {
        alpha += FADE_SPEED;

        if(alpha >= 1.0f)
        {
            alpha = 1.0f;
            transition = TransState::NONE;
        }
    }


    // ---- 菜单内动画 ----

    // 全调一遍，非当前菜单也无所谓
    startMenu.Update();
    pauseMenu.Update();
    configMenu.Update();
    saveMenu.Update();
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


    if(alpha < 1.0f)
    {
        Uint8 a = (Uint8)((1.0f - alpha) * 255.0f);

        renderer.DrawFilledRect(
            0, 0,
            UILayout::SCREEN_W,
            UILayout::SCREEN_H,
            0, 0, 0,
            a
        );
    }
}





void UIManager::HandleInput(int key)
{
    switch(currentState)
    {
    case UIState::START:  startMenu.HandleInput(key);  break;
    case UIState::PAUSE:  pauseMenu.HandleInput(key);  break;
    case UIState::SAVE:   saveMenu.HandleInput(key);   break;
    case UIState::CONFIG: configMenu.HandleInput(key); break;
    default: break;
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