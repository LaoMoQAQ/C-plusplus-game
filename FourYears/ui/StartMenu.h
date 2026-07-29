#ifndef START_MENU_H
#define START_MENU_H


#include <string>

#include "../core/Renderer.h"


class StartMenu
{

public:

    StartMenu();


    void Render(
        Renderer& renderer
    );


    void HandleInput(
        int key
    );


    int GetChoice() const;


    void Reset();


private:

    int choice;


    std::string items[5];


};


#endif