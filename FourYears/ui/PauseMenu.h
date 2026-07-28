#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H


#include <string>


#include "../core/Renderer.h"



class PauseMenu
{


public:


    PauseMenu();



    // 绘制暂停菜单
    void Render(
        Renderer& renderer
    );



    // 输入处理
    void HandleInput(
        int key
    );



    // 获取选择
    int GetChoice() const;



    // 重置
    void Reset();



private:


    int choice;


    std::string items[6];


};



#endif