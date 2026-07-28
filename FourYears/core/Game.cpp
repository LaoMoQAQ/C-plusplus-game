#include "Game.h"


#include <iostream>


#include <SDL_ttf.h>


Game::Game()
{

    running=false;


    window=nullptr;


}







Game::~Game()
{

    Quit();

}








bool Game::Init()
{


    if(
        SDL_Init(
            SDL_INIT_VIDEO
            |
            SDL_INIT_AUDIO
        )
        !=0
    )
    {


        std::cout
        <<
        "SDL初始化失败\n";


        return false;


    }



    if(
        TTF_Init()
        !=0
    )
    {

        std::cout
        <<
        "TTF初始化失败:"
        <<
        TTF_GetError()
        <<
        std::endl;


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







    fontManager.Load(
        "resource/font/simhei.ttf"
    );


    
    renderer.SetFont(
        fontManager.GetFont()
    );



    renderer.SetFont(
        fontManager.GetFont()
    );







    story.Load(
        "script/chapter01.txt"
    );


    // 推进第一句
    story.Next();


    auto event =
    story.GetCurrentEvent();



    ui.GetDialogueUI()
    .SetSpeaker(
        event.name
    );



    ui.GetDialogueUI()
    .SetText(
        event.text
    );



    ui.SetState(
        UIState::DIALOGUE
    );





    ui.SetState(
        UIState::DIALOGUE
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
            event.type
            ==
            SDL_QUIT
        )
        {


            running=false;


        }



        else if(
            event.type
            ==
            SDL_KEYDOWN
        )
        {


            switch(
                event.key.keysym.sym
            )
            {


            case SDLK_ESCAPE:


                ui.SetState(
                    UIState::PAUSE
                );


                break;



            case SDLK_SPACE:


                if(
                    ui.GetState()
                    ==
                    UIState::DIALOGUE
                )
                {


                    story.Next();



                    auto line =
                    story.GetCurrentEvent();



                    ui.GetDialogueUI()
                    .SetSpeaker(
                        line.name
                    );



                    ui.GetDialogueUI()
                    .SetText(
                        line.text
                    );


                }


                break;



            }


        }


    }


}









void Game::Update()
{


}









void Game::Render()
{


    renderer.Clear();



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