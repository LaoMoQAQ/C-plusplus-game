#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H


#include "../core/Renderer.h"
#include "../core/Config.h"

#include "MenuCommon.h"



class ConfigMenu
{


public:


    ConfigMenu();



    void SetConfig(
        Config* config
    );



    void Render(
        Renderer& renderer
    );



    void HandleInput(
        int key
    );



    void Save();



    int GetChoice() const;



    void HandleMouseMove(
        int x,
        int y
    );


    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );



private:


    Config* config;

    int choice;

    static constexpr int ITEM_COUNT = 6;

    std::string items[ITEM_COUNT];


    static constexpr int MENU_X   = 420;
    static constexpr int MENU_Y   = 180;
    static constexpr int MENU_GAP = 55;
    static constexpr int ITEM_W   = 700;
    static constexpr int ITEM_H   = 50;

};



#endif