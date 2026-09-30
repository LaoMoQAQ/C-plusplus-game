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



    int GetChoice() const;



    void HandleMouseMove(
        int x,
        int y
    );


    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );



    void Save();



    bool TryExit();

    void CancelConfirm();

    bool IsConfirming() const;


    // ==========================================================
    // [修改] 返回值改为 int
    // ==========================================================
    //
    // -1 : 取消询问，留在设置页
    //  0 : 返回，未保存（调用方需还原 Config）
    //  1 : 返回，已保存

    int HandleConfirmKey(
        int sym
    );



private:


    Config* config;

    int choice;


    static constexpr int ITEM_COUNT = 5;

    std::string items[ITEM_COUNT];


    static constexpr int MENU_X   = 420;
    static constexpr int MENU_Y   = 180;
    static constexpr int MENU_GAP = 55;
    static constexpr int ITEM_W   = 700;
    static constexpr int ITEM_H   = 50;


    bool dirty = false;

    bool confirmSave = false;

};



#endif