#ifndef START_MENU_H
#define START_MENU_H


#include <string>

#include "../core/Renderer.h"
#include "../core/FontManager.h"

#include "MenuCommon.h"


class StartMenu
{

public:

    StartMenu();


    void SetFontManager(
        FontManager* fm
    );


    void Render(
        Renderer& renderer
    );


    void HandleInput(
        int key
    );


    void HandleMouseMove(
        int x,
        int y
    );


    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );


    int GetChoice() const;

    void Reset();

private:

    int choice;


    // 主菜单 4 项：
    //   0 开始游戏
    //   1 存档      ← 合并了原来的"存档管理"+"读取存档"
    //   2 设置
    //   3 退出游戏
    static constexpr int ITEM_COUNT = 4;

    std::string items[ITEM_COUNT];


    FontManager* fontManager = nullptr;


    static constexpr int MENU_X   = 1360;
    static constexpr int MENU_Y   = 400;
    static constexpr int MENU_GAP = 75;
    static constexpr int ITEM_W   = 220;
    static constexpr int ITEM_H   = 60;

};


#endif