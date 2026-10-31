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




// ==========================================================
// SetStateImmediate
// ==========================================================
//
// 直接切状态，不播黑幕过渡。
//
// 「主菜单 -> 剧情」走这个：那一瞬间背景、立绘、对话框都已经
// 按事件设置好了，再压黑一次纯属多余，玩家看到的就是黑一下。

void UIManager::SetStateImmediate(UIState state)
{
    currentState = state;
    pendingState = state;

    transition = TransState::NONE;
    alpha = 1.0f;
}





// ==========================================================
// GetState
// ==========================================================
//
// 只返回 currentState，不返回 pendingState。
//
// 之前过渡期间返回 pendingState，而 Render() 用的是 currentState，
// 两者不一致：画面还在画旧菜单，输入却已经按新菜单处理。
// 结果是过渡动画期间按方向键会操作到"还没显示出来的"菜单，
// 表现为画面乱跳、像闪屏。

UIState UIManager::GetState() const
{
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

void UIManager::Update(float delta)
{
    // ---- 过渡 ----
    //
    // 过渡速度同样按 60 FPS 标定，用 SmoothFactor 换算，
    // 保证不同帧率下淡入淡出的时长一致。

    float step = SmoothFactor(FADE_SPEED, delta);

    if(transition == TransState::OUT)
    {
        alpha -= step;

        if(alpha <= 0.0f)
        {
            alpha = 0.0f;
            currentState = pendingState;
            transition = TransState::IN;
        }
    }
    else if(transition == TransState::IN)
    {
        alpha += step;

        if(alpha >= 1.0f)
        {
            alpha = 1.0f;
            transition = TransState::NONE;
        }
    }


    // ---- 菜单内动画 ----

    // 全调一遍，非当前菜单也无所谓
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