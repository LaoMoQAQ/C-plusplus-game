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
    AFFECTION
};



class UIManager
{

public:

    UIManager();

    void SetState(UIState state);
    UIState GetState() const;
    bool IsTransitioning() const;

    void Update();

    void Render(Renderer& renderer);

    void HandleInput(int key);
    void HandleMouseMove(int x, int y);
    MenuMouseResult HandleMouseClick(int x, int y);



    DialogueUI&    GetDialogueUI();
    StartMenu&     GetStartMenu();
    PauseMenu&     GetPauseMenu();
    SaveMenu&      GetSaveMenu();
    ConfigMenu&    GetConfigMenu();
    AffectionMenu& GetAffectionMenu();



private:

    UIState currentState;
    UIState pendingState;


    enum class TransState
    {
        NONE,
        OUT,
        IN
    };

    TransState transition = TransState::NONE;

    float alpha = 1.0f;

    // [修改] 0.08 -> 0.18，单程约 0.1 秒
    static constexpr float FADE_SPEED = 0.18f;



    DialogueUI    dialogueUI;
    StartMenu     startMenu;
    PauseMenu     pauseMenu;
    SaveMenu      saveMenu;
    ConfigMenu    configMenu;
    AffectionMenu affectionMenu;

};


#endif