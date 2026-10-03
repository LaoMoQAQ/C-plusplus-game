#ifndef START_MENU_H
#define START_MENU_H


#include <string>

#include "../core/Renderer.h"
#include "../core/FontManager.h"

#include "MenuCommon.h"


class StartMenu
{

public:

    StartMenu();

    void SetFontManager(FontManager* fm);
    void Render(Renderer& renderer);
    void HandleInput(int key);
    void HandleMouseMove(int x, int y);
    MenuMouseResult HandleMouseClick(int x, int y);

    int GetChoice() const;
    void Reset();

    void Update();



private:

    int choice;

    static constexpr int ITEM_COUNT = 4;

    std::string items[ITEM_COUNT];

    FontManager* fontManager = nullptr;


    static constexpr int MENU_X   = 1360;
    static constexpr int MENU_Y   = 400;
    static constexpr int MENU_GAP = 75;
    static constexpr int ITEM_W   = 220;
    static constexpr int ITEM_H   = 60;


    // 高亮块显示位置（平滑后的 Y）
    float displayHighlightY = 400.0f;

    // [新增] 双速度：
    //   键盘切换用慢速，视觉舒缓
    //   鼠标移动用快速，跟手
    static constexpr float HL_ANIM_SPEED      = 0.25f;
    static constexpr float HL_ANIM_SPEED_FAST = 0.55f;

    float hlSpeed = HL_ANIM_SPEED;

};


#endif