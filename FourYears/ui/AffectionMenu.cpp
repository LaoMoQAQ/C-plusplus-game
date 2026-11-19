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
    // 布局：标题居中，下面是表格。
    //
    // 原来序号/角色/数值分散在 500/600/900 三个位置，
    // 看起来零散；页码 [1/2] 在 800，又和底部提示条（约 828）挤在一起。
    //
    // 现在：
    //   序号右对齐到 640，角色左对齐 680，数值右对齐 940
    //   页码移到 792，提示条在 850，两者不再重叠

    int hy = 372;

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

    hy += 78;

    if(records.empty())
    {
        renderer.DrawText("（暂无记录）", 700, hy);
    }
    else
    {
        for(int i = 0; i < PAGE_SIZE; i++)
        {
            int idx = scrollOffset + i;

            if(idx >= (int)records.size())
                break;

            const auto& rec = records[idx];


            // 序号
            std::string idxStr =
                "#" + std::to_string(idx + 1);

            renderer.DrawText(idxStr, 600, hy);


            // 角色
            renderer.DrawText(rec.character, 680, hy);


            // 变化值（带正负号）
            std::string deltaStr;

            if(rec.delta > 0)
                deltaStr = "+" + std::to_string(rec.delta);
            else
                deltaStr = std::to_string(rec.delta);

            renderer.DrawText(deltaStr, 900, hy);


            hy += 44;
        }


        // 页码
        int totalPages =
            ((int)records.size() + PAGE_SIZE - 1) / PAGE_SIZE;

        if(totalPages < 1) totalPages = 1;

        int currentPage = scrollOffset / PAGE_SIZE + 1;

        std::string pageStr =
            "[" + std::to_string(currentPage) +
            " / " + std::to_string(totalPages) + "]";

        // 页码居中，位置在最后一行记录之下、提示条之上。
        // 原来是写死 740（并不居中），y=800 又和提示条（约 828）挤在一起。
        int pw = 0, ph = 0;
        TTF_Font* pf = fontManager ? fontManager->GetFont() : nullptr;

        if(pf)
            TTF_SizeUTF8(pf, pageStr.c_str(), &pw, &ph);

        renderer.DrawText(pageStr, 800 - pw / 2, 788);
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