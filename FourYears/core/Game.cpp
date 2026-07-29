#include "Game.h"

#include <iostream>

#include <SDL_ttf.h>



Game::Game()
{

    running=false;

    window=nullptr;

    currentBackground=nullptr;

    currentCharacter=nullptr;

}




Game::~Game()
{

    Quit();

}








bool Game::Init()
{

    if(
        SDL_Init(
            SDL_INIT_VIDEO |
            SDL_INIT_AUDIO
        )
        !=0
    )
    {

        std::cout
        <<"SDL初始化失败\n";

        return false;

    }




    if(
        TTF_Init()!=0
    )
    {

        std::cout
        <<"TTF初始化失败:"
        <<TTF_GetError()
        <<std::endl;


        return false;

    }





    config.Load(
        "config.ini"
    );





    window =
    SDL_CreateWindow(

        "Four Years",

        SDL_WINDOWPOS_CENTERED,

        SDL_WINDOWPOS_CENTERED,

        config.GetWidth(),

        config.GetHeight(),

        SDL_WINDOW_SHOWN

    );




    if(!window)
    {

        return false;

    }






    if(
        !renderer.Init(
            window
        )
    )
    {

        return false;

    }






    resourceManager.Init();







    fontManager.Load(
        "resource/font/simhei.ttf"
    );


    saveSystem.Init();



    renderer.SetFont(
        fontManager.GetFont()
    );







    story.Load(
        "script/chapter01.txt"
    );







    /*
        预加载资源
    */


    for(auto& e:story.events)
    {


        if(
            !e.background.empty()
        )
        {


            resourceManager.LoadTexture(

                renderer.GetSDLRenderer(),

                "resource/bg/"+e.background

            );


        }





        if(
            !e.character.empty()
        )
        {


            resourceManager.LoadTexture(

                renderer.GetSDLRenderer(),

                "resource/character/"+e.character

            );


        }


    }







    /*
        初始进入开始菜单

    */

    ui.SetState(
        UIState::START
    );





    running=true;



    return true;


}









void Game::Run()
{


    while(running)
    {


        HandleEvents();


        Update();


        Render();



        SDL_Delay(
            16
        );


    }


}









void Game::HandleEvents()
{


    while(
        SDL_PollEvent(
            &event
        )
    )
    {


        if(
            event.type==
            SDL_QUIT
        )
        {

            running=false;

        }





        if(
            event.type==
            SDL_KEYDOWN
        )
        {


            switch(
                event.key.keysym.sym
            )
            {





            case SDLK_UP:

                if(ui.GetState()==UIState::SAVE)
                {
                    ui.GetSaveMenu()
                    .HandleInput(1);
                }
                else
                {
                    ui.HandleInput(1);
                }

                break;





            case SDLK_DOWN:

                if(ui.GetState()==UIState::SAVE)
                {
                    ui.GetSaveMenu()
                    .HandleInput(2);
                }
                else
                {
                    ui.HandleInput(2);
                }

                break;








            case SDLK_RETURN:
            {

                if(ui.GetState()==UIState::SAVE)
                {

                    ui.GetSaveMenu()
                    .Confirm(
                        saveSystem
                    );

                }

                if(
                    ui.GetState()
                    ==
                    UIState::START
                )
                {


                    int choice=
                    ui.GetStartMenu()
                    .GetChoice();




                    switch(choice)
                    {





                    case 0:
                    {

                        story.SetIndex(0);



                        StoryEvent e=
                        story.GetCurrentEvent();




                        UpdateScene();



                        ui.GetDialogueUI()
                        .SetSpeaker(
                            e.name
                        );



                        ui.GetDialogueUI()
                        .SetText(
                            e.text,
                            e.waitTime
                        );



                        ui.SetState(
                            UIState::DIALOGUE
                        );

                    }

                    break;






                    case 1:

                        ui.SetState(
                            UIState::SAVE
                        );

                    break;






                    case 2:

                        ui.SetState(
                            UIState::SAVE
                        );

                    break;






                    case 3:

                        ui.SetState(
                            UIState::CONFIG
                        );

                    break;






                    case 4:

                        running=false;

                    break;


                    }

                }



                else if(
                    ui.GetState()
                    ==
                    UIState::PAUSE
                )
                {

                    int choice=
                    ui.GetPauseMenu()
                    .GetChoice();



                    switch(choice)
                    {


                    case 0:

                        ui.SetState(
                            UIState::DIALOGUE
                        );

                    break;



                    case 1:

                        ui.SetState(
                            UIState::SAVE
                        );

                    break;



                    case 2:

                        ui.SetState(
                            UIState::SAVE
                        );

                    break;



                    case 3:

                        ui.SetState(
                            UIState::HISTORY
                        );

                    break;



                    case 4:

                        ui.SetState(
                            UIState::CONFIG
                        );

                    break;



                    case 5:

                        ui.SetState(
                            UIState::START
                        );

                    break;



                    }

                }



            }

            break;
                        case SDLK_ESCAPE:
            {


                if(
                    ui.GetState()
                    ==
                    UIState::DIALOGUE
                )
                {

                    ui.SetState(
                        UIState::PAUSE
                    );

                }


                else if(
                    ui.GetState()
                    ==
                    UIState::PAUSE
                )
                {

                    ui.SetState(
                        UIState::DIALOGUE
                    );

                }


                else if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    ui.SetState(
                        UIState::START
                    );

                }


                else if(
                    ui.GetState()
                    ==
                    UIState::CONFIG 
                )
                {

                    ui.SetState(
                        UIState::START
                    );

                }
                

            }

            break;







            case SDLK_SPACE:
            {


                if(
                    ui.GetState()
                    ==
                    UIState::DIALOGUE
                )
                {


                    if(
                        !ui.GetDialogueUI()
                        .Finished()
                    )
                    {

                        ui.GetDialogueUI()
                        .Skip();


                    }

                    else
                    {


                        if(
                            story.Next()
                        )
                        {


                            StoryEvent e=
                            story.GetCurrentEvent();



                            UpdateScene();



                            ui.GetDialogueUI()
                            .SetSpeaker(
                                e.name
                            );



                            ui.GetDialogueUI()
                            .SetText(
                                e.text,
                                e.waitTime
                            );


                        }


                    }


                }


                break;

            }





            }


        }


    }


}









void Game::Update()
{


    ui.GetDialogueUI()
    .Update();



}









/*
    更新当前场景资源

    有新的背景:
        替换

    没有:
        保留

*/

void Game::UpdateScene()
{


    StoryEvent e=
    story.GetCurrentEvent();




    if(
        !e.background.empty()
    )
    {


        currentBackground=
        resourceManager.GetTexture(

            "resource/bg/"
            +
            e.background

        );


    }






    if(
        !e.character.empty()
    )
    {


        currentCharacter=
        resourceManager.GetTexture(

            "resource/character/"
            +
            e.character

        );


    }



}









void Game::Render()
{


    renderer.Clear();





    /*
        开始菜单不显示剧情背景

    */

    if(
        ui.GetState()
        !=
        UIState::START
    )
    {



        if(
            currentBackground
        )
        {


            renderer.DrawTexture(

                currentBackground,

                0,

                0

            );


        }





        if(
            currentCharacter
        )
        {


            renderer.DrawTexture(

                currentCharacter,

                520,

                80

            );


        }



    }







    ui.Render(
        renderer
    );



    renderer.Present();


}









void Game::Quit()
{


    running=false;





    if(window)
    {

        SDL_DestroyWindow(
            window
        );


        window=nullptr;

    }





    TTF_Quit();


    SDL_Quit();


}