#ifndef START_MENU_H
#define START_MENU_H


#include <string>


#include "../core/Renderer.h"



class StartMenu
{


public:


    StartMenu();



    // 绘制菜单
    void Render(
        Renderer& renderer
    );



    // 输入处理
    void HandleInput(
        int key
    );



    // 获取选择结果
    int GetChoice() const;



    // 重置选择
    void Reset();




private:


    int choice;



    std::string items[4];



};



#endif