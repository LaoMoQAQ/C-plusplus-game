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

    resourceManager.Init();







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

    for(auto& e : story.events)
    {
        if(!e.background.empty())
        {
            resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                "resource/bg/" + e.background
            );
        }

        if(!e.character.empty())
        {
            resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                "resource/character/" + e.character
            );
        }
    }

    // 不要立即进入剧情
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


            switch(event.key.keysym.sym)
            {
            case SDLK_UP:

                ui.HandleInput(1);

                break;

            case SDLK_DOWN:

                ui.HandleInput(2);

            break;

            case SDLK_RETURN:

                if(ui.GetState()==UIState::START)
                {
                    if(ui.GetStartMenu().GetChoice()==0)
                    {
                        story.SetIndex(0);

                        StoryEvent e=
                            story.GetCurrentEvent();

                        ui.GetDialogueUI().SetSpeaker(
                            e.name
                        );

                        ui.GetDialogueUI().SetText(
                            e.text,
                            e.waitTime
                        );

                        ui.SetState(
                            UIState::DIALOGUE
                        );
                    }
                }

                break;

            case SDLK_ESCAPE:

                if(ui.GetState()==UIState::DIALOGUE)
                {
                    ui.SetState(
                        UIState::PAUSE
                    );
                }
                else if(ui.GetState()==UIState::PAUSE)
                {
                    ui.SetState(
                        UIState::DIALOGUE
                    );
                }

                break;

            case SDLK_SPACE:

                if(ui.GetState()==UIState::DIALOGUE)
                {
                    if(!ui.GetDialogueUI().Finished())
                    {
                        ui.GetDialogueUI().Skip();
                    }
                    else
                    {
                        story.Next();

                        StoryEvent e=
                            story.GetCurrentEvent();

                        ui.GetDialogueUI().SetSpeaker(
                            e.name
                        );

                        ui.GetDialogueUI().SetText(
                            e.text,
                            e.waitTime
                        );
                    }
                }

                break;
            }


        }


    }


}









void Game::Update()
{

    ui.GetDialogueUI().Update();
    
}









void Game::Render()
{


    renderer.Clear();

    if(
        ui.GetState()!=UIState::START
    )
    {
        StoryEvent e=
        story.GetCurrentEvent();

        SDL_Texture* bg=
        resourceManager.GetTexture(
            "resource/bg/"+e.background
        );

        renderer.DrawTexture(
            bg,
            0,
            0
        );

        SDL_Texture* ch=
        resourceManager.GetTexture(
            "resource/character/"+e.character
        );

        renderer.DrawTexture(
            ch,
            520,
            80
        );
    }

    ui.Render(renderer);

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