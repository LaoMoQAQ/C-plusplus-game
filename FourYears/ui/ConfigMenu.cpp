#include "ConfigMenu.h"



ConfigMenu::ConfigMenu()
{
    config = nullptr;
    choice = 0;

    items[0] = "BGM音量";
    items[1] = "SE音量";
    items[2] = "文字速度";
    items[3] = "全屏";
    items[4] = "自动播放";

    displayHighlightY = (float)MENU_Y;
}





void ConfigMenu::SetConfig(Config* config)
{
    this->config = config;
    dirty = false;
    confirmSave = false;
}





void ConfigMenu::Update()
{
    float target = (float)(MENU_Y + choice * MENU_GAP);

    float diff = target - displayHighlightY;

    if(diff > -0.5f && diff < 0.5f)
        displayHighlightY = target;
    else
        displayHighlightY += diff * hlSpeed;
}




void ConfigMenu::Render(Renderer& renderer)
{
    // ---- 全屏遮罩 ----

    renderer.DrawFilledRect(
        0, 0,
        UILayout::SCREEN_W,
        UILayout::SCREEN_H,
        UILayout::OVERLAY_R,
        UILayout::OVERLAY_G,
        UILayout::OVERLAY_B,
        UILayout::OVERLAY_A
    );


    // ---- 标题 ----

    renderer.DrawText("设置", 740, 100);

    renderer.DrawFilledRect(
        700, 150, 200, 2,
        UILayout::LINE_R,
        UILayout::LINE_G,
        UILayout::LINE_B,
        UILayout::LINE_A
    );


    if(config == nullptr)
    {
        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );
        return;
    }


    // ---- 高亮块 ----

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


    // ---- 菜单项 ----

    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int iy = MENU_Y + i * MENU_GAP;

        std::string value;

        if(i == 0)
            value = std::to_string(config->GetBGMVolume());
        else if(i == 1)
            value = std::to_string(config->GetSEVolume());
        else if(i == 2)
            value = std::to_string(config->GetTextSpeed()) + " 字/秒";
        else if(i == 3)
            value = config->IsFullscreen() ? "开启" : "关闭";
        else if(i == 4)
            value = config->IsAutoPlay() ? "开启" : "关闭";


        std::string text;

        if(i == choice)
            text = "> ";

        text += items[i];

        if(!value.empty())
            text += " : " + value;


        renderer.DrawText(text, MENU_X, iy);
    }


    // ---- 底部提示 ----

    if(confirmSave)
    {
        renderer.DrawText("设置已修改，是否保存？", 480, 730);
        renderer.DrawText(
            "Y 保存并返回  /  N 不保存返回  /  ESC 取消",
            420, 780
        );
    }
    else
    {
        renderer.DrawText("← → 调整   S 保存", 620, 750);
    }


    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );
}





void ConfigMenu::HandleInput(int key)
{
    if(config == nullptr) return;
    if(confirmSave) return;

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
    else if(key == 3)
    {
        switch(choice)
        {
        case 0:
        {
            int old = config->GetBGMVolume();
            int v = old - 5;
            if(v < 0) v = 0;
            if(v != old) { config->SetBGMVolume(v); dirty = true; }
        } break;

        case 1:
        {
            int old = config->GetSEVolume();
            int v = old - 5;
            if(v < 0) v = 0;
            if(v != old) { config->SetSEVolume(v); dirty = true; }
        } break;

        case 2:
        {
            int old = config->GetTextSpeed();
            int v = old - 5;
            if(v < 5) v = 5;
            if(v != old) { config->SetTextSpeed(v); dirty = true; }
        } break;

        case 3:
            if(config->IsFullscreen())
            { config->SetFullscreen(false); dirty = true; }
            break;

        case 4:
            if(config->IsAutoPlay())
            { config->SetAutoPlay(false); dirty = true; }
            break;
        }
    }
    else if(key == 4)
    {
        switch(choice)
        {
        case 0:
        {
            int old = config->GetBGMVolume();
            int v = old + 5;
            if(v > 100) v = 100;
            if(v != old) { config->SetBGMVolume(v); dirty = true; }
        } break;

        case 1:
        {
            int old = config->GetSEVolume();
            int v = old + 5;
            if(v > 100) v = 100;
            if(v != old) { config->SetSEVolume(v); dirty = true; }
        } break;

        case 2:
        {
            int old = config->GetTextSpeed();
            int v = old + 5;
            if(v > 60) v = 60;
            if(v != old) { config->SetTextSpeed(v); dirty = true; }
        } break;

        case 3:
            if(!config->IsFullscreen())
            { config->SetFullscreen(true); dirty = true; }
            break;

        case 4:
            if(!config->IsAutoPlay())
            { config->SetAutoPlay(true); dirty = true; }
            break;
        }
    }
}





void ConfigMenu::Save()
{
    if(config)
    {
        config->Save("config.ini");
        dirty = false;
    }
}





int ConfigMenu::GetChoice() const
{
    return choice;
}





void ConfigMenu::HandleMouseMove(int x, int y)
{
    if(confirmSave) return;

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




MenuMouseResult ConfigMenu::HandleMouseClick(int x, int y)
{
    if(x >= UILayout::BACK_X &&
       x <  UILayout::BACK_X + UILayout::BACK_W &&
       y >= UILayout::BACK_Y &&
       y <  UILayout::BACK_Y + UILayout::BACK_H)
    {
        return MenuMouseResult::BACK;
    }

    if(confirmSave) return MenuMouseResult::NONE;

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





bool ConfigMenu::TryExit()
{
    if(!dirty) return true;
    confirmSave = true;
    return false;
}





void ConfigMenu::CancelConfirm()
{
    confirmSave = false;
}





bool ConfigMenu::IsConfirming() const
{
    return confirmSave;
}





int ConfigMenu::HandleConfirmKey(int sym)
{
    if(!confirmSave) return -1;

    if(sym == SDLK_y)
    {
        Save();
        confirmSave = false;
        return 1;
    }

    if(sym == SDLK_n)
    {
        confirmSave = false;
        return 0;
    }

    if(sym == SDLK_ESCAPE)
    {
        confirmSave = false;
        return -1;
    }

    return -1;
}