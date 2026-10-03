#include "StartMenu.h"



StartMenu::StartMenu()
{
    choice = 0;

    items[0] = "开始游戏";
    items[1] = "存档";
    items[2] = "设置";
    items[3] = "退出游戏";

    displayHighlightY = (float)MENU_Y;
}





void StartMenu::SetFontManager(FontManager* fm)
{
    fontManager = fm;
}





void StartMenu::Update()
{
    float target = (float)(MENU_Y + choice * MENU_GAP);

    float diff = target - displayHighlightY;

    if(diff > -0.5f && diff < 0.5f)
        displayHighlightY = target;
    else
        displayHighlightY += diff * hlSpeed;
}





void StartMenu::Render(Renderer& renderer)
{
    if(fontManager)
    {
        TTF_Font* titleFont =
            fontManager->GetFont("resource/font/title.ttf", 90);

        if(titleFont)
        {
            SDL_Color c = { 255, 255, 255, 255 };
            renderer.DrawText("FourYears", 180, 80, titleFont, c);
        }


        TTF_Font* subFont = fontManager->GetFont(28);

        if(subFont)
        {
            SDL_Color c = { 255, 255, 255, 255 };
            renderer.DrawText("四年 · 我们的青春", 280, 220, subFont, c);
        }
    }
    else
    {
        renderer.DrawText("Four Years", 80, 60);
    }


    // 面板
    const int panelX = MENU_X - 40;
    const int panelY = MENU_Y - 60;
    const int panelW = ITEM_W + 80;
    const int panelH = ITEM_COUNT * MENU_GAP + 60;

    renderer.DrawFilledRoundRect(
        panelX, panelY, panelW, panelH,
        UILayout::PANEL_RADIUS,
        UILayout::PANEL_R,
        UILayout::PANEL_G,
        UILayout::PANEL_B,
        UILayout::PANEL_A
    );


    // 高亮块（平滑位置）
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


    // 文字
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
}





void StartMenu::HandleInput(int key)
{
    // [新增] 键盘切换：慢速动画
    hlSpeed = HL_ANIM_SPEED;

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





void StartMenu::HandleMouseMove(int x, int y)
{
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int ix = MENU_X;
        int iy = MENU_Y + i * MENU_GAP;

        if(x >= ix && x < ix + ITEM_W &&
           y >= iy && y < iy + ITEM_H)
        {
            // [新增] 鼠标移动：快速动画，几乎跟手
            hlSpeed = HL_ANIM_SPEED_FAST;
            choice = i;
            return;
        }
    }
}





MenuMouseResult StartMenu::HandleMouseClick(int x, int y)
{
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





int StartMenu::GetChoice() const
{
    return choice;
}





void StartMenu::Reset()
{
    choice = 0;
}