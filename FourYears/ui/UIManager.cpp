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



    // [修改] HISTORY -> AFFECTION
    case UIState::AFFECTION:

        affectionMenu.Render(
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



    // [修改] AFFECTION 页面没有滚动输入

    default:

        break;


    }



}





void UIManager::HandleMouseMove(
    int x,
    int y
)
{

    switch(currentState)
    {

    case UIState::START:

        startMenu.HandleMouseMove(
            x,
            y
        );

        break;


    case UIState::PAUSE:

        pauseMenu.HandleMouseMove(
            x,
            y
        );

        break;


    case UIState::SAVE:

        saveMenu.HandleMouseMove(
            x,
            y
        );

        break;


    case UIState::CONFIG:

        configMenu.HandleMouseMove(
            x,
            y
        );

        break;


    default:

        break;

    }

}





MenuMouseResult UIManager::HandleMouseClick(
    int x,
    int y
)
{

    switch(currentState)
    {

    case UIState::START:

        return startMenu.HandleMouseClick(
            x,
            y
        );


    case UIState::PAUSE:

        return pauseMenu.HandleMouseClick(
            x,
            y
        );


    case UIState::SAVE:

        return saveMenu.HandleMouseClick(
            x,
            y
        );


    case UIState::CONFIG:

        return configMenu.HandleMouseClick(
            x,
            y
        );


    // [修改] HISTORY -> AFFECTION
    case UIState::AFFECTION:

        return affectionMenu.HandleMouseClick(
            x,
            y
        );


    default:

        return MenuMouseResult::NONE;

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





// [修改] GetHistoryMenu -> GetAffectionMenu
AffectionMenu& UIManager::GetAffectionMenu()
{

    return affectionMenu;

}