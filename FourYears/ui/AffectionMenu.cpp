#include "AffectionMenu.h"

#include <map>
#include <string>




AffectionMenu::AffectionMenu()
{
}





void AffectionMenu::SetRouteManager(RouteManager* rm)
{
    routeManager = rm;
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

    renderer.DrawText("好感度", 720, 100);

    renderer.DrawFilledRect(
        700, 150, 200, 2,
        UILayout::LINE_R,
        UILayout::LINE_G,
        UILayout::LINE_B,
        UILayout::LINE_A
    );



    if(routeManager == nullptr)
    {
        renderer.DrawText("暂无数据", 700, 250);
        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );
        return;
    }



    auto affection = routeManager->GetAllAffection();

    if(affection.empty())
    {
        renderer.DrawText("暂无好感度记录", 660, 250);
        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );
        return;
    }



    int y = 250;

    for(auto& pair : affection)
    {
        std::string line =
            pair.first + "    " + std::to_string(pair.second);

        renderer.DrawText(line, 680, y);

        y += 60;
    }



    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );
}





void AffectionMenu::Reset()
{
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