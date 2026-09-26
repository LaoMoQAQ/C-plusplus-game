#ifndef HISTORY_MENU_H
#define HISTORY_MENU_H


#include "../core/Renderer.h"
#include "../story/History.h"

#include "MenuCommon.h"



class HistoryMenu
{


public:


    HistoryMenu();



    void SetHistory(
        History* history
    );



    void Render(
        Renderer& renderer
    );



    void HandleInput(
        int key
    );



    void Reset();



    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );



private:


    History* history;

    int offset;

};



#endif