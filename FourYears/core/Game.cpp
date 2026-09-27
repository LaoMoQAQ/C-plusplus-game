#include "Game.h"

#include <iostream>

#include <ctime>

#include <SDL_ttf.h>

#include <SDL_syswm.h>

#include <windows.h>



static constexpr int CHARACTER_HEIGHT   = 700;

static constexpr int CHARACTER_CENTER_X = 1220;



// ==========================================================
// [新增] 从脚本路径推算章节显示名
// ==========================================================
//
// 存档列表里显示的 [第X章]，
// 不再用固定数字，而是根据脚本路径判断。
//
// 需要新增章节时，在这里加一条。

static std::string ChapterNameFromScript(
    const std::string& script
)
{
    if(script.find("chapter01") != std::string::npos)
        return "第1章";

    if(script.find("chapter02") != std::string::npos)
        return "第2章";

    if(script.find("alone_ending") != std::string::npos)
        return "个人结局";

    if(script.find("li_junhao_route") != std::string::npos)
        return "李君浩线";

    if(script.find("zhang_hanyu_route") != std::string::npos)
        return "张瀚宇线";

    if(script.find("ending") != std::string::npos)
        return "结局";

    return "未知";
}

// ==========================================================



Game::Game()
{

    running=false;

    window=nullptr;

    currentBackground=nullptr;

    currentCharacter=nullptr;


    lastState=
    UIState::START;

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






    SDL_SetHint(
        SDL_HINT_IME_SHOW_UI,
        "0"
    );

    SDL_StopTextInput();

    SDL_EventState(
        SDL_TEXTINPUT,
        SDL_DISABLE
    );

    SDL_EventState(
        SDL_TEXTEDITING,
        SDL_DISABLE
    );


#ifdef _WIN32

    {
        SDL_SysWMinfo wmInfo;

        SDL_VERSION(
            &wmInfo.version
        );


        if(
            SDL_GetWindowWMInfo(
                window,
                &wmInfo
            )
        )
        {

            HWND hwnd =
                wmInfo.info.win.window;


            HMODULE hImm =
                LoadLibraryA(
                    "imm32.dll"
                );


            if(hImm)
            {

                typedef HIMC (WINAPI *PFN_ImmAssociateContext)(
                    HWND,
                    HIMC
                );


                PFN_ImmAssociateContext
                    pImmAssociateContext =
                    (PFN_ImmAssociateContext)
                    GetProcAddress(
                        hImm,
                        "ImmAssociateContext"
                    );


                if(pImmAssociateContext)
                {

                    pImmAssociateContext(
                        hwnd,
                        NULL
                    );

                }

            }

        }

    }

#endif






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






    resourceManager.LoadTexture(

        renderer.GetSDLRenderer(),

        "resource/bg/main_menu.png"

    );



    ui.GetStartMenu().SetFontManager(
        &fontManager
    );



    ui.GetConfigMenu().SetConfig(
        &config
    );



    ui.GetHistoryMenu().SetHistory(
        &story.GetHistory()
    );






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
            event.type ==
            SDL_QUIT
        )
        {

            running=false;

        }





        if(
            event.type ==
            SDL_KEYDOWN
        )
        {

            // ==========================================================
            // [新增] 重命名输入模式优先拦截
            // ==========================================================
            //
            // 处于重命名模式时，所有按键都交给 SaveMenu 处理，
            // 不进入下面的正常分支。
            // 这样上下方向键不会切换存档选项，
            // 字母键也能作为文本输入。

            if(
                ui.GetState()
                ==
                UIState::SAVE
                &&
                ui.GetSaveMenu()
                .IsRenaming()
            )
            {

                ui.GetSaveMenu()
                .HandleRenameKey(
                    event.key.keysym.sym,
                    saveSystem
                );

                continue;

            }

            // ==========================================================


            switch(
                event.key.keysym.sym
            )
            {


            case SDLK_UP:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    ui.GetSaveMenu()
                    .HandleInput(1);

                }
                else if(
                    ui.GetState()
                    ==
                    UIState::DIALOGUE
                    &&
                    ui.GetDialogueUI()
                    .HasChoice()
                )
                {

                    ui.GetDialogueUI()
                    .MoveChoice(-1);

                }
                else
                {

                    ui.HandleInput(1);

                }

            }
            break;





            case SDLK_DOWN:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    ui.GetSaveMenu()
                    .HandleInput(2);

                }
                else if(
                    ui.GetState()
                    ==
                    UIState::DIALOGUE
                    &&
                    ui.GetDialogueUI()
                    .HasChoice()
                )
                {

                    ui.GetDialogueUI()
                    .MoveChoice(1);

                }
                else
                {

                    ui.HandleInput(2);

                }

            }
            break;





            // 新建存档
            case SDLK_n:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    // [修改] 加上章节显示名
                    ui.GetSaveMenu()
                    .Create(
                        saveSystem,
                        "新的存档",
                        story.GetCurrentFile(),
                        ChapterNameFromScript(
                            story.GetCurrentFile()
                        ),
                        story.GetIndex()
                    );

                }

            }
            break;





            case SDLK_c:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    ui.GetSaveMenu()
                    .Copy(
                        saveSystem
                    );

                }

            }
            break;





            case SDLK_DELETE:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    if(
                        ui.GetSaveMenu()
                        .IsConfirmingDelete()
                    )
                    {

                        ui.GetSaveMenu()
                        .ConfirmDelete(
                            saveSystem
                        );

                    }
                    else
                    {

                        ui.GetSaveMenu()
                        .BeginDelete();

                    }

                }

            }
            break;





            // 重命名存档
            case SDLK_r:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    // [修改] 不再自动生成名字，
                    // 进入输入模式让用户自己敲。
                    ui.GetSaveMenu()
                    .BeginRename();

                }

            }
            break;





            case SDLK_RETURN:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::DIALOGUE
                    &&
                    ui.GetDialogueUI()
                    .HasChoice()
                )
                {

                    ConfirmChoice();

                }
                else if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                    &&
                    ui.GetSaveMenu()
                    .IsConfirmingDelete()
                )
                {

                    ui.GetSaveMenu()
                    .ConfirmDelete(
                        saveSystem
                    );

                }
                else
                {

                    OnActivateCurrentState();

                }

            }
            break;





            case SDLK_ESCAPE:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                    &&
                    ui.GetSaveMenu()
                    .IsConfirmingDelete()
                )
                {

                    ui.GetSaveMenu()
                    .CancelDelete();

                }
                else
                {

                    OnBack();

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

                    OnAdvanceDialogue();

                }

            }
            break;



            }

        }





        if(
            event.type ==
            SDL_MOUSEMOTION
        )
        {

            ui.HandleMouseMove(

                event.motion.x,

                event.motion.y

            );

        }





        if(
            event.type ==
            SDL_MOUSEBUTTONDOWN
            &&
            event.button.button ==
            SDL_BUTTON_LEFT
        )
        {

            int mx = event.button.x;

            int my = event.button.y;



            if(
                ui.GetState()
                ==
                UIState::DIALOGUE
            )
            {

                if(
                    !ui.GetDialogueUI()
                    .HasChoice()
                )
                {
                    OnAdvanceDialogue();
                }

            }
            else
            {

                MenuMouseResult r =
                    ui.HandleMouseClick(
                        mx,
                        my
                    );



                if(
                    r ==
                    MenuMouseResult::ACTIVATE
                )
                {

                    if(
                        ui.GetState()
                        ==
                        UIState::SAVE
                        &&
                        ui.GetSaveMenu()
                        .IsConfirmingDelete()
                    )
                    {

                        ui.GetSaveMenu()
                        .ConfirmDelete(
                            saveSystem
                        );

                    }
                    else
                    {

                        OnActivateCurrentState();

                    }

                }

                else if(
                    r ==
                    MenuMouseResult::BACK
                )
                {

                    OnBack();

                }

            }

        }


    }

}





void Game::UpdateScene()
{
    StoryEvent e = story.GetCurrentEvent();


    if(!e.background.empty())
    {
        std::string path =
            "resource/bg/" + e.background;

        SDL_Texture* tex =
            resourceManager.GetTexture(path);

        if(!tex)
        {
            tex = resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                path
            );
        }

        if(tex)
        {
            currentBackground = tex;
        }
    }


    if(!e.character.empty())
    {
        std::string path =
            "resource/character/" + e.character;

        SDL_Texture* tex =
            resourceManager.GetTexture(path);

        if(!tex)
        {
            tex = resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                path
            );
        }

        if(tex)
        {
            currentCharacter = tex;
        }
    }
}











void Game::Render()
{
    renderer.Clear();


    UIState st = ui.GetState();



    bool useGameBackground = false;

    if(
        st == UIState::DIALOGUE ||
        st == UIState::PAUSE
    )
    {
        useGameBackground = true;
    }
    else if(
        st == UIState::SAVE ||
        st == UIState::CONFIG ||
        st == UIState::HISTORY
    )
    {
        useGameBackground =
            (lastState == UIState::PAUSE);
    }



    bool needBlur =

        st == UIState::PAUSE
        ||
        st == UIState::SAVE
        ||
        st == UIState::CONFIG
        ||
        st == UIState::HISTORY;



    SDL_Texture* menuBg =
        resourceManager.GetTexture(
            "resource/bg/main_menu.png"
        );


    SDL_Texture* bgToDraw = nullptr;


    if(useGameBackground)
    {
        bgToDraw = currentBackground
                   ? currentBackground
                   : menuBg;
    }
    else
    {
        bgToDraw = menuBg;
    }



    if(bgToDraw)
    {
        if(needBlur)
        {
            renderer.DrawBlurTexture(
                bgToDraw,
                0,
                0
            );
        }
        else
        {
            renderer.DrawTexture(
                bgToDraw,
                0,
                0
            );
        }
    }



    if(
        st == UIState::DIALOGUE &&
        currentCharacter
    )
    {

        int texW = 0;
        int texH = 0;

        SDL_QueryTexture(
            currentCharacter,
            nullptr,
            nullptr,
            &texW,
            &texH
        );


        if(texW > 0 && texH > 0)
        {

            int drawH = CHARACTER_HEIGHT;

            int drawW =
                texW * drawH / texH;


            int drawX =
                CHARACTER_CENTER_X
                - drawW / 2;


            int drawY =
                (config.GetHeight() - drawH) / 2;


            renderer.DrawTexture(
                currentCharacter,
                drawX,
                drawY,
                drawW,
                drawH
            );

        }

    }



    ui.Render(renderer);

    renderer.Present();
}




void Game::Update()
{

    ui.GetDialogueUI()
    .Update();


}




void Game::Quit()
{


    running=false;





    if(
        window
    )
    {


        SDL_DestroyWindow(
            window
        );

        window=nullptr;

    }





    TTF_Quit();

    SDL_Quit();

}





// ==========================================================
// 激活当前状态的菜单项
// ==========================================================

void Game::OnActivateCurrentState()
{

    // 存档界面

    if(
        ui.GetState()
        ==
        UIState::SAVE
    )
    {

        std::string scriptFile;
        std::string chapterName;

        int idx = 0;


        if(
            ui.GetSaveMenu()
            .Confirm(
                saveSystem,
                scriptFile,
                chapterName,
                idx
            )
        )
        {

            if(
                !scriptFile.empty()
                &&
                story.Load(scriptFile)
            )
            {

                if(
                    idx >= 0
                    &&
                    idx <
                    (int)story.events.size()
                )
                {

                    story.SetIndex(
                        idx
                    );


                    UpdateScene();


                    StoryEvent e =
                        story
                        .GetCurrentEvent();


                    ui.GetDialogueUI()
                    .SetSpeaker(
                        e.name
                    );


                    ui.GetDialogueUI()
                    .SetText(
                        e.text,
                        e.waitTime
                    );



                    if(e.isChoice)
                    {
                        ui.GetDialogueUI()
                        .ShowChoice(
                            e.choices
                        );

                        currentChoiceTargets =
                            e.choiceTargets;
                    }
                    else
                    {
                        ui.GetDialogueUI()
                        .ClearChoice();

                        currentChoiceTargets.clear();
                    }


                    ui.SetState(
                        UIState::DIALOGUE
                    );

                }

            }

        }

    }





    // 开始菜单

    else if(
        ui.GetState()
        ==
        UIState::START
    )
    {


        int choice =
        ui.GetStartMenu()
        .GetChoice();



        switch(choice)
        {


        case 0:
        {

            story.Load(
                "script/chapter01.txt"
            );



            ui.GetDialogueUI()
            .ClearChoice();

            currentChoiceTargets.clear();



            StoryEvent e =
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
        {

            lastState =
                UIState::START;


            ui.GetSaveMenu()
            .SetPage(
                SavePage::LOAD
            );


            ui.GetSaveMenu()
            .Refresh(
                saveSystem
            );


            ui.SetState(
                UIState::SAVE
            );

        }
        break;





        case 2:
        {

            lastState =
                UIState::START;


            ui.SetState(
                UIState::CONFIG
            );

        }
        break;





        case 3:
        {

            running=false;

        }
        break;


        }

    }





    // 暂停菜单

    else if(
        ui.GetState()
        ==
        UIState::PAUSE
    )
    {


        int choice =
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
        {

            lastState =
                UIState::PAUSE;


            ui.GetSaveMenu()
            .SetPage(
                SavePage::MANAGE
            );


            ui.GetSaveMenu()
            .Refresh(
                saveSystem
            );


            ui.SetState(
                UIState::SAVE
            );

        }
        break;



        case 2:
        {

            lastState =
                UIState::PAUSE;


            ui.GetSaveMenu()
            .SetPage(
                SavePage::LOAD
            );


            ui.GetSaveMenu()
            .Refresh(
                saveSystem
            );


            ui.SetState(
                UIState::SAVE
            );

        }
        break;



        case 3:
        {

            lastState =
                UIState::PAUSE;


            ui.SetState(
                UIState::HISTORY
            );

        }
        break;



        case 4:
        {

            lastState =
                UIState::PAUSE;


            ui.SetState(
                UIState::CONFIG
            );

        }
        break;



        case 5:

            ui.SetState(
                UIState::START
            );

        break;

        }

    }





    else if(
        ui.GetState()
        ==
        UIState::CONFIG
    )
    {

        if(
            ui.GetConfigMenu()
            .GetChoice()
            ==
            5
        )
        {

            ui.GetConfigMenu()
            .Save();

        }

    }

}





// ==========================================================
// 推进对话
// ==========================================================

void Game::OnAdvanceDialogue()
{

    if(
        ui.GetDialogueUI()
        .HasChoice()
    )
    {
        return;
    }



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

            StoryEvent e =
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



            if(e.isChoice)
            {

                ui.GetDialogueUI()
                .ShowChoice(
                    e.choices
                );


                currentChoiceTargets =
                    e.choiceTargets;

            }

        }


    }

}





// ==========================================================
// 返回上一级
// ==========================================================

void Game::OnBack()
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
            lastState
        );

    }


    else if(
        ui.GetState()
        ==
        UIState::CONFIG
    )
    {

        ui.SetState(
            lastState
        );

    }


    else if(
        ui.GetState()
        ==
        UIState::HISTORY
    )
    {

        ui.SetState(
            lastState
        );

    }

}





// ==========================================================
// 确认选择
// ==========================================================

void Game::ConfirmChoice()
{

    int idx =
        ui.GetDialogueUI()
        .GetChoiceIndex();



    if(
        idx < 0
        ||
        idx >=
        (int)currentChoiceTargets.size()
    )
    {
        return;
    }



    std::string target =
        currentChoiceTargets[idx];



    ui.GetDialogueUI()
    .ClearChoice();

    currentChoiceTargets.clear();



    if(target.empty())
    {
        return;
    }



    if(story.Load(target))
    {

        StoryEvent e =
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


        if(e.isChoice)
        {

            ui.GetDialogueUI()
            .ShowChoice(
                e.choices
            );


            currentChoiceTargets =
                e.choiceTargets;

        }

    }

}