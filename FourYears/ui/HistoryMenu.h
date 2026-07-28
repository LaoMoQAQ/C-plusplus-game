#ifndef HISTORY_MENU_H
#define HISTORY_MENU_H


#include "../core/Renderer.h"
#include "../story/History.h"



class HistoryMenu
{


public:


    HistoryMenu();



    // 设置历史数据
    void SetHistory(
        History* history
    );



    // 绘制历史
    void Render(
        Renderer& renderer
    );



    // 输入滚动
    void HandleInput(
        int key
    );



    // 重置位置
    void Reset();



private:


    History* history;


    int offset;


};



#endif