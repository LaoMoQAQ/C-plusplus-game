#ifndef UI_MANAGER_H
#define UI_MANAGER_H


#include "DialogueUI.h"
#include "StartMenu.h"
#include "PauseMenu.h"
#include "SaveMenu.h"
#include "ConfigMenu.h"
#include "HistoryMenu.h"



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



    // 切换界面
    void SetState(
        UIState state
    );



    // 获取当前界面
    UIState GetState() const;



    // 绘制
    void Render(
        Renderer& renderer
    );



    // 输入
    void HandleInput(
        int key
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