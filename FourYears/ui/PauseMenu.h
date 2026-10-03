#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H


#include <string>

#include "../core/Renderer.h"

#include "MenuCommon.h"



class PauseMenu
{

public:

    PauseMenu();

    void Render(Renderer& renderer);
    void HandleInput(int key);
    void HandleMouseMove(int x, int y);
    MenuMouseResult HandleMouseClick(int x, int y);

    int GetChoice() const;
    void Reset();

    void Update();



private:

    static constexpr int ITEM_COUNT = 6;

    int choice;

    std::string items[ITEM_COUNT];


    static constexpr int MENU_X   = 500;
    static constexpr int MENU_Y   = 230;
    static constexpr int MENU_GAP = 55;
    static constexpr int ITEM_W   = 400;
    static constexpr int ITEM_H   = 50;


    float displayHighlightY = 230.0f;

    // [新增] 双速度
    static constexpr float HL_ANIM_SPEED      = 0.25f;
    static constexpr float HL_ANIM_SPEED_FAST = 0.55f;

    float hlSpeed = HL_ANIM_SPEED;

};


#endif