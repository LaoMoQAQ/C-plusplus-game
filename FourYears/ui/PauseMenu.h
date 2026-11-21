#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H


#include <string>

#include "../core/Renderer.h"

#include "MenuCommon.h"



// ==========================================================
// PauseMenu
// ==========================================================
//
// 布局：屏幕正中的一块玻璃面板，菜单项左对齐排在面板内。
//
// 之前是"全屏半透明遮罩 + 裸文字"，视觉上很平。
// 现在改成悬浮玻璃面板：
//   面板背景是背景的强模糊 + 折射（液体玻璃），
//   边缘有高光、下方有投影，文字压在面板上。
//
// 面板尺寸按菜单项数量算出来，上下留同样的内边距。

class PauseMenu
{

public:

    PauseMenu();

    // 按键提示需要按字号取字体
    void SetFontManager(FontManager* fm) { fontManager = fm; }

    void Render(Renderer& renderer);
    void HandleInput(int key);
    void HandleMouseMove(int x, int y);
    MenuMouseResult HandleMouseClick(int x, int y);

    int GetChoice() const;
    void Reset();

    void Update(float delta);



private:

    // 按键提示按字号取字体，未设置时提示条不画
    FontManager* fontManager = nullptr;

    static constexpr int ITEM_COUNT = 6;

    int choice;

    std::string items[ITEM_COUNT];


    // ---- 玻璃面板 ----
    //
    // 面板宽度定死 560，高度按 ITEM_COUNT 算。
    // 菜单项在面板内左右各留 PANEL_PAD_X 的边距。

    static constexpr int PANEL_W     = 560;
    static constexpr int PANEL_PAD_X = 44;
    static constexpr int PANEL_PAD_Y = 52;
    static constexpr int PANEL_RADIUS = 22;

    static constexpr int MENU_GAP = 58;
    static constexpr int ITEM_H   = 44;

    // 面板高度 = 上下内边距 + 所有菜单项
    static constexpr int PANEL_H =
        PANEL_PAD_Y * 2 + ITEM_COUNT * MENU_GAP;

    // 面板左上角（水平居中，垂直略偏上）
    static constexpr int PANEL_X = (UILayout::SCREEN_W - PANEL_W) / 2;
    static constexpr int PANEL_Y = 170;

    // 菜单项起始位置：面板左内边距、标题下方
    static constexpr int MENU_X = PANEL_X + PANEL_PAD_X;
    static constexpr int MENU_Y = PANEL_Y + PANEL_PAD_Y + 46;
    static constexpr int ITEM_W = PANEL_W - PANEL_PAD_X * 2;


    // 面板外的压暗程度。背景已经是强模糊，
    // 这里只要很淡一层，把面板外压暗一点突出主体。
    static constexpr Uint8 PAUSE_OVERLAY_A = 70;


    float displayHighlightY = (float)MENU_Y;

    // 入场动画：0 -> 1
    float appear = 0.0f;

    static constexpr float HL_ANIM_SPEED      = 0.25f;
    static constexpr float HL_ANIM_SPEED_FAST = 0.55f;

    float hlSpeed = HL_ANIM_SPEED;

};


#endif
