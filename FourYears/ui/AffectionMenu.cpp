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

    renderer.DrawText("好感度", 740, 70);

    renderer.DrawFilledRect(
        700, 115, 200, 2,
        UILayout::LINE_R,
        UILayout::LINE_G,
        UILayout::LINE_B,
        UILayout::LINE_A
    );



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


        // 名字
        renderer.DrawText(name, 500, y);


        // 进度条背景
        renderer.DrawFilledRoundRect(
            650, y + 6,
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
                650, y + 6,
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

    int hy = 380;

    renderer.DrawText("变化历史", 720, hy);

    renderer.DrawFilledRect(
        660, hy + 45, 280, 2,
        UILayout::LINE_R,
        UILayout::LINE_G,
        UILayout::LINE_B,
        UILayout::LINE_A
    );


    const auto& records = routeManager->GetRecords();

    hy += 85;

    if(records.empty())
    {
        renderer.DrawText("（暂无记录）", 660, hy);
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

            renderer.DrawText(idxStr, 500, hy);


            // 角色
            renderer.DrawText(rec.character, 600, hy);


            // 变化值（带正负号）
            std::string deltaStr;

            if(rec.delta > 0)
                deltaStr = "+" + std::to_string(rec.delta);
            else
                deltaStr = std::to_string(rec.delta);

            renderer.DrawText(deltaStr, 900, hy);


            hy += 45;
        }


        // 页码
        int totalPages =
            ((int)records.size() + PAGE_SIZE - 1) / PAGE_SIZE;

        if(totalPages < 1) totalPages = 1;

        int currentPage = scrollOffset / PAGE_SIZE + 1;

        std::string pageStr =
            "[" + std::to_string(currentPage) +
            " / " + std::to_string(totalPages) + "]";

        renderer.DrawText(pageStr, 740, 800);

        // 提示
        if(totalPages > 1)
        {
            renderer.DrawText(
                "↑ ↓ 翻页",
                720, 840
            );
        }
    }



    // ---- 返回 ----

    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );
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