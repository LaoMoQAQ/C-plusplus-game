#ifndef MENU_COMMON_H
#define MENU_COMMON_H


#include <cmath>
#include <initializer_list>

#include "../core/Renderer.h"
#include "../core/FontManager.h"


// ==========================================================
// 帧率无关的平滑系数
// ==========================================================
//
// 各菜单的高亮块用指数逼近：
//   display += (target - display) * factor
//
// 直接写死 factor 会让手感随帧率变化。这里把
// "每帧 1/60 秒收敛 perFrame" 换算成当前 delta 下的等效系数，
// 60 FPS 时结果和原来完全一致，其他帧率下手感不变。
//
// perFrame 取 (0, 1]，delta 单位为秒。

inline float SmoothFactor(float perFrame, float delta)
{
    if(perFrame >= 1.0f)
        return 1.0f;

    if(perFrame <= 0.0f || delta <= 0.0f)
        return 0.0f;

    return 1.0f - std::pow(1.0f - perFrame, delta * 60.0f);
}


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



    // ---- 按键提示（底部操作条） ----
    //
    // 键帽是带浅色底、略微提亮的圆角块，标签是中性灰小字。
    // 比原来直接写 "[ESC]" 这种纯文本清晰得多。
    //
    // 对比度调过两轮：透明度 46 在浅色背景上几乎看不见，
    // 提到 92 之后键帽轮廓在任何背景上都能分辨。

    constexpr unsigned char KEYCAP_R = 250;
    constexpr unsigned char KEYCAP_G = 250;
    constexpr unsigned char KEYCAP_B = 252;
    constexpr unsigned char KEYCAP_A = 92;

    constexpr unsigned char KEYTEXT_R = 255;
    constexpr unsigned char KEYTEXT_G = 255;
    constexpr unsigned char KEYTEXT_B = 255;
    constexpr unsigned char KEYTEXT_A = 250;

    constexpr unsigned char HINT_R = 198;
    constexpr unsigned char HINT_G = 202;
    constexpr unsigned char HINT_B = 212;
    constexpr unsigned char HINT_A = 240;

    constexpr int KEYCAP_H      = 38;           // 键帽高度
    constexpr int KEYCAP_PAD_X  = 14;           // 键帽左右内边距
    constexpr int KEYCAP_RADIUS = 8;
    constexpr int KEYCAP_GAP    = 9;            // 键帽与说明文字的间距
    constexpr int HINT_GAP      = 38;           // 不同提示之间的间距
    constexpr int HINT_SIZE     = 24;           // 说明文字字号
    constexpr int KEY_SIZE      = 20;           // 键帽文字字号


    // 按键提示条背后的压暗条。
    // 底部是画面最亮的地方，不加这个提示文字会和背景糊在一起。

    constexpr unsigned char HINTBAR_R = 0;
    constexpr unsigned char HINTBAR_G = 0;
    constexpr unsigned char HINTBAR_B = 0;
    constexpr unsigned char HINTBAR_A = 110;
}


// ==========================================================
// 按键提示绘制
// ==========================================================
//
// 用法：
//     DrawKeyHints(renderer, fontMgr, y,
//                  { {"←→", "调整"}, {"S", "保存"}, {"ESC", "返回"} });
//
// 整条提示水平居中。每个提示 = 圆角键帽 + 右侧说明文字。

struct KeyHint
{
    const char* key;    // 键帽上显示的字，如 "ESC"、"←→"
    const char* desc;   // 说明，如 "返回"
};


inline void DrawKeyHints(
    Renderer& renderer,
    FontManager& fontMgr,
    int y,
    std::initializer_list<KeyHint> hints,
    int centerX = UILayout::SCREEN_W / 2
){
    TTF_Font* keyFont  = fontMgr.GetFont(UILayout::KEY_SIZE);
    TTF_Font* descFont = fontMgr.GetFont(UILayout::HINT_SIZE);

    if(!keyFont || !descFont)
        return;


    // ---- 先量总宽度，才能整体居中 ----

    int totalW = 0;
    int n = 0;

    for(const auto& h : hints)
    {
        int kw = 0, kh = 0;
        TTF_SizeUTF8(keyFont, h.key, &kw, &kh);

        int dw = 0, dh = 0;
        TTF_SizeUTF8(descFont, h.desc, &dw, &dh);

        totalW += kw + UILayout::KEYCAP_PAD_X * 2;   // 键帽
        totalW += UILayout::KEYCAP_GAP;              // 键帽与说明之间
        totalW += dw;                                // 说明文字

        n++;
    }

    if(n > 1)
        totalW += UILayout::HINT_GAP * (n - 1);

    if(totalW <= 0)
        return;


    int x = centerX - totalW / 2;


    // ---- 背衬 ----
    //
    // 底部是画面最亮的区域，纯文字容易和背景糊在一起。
    // 先铺一条半透明暗色圆角条，把提示文字托起来。

    {
        int barPadX = 34;
        int barPadY = 11;

        renderer.DrawFilledRoundRect(
            x - barPadX,
            y - barPadY,
            totalW + barPadX * 2,
            UILayout::KEYCAP_H + barPadY * 2,
            UILayout::KEYCAP_RADIUS + 6,
            UILayout::HINTBAR_R,
            UILayout::HINTBAR_G,
            UILayout::HINTBAR_B,
            UILayout::HINTBAR_A
        );
    }


    // ---- 逐个画 ----

    SDL_Color keyColor = {
        UILayout::KEYTEXT_R,
        UILayout::KEYTEXT_G,
        UILayout::KEYTEXT_B,
        UILayout::KEYTEXT_A
    };

    SDL_Color descColor = {
        UILayout::HINT_R,
        UILayout::HINT_G,
        UILayout::HINT_B,
        UILayout::HINT_A
    };


    for(const auto& h : hints)
    {
        int kw = 0, kh = 0;
        TTF_SizeUTF8(keyFont, h.key, &kw, &kh);

        int dw = 0, dh = 0;
        TTF_SizeUTF8(descFont, h.desc, &dw, &dh);


        // 键帽
        int capW = kw + UILayout::KEYCAP_PAD_X * 2;

        renderer.DrawFilledRoundRect(
            x,
            y,
            capW,
            UILayout::KEYCAP_H,
            UILayout::KEYCAP_RADIUS,
            UILayout::KEYCAP_R,
            UILayout::KEYCAP_G,
            UILayout::KEYCAP_B,
            UILayout::KEYCAP_A
        );


        // 键帽文字（在键帽内水平居中、垂直居中）
        renderer.DrawText(
            h.key,
            x + (capW - kw) / 2,
            y + (UILayout::KEYCAP_H - kh) / 2,
            keyFont,
            keyColor
        );


        x += capW + UILayout::KEYCAP_GAP;


        // 说明文字（和键帽垂直居中对齐）
        renderer.DrawText(
            h.desc,
            x,
            y + (UILayout::KEYCAP_H - dh) / 2,
            descFont,
            descColor
        );


        x += dw + UILayout::HINT_GAP;
    }
}



// ==========================================================
// 菜单标题（居中标题 + 下方短横线）
// ==========================================================
//
// 原来各菜单都写死 DrawText("设置", 740, 100)，
// 但 740 对 2 字/3 字/4 字标题都不居中——
// 字越少偏得越明显（截图里「存档管理」明显偏左）。
//
// 这里按实际文字宽度算居中位置，横线也跟着居中。

inline void DrawMenuTitle(
    Renderer& renderer,
    FontManager& fontMgr,
    const char* title,
    int y,
    int lineGap = 14,       // 标题底部到横线的距离
    int lineW   = 200,      // 横线宽度（保持各菜单一致）
    int titleSize = 32
)
{
    TTF_Font* font = fontMgr.GetFont(titleSize);

    if(!font)
        return;

    int tw = 0, th = 0;
    TTF_SizeUTF8(font, title, &tw, &th);

    if(tw <= 0)
        return;

    int cx = UILayout::SCREEN_W / 2;

    // 标题水平居中
    renderer.DrawText(
        title,
        cx - tw / 2,
        y,
        font,
        SDL_Color{ 255, 255, 255, 255 }
    );

    // 横线也居中，并且和标题同宽（不够 lineW 时用 lineW，避免太短）
    int w = (tw > lineW) ? tw : lineW;

    renderer.DrawFilledRect(
        cx - w / 2,
        y + th + lineGap,
        w,
        2,
        UILayout::LINE_R,
        UILayout::LINE_G,
        UILayout::LINE_B,
        UILayout::LINE_A
    );
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