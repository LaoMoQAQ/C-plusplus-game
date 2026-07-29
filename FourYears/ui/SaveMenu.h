#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include <vector>
#include <string>


#include "../core/Renderer.h"
#include "../core/SaveSystem.h"

#include "../SavePage.h"



class SaveMenu
{

public:


    SaveMenu();



    // 刷新存档列表

    void Refresh(
        SaveSystem& saveSystem
    );



    // 绘制

    void Render(
        Renderer& renderer
    );



    // 输入

    void HandleInput(
        int key
    );



    // 确认

    void Confirm(
        SaveSystem& saveSystem
    );



    // 返回

    bool Back();



    // 获取选择

    int GetChoice() const;



    // 当前页面

    SavePage GetPage() const;



private:


    std::vector<SaveData> saves;



    int choice;



    SavePage page;



    bool back;



};

#endif