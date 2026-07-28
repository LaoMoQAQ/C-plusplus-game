#include "UIManager.h"



UIManager::UIManager()
{

    currentState =
    UIState::START;


}






void UIManager::SetState(

    UIState state

)
{

    currentState = state;

}






UIState UIManager::GetState() const
{

    return currentState;

}








void UIManager::Render(

    Renderer& renderer

)
{


    switch(currentState)
    {


    case UIState::START:

        startMenu.Render(
            renderer
        );

        break;



    case UIState::DIALOGUE:

        dialogueUI.Render(
            renderer
        );

        break;



    case UIState::PAUSE:

        pauseMenu.Render(
            renderer
        );

        break;



    case UIState::SAVE:

        saveMenu.Render(
            renderer
        );

        break;



    case UIState::CONFIG:

        configMenu.Render(
            renderer
        );

        break;



    case UIState::HISTORY:

        historyMenu.Render(
            renderer
        );

        break;


    }



}








void UIManager::HandleInput(

    int key

)
{


    switch(currentState)
    {



    case UIState::START:

        startMenu.HandleInput(
            key
        );

        break;



    case UIState::PAUSE:

        pauseMenu.HandleInput(
            key
        );

        break;



    case UIState::SAVE:

        saveMenu.HandleInput(
            key
        );

        break;



    case UIState::CONFIG:

        configMenu.HandleInput(
            key
        );

        break;



    case UIState::HISTORY:

        historyMenu.HandleInput(
            key
        );

        break;



    default:

        break;


    }



}








DialogueUI& UIManager::GetDialogueUI()
{

    return dialogueUI;

}







StartMenu& UIManager::GetStartMenu()
{

    return startMenu;

}







PauseMenu& UIManager::GetPauseMenu()
{

    return pauseMenu;

}







SaveMenu& UIManager::GetSaveMenu()
{

    return saveMenu;

}







ConfigMenu& UIManager::GetConfigMenu()
{

    return configMenu;

}







HistoryMenu& UIManager::GetHistoryMenu()
{

    return historyMenu;

}