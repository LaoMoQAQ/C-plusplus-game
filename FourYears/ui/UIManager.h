#ifndef UI_MANAGER_H
#define UI_MANAGER_H


#include "DialogueUI.h"
#include "StartMenu.h"
#include "PauseMenu.h"
#include "SaveMenu.h"
#include "ConfigMenu.h"
#include "HistoryMenu.h"

#include "MenuCommon.h"



enum class UIState
{

    START,

    DIALOGUE,

    PAUSE,

    SAVE,

    CONFIG,

    HISTORY

};





class UIManager
{


public:


    UIManager();



    void SetState(
        UIState state
    );


    UIState GetState() const;


    void Render(
        Renderer& renderer
    );


    void HandleInput(
        int key
    );



    // 鼠标移动分发到当前状态的菜单
    void HandleMouseMove(
        int x,
        int y
    );



    // 鼠标点击分发。
    // 返回值告诉 Game 该做什么。
    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );




    DialogueUI& GetDialogueUI();


    StartMenu& GetStartMenu();

    PauseMenu& GetPauseMenu();

    SaveMenu& GetSaveMenu();

    ConfigMenu& GetConfigMenu();

    HistoryMenu& GetHistoryMenu();




private:


    UIState currentState;



    DialogueUI dialogueUI;

    StartMenu startMenu;

    PauseMenu pauseMenu;

    SaveMenu saveMenu;

    ConfigMenu configMenu;

    HistoryMenu historyMenu;

};



#endif