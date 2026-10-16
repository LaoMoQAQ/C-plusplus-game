#ifndef AFFECTION_MENU_H
#define AFFECTION_MENU_H


#include "../core/Renderer.h"
#include "../story/RouteManager.h"

#include "MenuCommon.h"



// ==========================================================
// AffectionMenu
// ==========================================================
//
// 好感度页面。
//
// 上半部分：当前好感度（进度条 + 数值）
// 下半部分：变化历史（↑↓ 翻页）
//
// 数据来源：RouteManager::GetAllAffection() / GetRecords()。
// 每次 Render 都直接拉取，数值永远最新。

class AffectionMenu
{

public:

    AffectionMenu();

    void SetRouteManager(RouteManager* rm);

    void Render(Renderer& renderer);

    void Reset();

    // [新增] ↑↓ 滚动历史
    // key: 1=上 2=下
    void HandleInput(int key);

    // [新增] 鼠标滚轮翻页
    // delta > 0 向下，delta < 0 向上
    void HandleMouseWheel(int delta);

    MenuMouseResult HandleMouseClick(int x, int y);



private:

    RouteManager* routeManager = nullptr;

    // [新增] 历史列表滚动位置（顶部索引）
    int scrollOffset = 0;

    // 每页显示多少条
    static constexpr int PAGE_SIZE = 6;

};


#endif