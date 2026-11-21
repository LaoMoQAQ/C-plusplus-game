#include "PauseMenu.h"

#include "../core/Anim.h"



PauseMenu::PauseMenu()
{
    choice = 0;

    items[0] = "继续游戏";
    items[1] = "保存游戏";
    items[2] = "读取存档";
    items[3] = "章节选择";
    items[4] = "好感度";
    items[5] = "设置";
    items[6] = "返回标题";

    displayHighlightY = (float)MENU_Y;
}




void PauseMenu::Update(float delta)
{
    float target = (float)(MENU_Y + choice * MENU_GAP);

    float diff = target - displayHighlightY;

    if(diff > -0.5f && diff < 0.5f)
        displayHighlightY = target;
    else
        displayHighlightY += diff * SmoothFactor(hlSpeed, delta);


    // 入场动画：每次进暂停菜单都重新播一遍
    if(appear < 1.0f)
    {
        appear += delta * 6.0f;

        if(appear > 1.0f)
            appear = 1.0f;
    }
}




void PauseMenu::Render(Renderer& renderer)
{
    float eased = Anim::EaseOutCubic(appear);


    // ---- 全屏压暗 ----
    //
    // 背景已经是 EndScene(true) 生成的强模糊 + 压暗，
    // 这里只补一点点，避免面板外太亮抢戏。

    renderer.DrawFilledRect(
        0, 0,
        UILayout::SCREEN_W,
        UILayout::SCREEN_H,
        0, 0, 0,
        (Uint8)(PAUSE_OVERLAY_A * eased)
    );


    // ---- 玻璃面板 ----
    //
    // 面板从 96% 放大到 100%，同时淡入。
    // DrawGlassPanel 内部会采样背景的强模糊纹理做折射。

    float grow = 0.96f + 0.04f * eased;

    float gw = PANEL_W * grow;
    float gh = PANEL_H * grow;

    float gx = PANEL_X + (PANEL_W - gw) * 0.5f;
    float gy = PANEL_Y + (PANEL_H - gh) * 0.5f;

    // tintAlpha 越小越通透（背景模糊透出来越多）。
    // 0.82 会让面板接近纯黑，玻璃感就没了；
    // 0.5 左右既能压住背景保证文字可读，又能看出折射。
    renderer.DrawGlassPanel(
        gx, gy, gw, gh,
        (float)PANEL_RADIUS,
        0.04f, 0.045f, 0.06f,    // 深色底
        0.52f,                    // 通透度
        0.96f * eased,            // 整体不透明度
        24.0f                     // 投影强度
    );


    // ---- 面板内内容（带入场偏移）----

    renderer.SetContentOffset(0.0f, (1.0f - eased) * 14.0f);
    renderer.SetContentAlpha(eased);


    // 标题（在面板内部居中）
    if(fontManager)
        DrawMenuTitle(renderer, *fontManager, "暂停", PANEL_Y + 34);
    else
        renderer.DrawText("暂停", MENU_X + 180, PANEL_Y + 34);


    // 高亮块
    renderer.DrawFilledRoundRect(
        MENU_X - 14,
        (int)displayHighlightY - 8,
        ITEM_W + 28,
        ITEM_H,
        UILayout::HL_RADIUS,
        UILayout::HL_R,
        UILayout::HL_G,
        UILayout::HL_B,
        (Uint8)(UILayout::HL_A * eased)
    );


    // 菜单文字
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int iy = MENU_Y + i * MENU_GAP;

        std::string text;

        if(i == choice)
            text = "> " + items[i];
        else
            text = items[i];

        renderer.DrawText(text, MENU_X, iy);
    }


    // 内容动画结束，恢复默认
    renderer.SetContentOffset(0.0f, 0.0f);
    renderer.SetContentAlpha(1.0f);


    // ---- 底部按键提示（面板外，不受面板动画影响）----

    if(fontManager)
    {
        DrawKeyHints(
            renderer, *fontManager, 812,
            {
                { "↑↓",    "选择" },
                { "Enter", "确认" },
                { "ESC",   "返回" }
            }
        );
    }
}





void PauseMenu::HandleInput(int key)
{
    if(key == SDLK_UP)
    {
        choice--;

        if(choice < 0)
            choice = ITEM_COUNT - 1;

        hlSpeed = HL_ANIM_SPEED_FAST;
    }
    else if(key == SDLK_DOWN)
    {
        choice++;

        if(choice >= ITEM_COUNT)
            choice = 0;

        hlSpeed = HL_ANIM_SPEED_FAST;
    }
}




int PauseMenu::GetChoice() const
{
    return choice;
}




void PauseMenu::Reset()
{
    choice = 0;
    displayHighlightY = (float)MENU_Y;

    appear = 0.0f;
    hlSpeed = HL_ANIM_SPEED;
}




void PauseMenu::HandleMouseMove(int x, int y)
{
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int ix = MENU_X - 14;
        int iy = MENU_Y + i * MENU_GAP - 8;

        if(x >= ix && x < ix + ITEM_W + 28 &&
           y >= iy && y < iy + ITEM_H)
        {
            hlSpeed = HL_ANIM_SPEED_FAST;
            choice = i;
            return;
        }
    }
}




MenuMouseResult PauseMenu::HandleMouseClick(int x, int y)
{
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int ix = MENU_X - 14;
        int iy = MENU_Y + i * MENU_GAP - 8;

        if(x >= ix && x < ix + ITEM_W + 28 &&
           y >= iy && y < iy + ITEM_H)
        {
            choice = i;
            return MenuMouseResult::ACTIVATE;
        }
    }

    return MenuMouseResult::NONE;
}
