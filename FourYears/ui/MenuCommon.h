#ifndef MENU_COMMON_H
#define MENU_COMMON_H


// ==========================================================
// 所有菜单共用的坐标和枚举
// ==========================================================

namespace UILayout
{
    // 返回按钮（屏幕右下角）
    constexpr int BACK_X = 1420;
    constexpr int BACK_Y = 830;
    constexpr int BACK_W = 160;
    constexpr int BACK_H = 50;
}


// 鼠标点击菜单后的结果
//
// NONE     - 没命中任何东西
// ACTIVATE - 命中了菜单项，
//            Game 应该调用 OnActivateCurrentState()
// BACK     - 命中了"返回"按钮，
//            Game 应该调用 OnBack()
enum class MenuMouseResult
{
    NONE,
    ACTIVATE,
    BACK
};


#endif