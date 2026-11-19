#include "AffectionMenu.h"

#include <map>
#include <string>
#include <vector>




AffectionMenu::AffectionMenu()
{
}





void AffectionMenu::SetRouteManager(RouteManager* rm)
{
    routeManager = rm;
}





void AffectionMenu::Reset()
{
    scrollOffset = 0;
}





// ==========================================================
// HandleInput
// ==========================================================
//
// 上下滚动历史。key: 1=上 2=下

void AffectionMenu::HandleInput(int key)
{
    if(routeManager == nullptr)
        return;


    int total = (int)routeManager->GetRecords().size();
    int maxOffset = total - PAGE_SIZE;
    if(maxOffset < 0) maxOffset = 0;


    if(key == 1)   // 上
    {
        if(scrollOffset > 0)
            scrollOffset--;
    }
    else if(key == 2)   // 下
    {
        if(scrollOffset < maxOffset)
            scrollOffset++;
    }
}





// ==========================================================
// HandleMouseWheel
// ==========================================================
//
// 滚轮翻页。每次滚 1 条。

void AffectionMenu::HandleMouseWheel(int delta)
{
    if(routeManager == nullptr)
        return;


    int total = (int)routeManager->GetRecords().size();
    int maxOffset = total - PAGE_SIZE;
    if(maxOffset < 0) maxOffset = 0;


    if(delta > 0)
    {
        // 向下
        scrollOffset++;

        if(scrollOffset > maxOffset)
            scrollOffset = maxOffset;
    }
    else if(delta < 0)
    {
        // 向上
        scrollOffset--;

        if(scrollOffset < 0)
            scrollOffset = 0;
    }
}





void AffectionMenu::Render(Renderer& renderer)
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

    if(fontManager)
        DrawMenuTitle(renderer, *fontManager, "好感度", 70);
    else
        renderer.DrawText("好感度", 740, 70);



    if(routeManager == nullptr)
    {
        renderer.DrawText("暂无数据", 700, 200);

        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );
        return;
    }



    // ==========================================================
    // 上半部分：好感度
    // ==========================================================

    auto affection = routeManager->GetAllAffection();

    // 固定显示顺序：李君浩、张瀚宇、其他
    std::vector<std::string> order = { "李君浩", "张瀚宇" };

    int y = 170;

    for(auto& name : order)
    {
        int value = 0;
        auto it = affection.find(name);
        if(it != affection.end())
            value = it->second;


        // 名字（右对齐到进度条左侧）
        renderer.DrawText(name, 470, y);


        // 进度条背景
        renderer.DrawFilledRoundRect(
            660, y + 6,
            400, 24,
            8,
            40, 40, 40, 200
        );


        // 进度条前景
        // 满格值 100（对应 MAX_AFFECTION）
        int filledW = value * 400 / 100;
        if(filledW > 400) filledW = 400;

        if(filledW > 0)
        {
            renderer.DrawFilledRoundRect(
                660, y + 6,
                filledW, 24,
                8,
                90, 140, 210, 230
            );
        }


        // 数值
        std::string numStr =
            std::to_string(value) + " / 100";

        renderer.DrawText(numStr, 1090, y);


        y += 70;
    }



    // ==========================================================
    // 下半部分：变化历史
    // ==========================================================
    //
    // 呈现形式：卡片列表，不是表格。
    //
    // 原来是三列对齐的表格（#序号 / 角色 / +N），信息密度低、页面显空。
    // 现在每条记录是一张圆角卡片：
    //   [时间线圆点]  角色名        +N
    //                 来源文本
    // 卡片高度随内容变化，首尾相连形成时间线。

    int hy = 366;

    if(fontManager)
        DrawMenuTitle(renderer, *fontManager, "变化历史", hy, 12, 280, 28);
    else
    {
        renderer.DrawText("变化历史", 720, hy);
        renderer.DrawFilledRect(
            660, hy + 45, 280, 2,
            UILayout::LINE_R,
            UILayout::LINE_G,
            UILayout::LINE_B,
            UILayout::LINE_A
        );
    }


    const auto& records = routeManager->GetRecords();

    hy += 76;

    if(records.empty())
    {
        renderer.DrawText("（暂无记录）", 700, hy);
    }
    else
    {
        // 卡片布局参数
        // 纵向预算：
        //   标题底 = 366 + 约 46 = 412
        //   列表起点 442
        //   4 张卡片：4*68 + 3*8 = 296  -> 底部 738
        //   页码 772，提示条 838
        // 6 条放不下（442 + 6*68 + 5*8 = 890，已经压到提示条）。
        constexpr int CARD_X      = 470;
        constexpr int CARD_W      = 660;
        constexpr int CARD_H      = 66;
        constexpr int CARD_GAP    = 12;
        constexpr int DOT_R       = 5;      // 时间线圆点半径
        constexpr int DOT_X       = CARD_X + 26;
        constexpr int TEXT_X      = CARD_X + 52;

        TTF_Font* smallFont = fontManager ? fontManager->GetFont(20) : nullptr;

        for(int i = 0; i < PAGE_SIZE; i++)
        {
            int idx = scrollOffset + i;

            if(idx >= (int)records.size())
                break;

            const auto& rec = records[idx];

            int cy = hy + i * (CARD_H + CARD_GAP);


            // ---- 卡片底 ----

            renderer.DrawFilledRoundRect(
                CARD_X, cy, CARD_W, CARD_H, 12,
                14, 16, 24, 175
            );


            // ---- 时间线：圆点 + 竖线 ----
            //
            // 圆点标在卡片左侧，卡片之间用竖线连起来，
            // 让「按顺序发生」这件事一眼可见。

            renderer.DrawFilledRoundRect(
                DOT_X - DOT_R, cy + CARD_H / 2 - DOT_R,
                DOT_R * 2, DOT_R * 2,
                DOT_R,
                90, 140, 210, 230
            );

            if(i + 1 < PAGE_SIZE && idx + 1 < (int)records.size())
            {
                renderer.DrawFilledRect(
                    DOT_X - 1,
                    cy + CARD_H / 2 + DOT_R + 2,
                    2,
                    CARD_GAP + 2,
                    90, 140, 210, 90
                );
            }


            // ---- 角色名 ----

            renderer.DrawText(rec.character, TEXT_X, cy + 6);


            // ---- 变化值（右对齐） ----
            //
            // 正值用蓝绿，负值用暖红，一眼区分好坏。

            std::string deltaStr;

            if(rec.delta > 0)
                deltaStr = "+" + std::to_string(rec.delta);
            else
                deltaStr = std::to_string(rec.delta);

            if(fontManager)
            {
                TTF_Font* df = fontManager->GetFont();

                if(df)
                {
                    int dw = 0, dh = 0;
                    TTF_SizeUTF8(df, deltaStr.c_str(), &dw, &dh);

                    SDL_Color dc;

                    if(rec.delta > 0)
                        dc = SDL_Color{ 120, 200, 160, 255 };
                    else if(rec.delta < 0)
                        dc = SDL_Color{ 225, 130, 130, 255 };
                    else
                        dc = SDL_Color{ 180, 184, 194, 255 };

                    renderer.DrawText(
                        deltaStr,
                        CARD_X + CARD_W - 24 - dw,
                        cy + 6,
                        df,
                        dc
                    );
                }
            }
            else
            {
                renderer.DrawText(deltaStr, CARD_X + CARD_W - 80, cy + 8);
            }


            // ---- 来源文本（小字，灰色） ----

            if(!rec.source.empty() && smallFont)
            {
                int sw = 0, sh = 0;
                TTF_SizeUTF8(smallFont, rec.source.c_str(), &sw, &sh);

                // 太长就截断
                std::string src = rec.source;
                int maxW = CARD_W - 100;

                while(sw > maxW && src.size() > 3)
                {
                    src = src.substr(0, src.size() - 3);
                    src += "...";
                    TTF_SizeUTF8(smallFont, src.c_str(), &sw, &sh);
                }

                renderer.DrawText(
                    src,
                    TEXT_X,
                    cy + CARD_H - sh - 6,
                    smallFont,
                    SDL_Color{ 150, 154, 166, 220 }
                );
            }
        }


        // ---- 页码 ----
        //
        // 原来是纯文本 "[1 / 2]"，和底部提示条挤在一起。
        // 改成居中的胶囊样式：两侧细线 + 中间文字，视觉上像分隔符。

        int totalPages =
            ((int)records.size() + PAGE_SIZE - 1) / PAGE_SIZE;

        if(totalPages < 1) totalPages = 1;

        int currentPage = scrollOffset / PAGE_SIZE + 1;

        std::string pageStr =
            std::to_string(currentPage) + " / " + std::to_string(totalPages);

        TTF_Font* pf = fontManager ? fontManager->GetFont(22) : nullptr;

        if(pf)
        {
            int pw = 0, ph = 0;
            TTF_SizeUTF8(pf, pageStr.c_str(), &pw, &ph);

            int cx = 800;
            int py = 772;        // 卡片底部 738 之下、提示条 838 之上

            // 左右各一段细线
            renderer.DrawFilledRect(
                cx - pw / 2 - 70, py + ph / 2 - 1,
                56, 1,
                140, 144, 154, 110
            );

            renderer.DrawFilledRect(
                cx + pw / 2 + 14, py + ph / 2 - 1,
                56, 1,
                140, 144, 154, 110
            );

            renderer.DrawText(
                pageStr,
                cx - pw / 2,
                py,
                pf,
                SDL_Color{ 190, 194, 204, 235 }
            );
        }
        else
        {
            renderer.DrawText(pageStr, 770, 772);
        }
    }


    // ---- 按键提示 ----
    //
    // 不判断 totalPages：这里已经出了上面对 records 的作用域。
    // 「↑↓ 翻页」在不足一页时按了也只是无效果，留着不影响。
    //
    // y=845：页码在 788，提示条高约 60，两者不重叠。

    if(fontManager)
    {
        DrawKeyHints(
            renderer, *fontManager, 845,
            {
                { "↑↓",  "翻页" },
                { "ESC", "返回" }
            }
        );
    }
}





MenuMouseResult AffectionMenu::HandleMouseClick(int x, int y)
{
    if(x >= UILayout::BACK_X &&
       x <  UILayout::BACK_X + UILayout::BACK_W &&
       y >= UILayout::BACK_Y &&
       y <  UILayout::BACK_Y + UILayout::BACK_H)
    {
        return MenuMouseResult::BACK;
    }

    return MenuMouseResult::NONE;
}