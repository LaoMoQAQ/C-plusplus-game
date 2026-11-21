#include "ChapterTitle.h"

#include "../core/Anim.h"
#include "../core/GL.h"

#include "MenuCommon.h"



namespace
{
    constexpr float FADE_IN  = 0.45f;
    constexpr float HOLD     = 1.25f;
    constexpr float FADE_OUT = 0.55f;

    constexpr float TOTAL = FADE_IN + HOLD + FADE_OUT;
}




void ChapterTitle::SetFontManager(FontManager* fm)
{
    fontManager = fm;
}




void ChapterTitle::Show(
    const std::string& t,
    const std::string& s
)
{
    title = t;
    subtitle = s;

    time = 0.0f;
    active = true;
}




void ChapterTitle::Skip()
{
    active = false;
    time = TOTAL;
}




void ChapterTitle::Update(float delta)
{
    if(!active)
        return;

    time += delta;

    if(time >= TOTAL)
        active = false;
}




bool ChapterTitle::IsActive() const
{
    return active;
}




void ChapterTitle::Render(Renderer& renderer)
{
    if(!active)
        return;


    // ---- 计算整体不透明度 ----

    float a;

    if(time < FADE_IN)
        a = time / FADE_IN;
    else if(time < FADE_IN + HOLD)
        a = 1.0f;
    else
        a = (TOTAL - time) / FADE_OUT;

    if(a < 0.0f) a = 0.0f;
    if(a > 1.0f) a = 1.0f;

    float eased = Anim::EaseOutCubic(a);


    // ---- 全屏压暗 ----

    renderer.DrawFilledRect(
        0, 0, 1600, 900,
        0, 0, 0,
        (Uint8)(175.0f * eased)
    );


    // ---- 中间的玻璃面板 ----

    const float px = 400.0f;
    const float py = 340.0f;
    const float pw = 800.0f;
    const float ph = 220.0f;

    float grow = 0.96f + 0.04f * eased;
    float gw = pw * grow;
    float gh = ph * grow;

    renderer.DrawGlassPanel(
        px + (pw - gw) * 0.5f,
        py + (ph - gh) * 0.5f,
        gw, gh,
        (float)UILayout::PANEL_RADIUS,
        0.04f, 0.045f, 0.06f,    // 深色底
        0.52f,                    // 通透度（和其他面板一致）
        0.95f * eased,
        0.0f                      // 标题卡不需要投影
    );


    // ---- 文字（整体淡入 + 轻微上移）----

    renderer.SetContentOffset(0.0f, (1.0f - eased) * 16.0f);
    renderer.SetContentAlpha(eased);


    TTF_Font* titleFont = fontManager
                        ? fontManager->GetFont("resource/font/title.ttf", 68)
                        : nullptr;

    if(titleFont)
    {
        int tw = 0, th = 0;

        if(renderer.MeasureText(title, titleFont, tw, th))
        {
            SDL_Color c;
            c.r = 245;
            c.g = 246;
            c.b = 252;
            c.a = 255;

            renderer.DrawText(
                title,
                (int)(px + (pw - (float)tw) * 0.5f),
                (int)(py + 52.0f),
                titleFont,
                c
            );
        }
    }


    if(fontManager)
    {
        int sw = 0, sh = 0;

        if(renderer.MeasureText(subtitle, sw, sh))
        {
            SDL_Color c;
            c.r = 176;
            c.g = 196;
            c.b = 226;
            c.a = 255;

            renderer.DrawText(
                subtitle,
                (int)(px + (pw - (float)sw) * 0.5f),
                (int)(py + 140.0f),
                c
            );
        }
    }


    renderer.SetContentOffset(0.0f, 0.0f);
    renderer.SetContentAlpha(1.0f);
}
