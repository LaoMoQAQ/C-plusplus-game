#include "PauseMenu.h"



PauseMenu::PauseMenu()
{
    choice = 0;

    items[0] = "继续游戏";
    items[1] = "保存游戏";
    items[2] = "读取存档";
    items[3] = "好感度";
    items[4] = "设置";
    items[5] = "返回标题";

    displayHighlightY = (float)MENU_Y;
}





void PauseMenu::Update()
{
    float target = (float)(MENU_Y + choice * MENU_GAP);

    float diff = target - displayHighlightY;

    if(diff > -0.5f && diff < 0.5f)
        displayHighlightY = target;
    else
        displayHighlightY += diff * hlSpeed;
}





void PauseMenu::Render(Renderer& renderer)
{
    // 遮罩
    renderer.DrawFilledRect(
        0, 0,
        UILayout::SCREEN_W,
        UILayout::SCREEN_H,
        UILayout::OVERLAY_R,
        UILayout::OVERLAY_G,
        UILayout::OVERLAY_B,
        UILayout::OVERLAY_A
    );


    // 标题
    renderer.DrawText("暂停", 740, 150);

    renderer.DrawFilledRect(
        700, 200, 200, 2,
        UILayout::LINE_R,
        UILayout::LINE_G,
        UILayout::LINE_B,
        UILayout::LINE_A
    );


    // 高亮块
    renderer.DrawFilledRoundRect(
        MENU_X - 20,
        (int)displayHighlightY - 6,
        ITEM_W + 40,
        ITEM_H - 4,
        UILayout::HL_RADIUS,
        UILayout::HL_R,
        UILayout::HL_G,
        UILayout::HL_B,
        UILayout::HL_A
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


    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );
}





void PauseMenu::HandleInput(int key)
{
    hlSpeed = HL_ANIM_SPEED;   // [新增] 键盘：慢速

    if(key == 1)
    {
        choice--;
        if(choice < 0) choice = ITEM_COUNT - 1;
    }
    else if(key == 2)
    {
        choice++;
        if(choice >= ITEM_COUNT) choice = 0;
    }
}





void PauseMenu::HandleMouseMove(int x, int y)
{
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int ix = MENU_X;
        int iy = MENU_Y + i * MENU_GAP;

        if(x >= ix && x < ix + ITEM_W &&
           y >= iy && y < iy + ITEM_H)
        {
            hlSpeed = HL_ANIM_SPEED_FAST;   // [新增] 鼠标：快速
            choice = i;
            return;
        }
    }
}





MenuMouseResult PauseMenu::HandleMouseClick(int x, int y)
{
    if(x >= UILayout::BACK_X &&
       x <  UILayout::BACK_X + UILayout::BACK_W &&
       y >= UILayout::BACK_Y &&
       y <  UILayout::BACK_Y + UILayout::BACK_H)
    {
        return MenuMouseResult::BACK;
    }


    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int ix = MENU_X;
        int iy = MENU_Y + i * MENU_GAP;

        if(x >= ix && x < ix + ITEM_W &&
           y >= iy && y < iy + ITEM_H)
        {
            choice = i;
            return MenuMouseResult::ACTIVATE;
        }
    }

    return MenuMouseResult::NONE;
}





int PauseMenu::GetChoice() const
{
    return choice;
}





void PauseMenu::Reset()
{
    choice = 0;
}