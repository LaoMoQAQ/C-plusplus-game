#ifndef MENU_COMMON_H
#define MENU_COMMON_H


// ==========================================================
// 所有菜单共用的布局、颜色与枚举
// ==========================================================

namespace UILayout
{
    // ---- 屏幕尺寸（和 config.ini 保持一致） ----

    constexpr int SCREEN_W = 1600;
    constexpr int SCREEN_H = 900;


    // ---- 返回按钮 ----

    constexpr int BACK_X = 1420;
    constexpr int BACK_Y = 830;
    constexpr int BACK_W = 160;
    constexpr int BACK_H = 50;



    // ---- 二级菜单：全屏暗色遮罩 ----
    //
    // 让菜单文字在复杂背景上也能看清。
    // 120 大约 47% 不透明。

    constexpr unsigned char OVERLAY_R = 0;
    constexpr unsigned char OVERLAY_G = 0;
    constexpr unsigned char OVERLAY_B = 0;
    constexpr unsigned char OVERLAY_A = 120;



    // ---- 菜单项选中高亮 ----

    constexpr unsigned char HL_R = 90;
    constexpr unsigned char HL_G = 140;
    constexpr unsigned char HL_B = 210;
    constexpr unsigned char HL_A = 180;

    constexpr int HL_RADIUS = 10;    // 圆角半径



    // ---- 菜单面板（主菜单右侧承托） ----

    constexpr unsigned char PANEL_R = 20;
    constexpr unsigned char PANEL_G = 20;
    constexpr unsigned char PANEL_B = 30;
    constexpr unsigned char PANEL_A = 140;

    constexpr int PANEL_RADIUS = 16;



    // ---- 标题装饰横线 ----

    constexpr unsigned char LINE_R = 220;
    constexpr unsigned char LINE_G = 220;
    constexpr unsigned char LINE_B = 220;
    constexpr unsigned char LINE_A = 180;
}



// ==========================================================
// MenuMouseResult
// ==========================================================

enum class MenuMouseResult
{
    NONE,
    ACTIVATE,
    BACK
};


#endif