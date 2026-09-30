#ifndef UI_MANAGER_H
#define UI_MANAGER_H


#include "DialogueUI.h"
#include "StartMenu.h"
#include "PauseMenu.h"
#include "SaveMenu.h"
#include "ConfigMenu.h"
#include "AffectionMenu.h"

#include "MenuCommon.h"



enum class UIState
{

    START,

    DIALOGUE,

    PAUSE,

    SAVE,

    CONFIG,

    // [修改] HISTORY -> AFFECTION
    AFFECTION

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



    void HandleMouseMove(
        int x,
        int y
    );



    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );




    DialogueUI& GetDialogueUI();


    StartMenu& GetStartMenu();

    PauseMenu& GetPauseMenu();

    SaveMenu& GetSaveMenu();

    ConfigMenu& GetConfigMenu();

    // [修改] GetHistoryMenu -> GetAffectionMenu
    AffectionMenu& GetAffectionMenu();




private:


    UIState currentState;



    DialogueUI dialogueUI;

    StartMenu startMenu;

    PauseMenu pauseMenu;

    SaveMenu saveMenu;

    ConfigMenu configMenu;

    // [修改] historyMenu -> affectionMenu
    AffectionMenu affectionMenu;

};



#endif