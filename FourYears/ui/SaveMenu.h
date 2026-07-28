#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include <string>


#include "../core/Renderer.h"
#include "../core/SaveSystem.h"



class SaveMenu
{


public:


    SaveMenu();



    // 绘制存档界面
    void Render(
        Renderer& renderer
    );



    // 输入
    void HandleInput(
        int key
    );



    // 当前选择槽位
    int GetSlot() const;



    // 保存
    bool Save(
        SaveSystem& system,
        SaveData& data
    );



    // 读取
    bool Load(
        SaveSystem& system,
        SaveData& data
    );



private:


    int slot;


};



#endif