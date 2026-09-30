#ifndef AFFECTION_MENU_H
#define AFFECTION_MENU_H


#include "../core/Renderer.h"
#include "../story/RouteManager.h"

#include "MenuCommon.h"


// ==========================================================
// 好感度页面
// ==========================================================
//
// 替代原来的"历史记录"页面。
// 显示当前所有角色的好感度数值。
//
// 数据来源：Story::GetRouteManager()
// 每次进入页面时从 RouteManager 拉取最新数据。

class AffectionMenu
{

public:

    AffectionMenu();


    // 绑定路线管理器（不拥有）
    void SetRouteManager(
        RouteManager* rm
    );


    void Render(
        Renderer& renderer
    );


    // 好感度页面没有滚动需求，
    // 只处理"返回"按钮，所以不需要 HandleInput。
    // 保留一个空函数以保持接口一致。

    void Reset();


    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );


private:

    RouteManager* routeManager = nullptr;

};


#endif