#include "AffectionMenu.h"


#include <map>
#include <string>




AffectionMenu::AffectionMenu()
{

}





void AffectionMenu::SetRouteManager(
    RouteManager* rm
)
{

    routeManager = rm;

}





void AffectionMenu::Render(
    Renderer& renderer
)
{

    renderer.DrawText(
        "好感度",
        500,
        100
    );



    if(routeManager == nullptr)
    {

        renderer.DrawText(
            "暂无数据",
            500,
            220
        );

        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );

        return;

    }



    auto affection =
        routeManager->GetAllAffection();



    if(affection.empty())
    {

        renderer.DrawText(
            "暂无好感度记录",
            500,
            220
        );

        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );

        return;

    }



    int y = 220;


    for(auto& pair : affection)
    {

        std::string line =
            pair.first
            +
            "    "
            +
            std::to_string(pair.second);


        renderer.DrawText(
            line,
            500,
            y
        );


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

    // 好感度页面没有滚动，
    // 留空保持接口一致。

}





MenuMouseResult AffectionMenu::HandleMouseClick(
    int x,
    int y
)
{

    if(
        x >= UILayout::BACK_X &&
        x <  UILayout::BACK_X + UILayout::BACK_W &&
        y >= UILayout::BACK_Y &&
        y <  UILayout::BACK_Y + UILayout::BACK_H
    )
    {
        return MenuMouseResult::BACK;
    }


    return MenuMouseResult::NONE;

}