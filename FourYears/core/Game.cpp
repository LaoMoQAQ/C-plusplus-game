#include "Game.h"

#include <iostream>

#include <ctime>

#include <SDL_ttf.h>

#include <SDL_syswm.h>

#include <windows.h>



static constexpr int CHARACTER_HEIGHT   = 700;

static constexpr int CHARACTER_CENTER_X = 1220;



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






    SDL_RenderSetLogicalSize(
        renderer.GetSDLRenderer(),
        config.GetWidth(),
        config.GetHeight()
    );






    resourceManager.Init();






    audioManager.Init();






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



    ui.GetAffectionMenu().SetRouteManager(
        &story.GetRouteManager()
    );






    lastBgmVolume = config.GetBGMVolume();
    lastSeVolume  = config.GetSEVolume();
    lastTextSpeed = config.GetTextSpeed();
    lastFullscreen = config.IsFullscreen();


    audioManager.SetBGMVolume(lastBgmVolume);
    audioManager.SetSEVolume(lastSeVolume);

    ui.GetDialogueUI().SetTextSpeed(lastTextSpeed);


    if(lastFullscreen)
    {
        SDL_SetWindowFullscreen(
            window,
            SDL_WINDOW_FULLSCREEN_DESKTOP
        );
    }






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

            // ----------------------------------------------------------
            // [修改] 设置询问状态拦截
            // ----------------------------------------------------------
            //
            // HandleConfirmKey 现在返回 int：
            //  -1 留在设置
            //   0 返回，未保存（还原 Config）
            //   1 返回，已保存

            if(
                ui.GetState()
                ==
                UIState::CONFIG
                &&
                ui.GetConfigMenu()
                .IsConfirming()
            )
            {

                int r =
                    ui.GetConfigMenu()
                    .HandleConfirmKey(
                        event.key.keysym.sym
                    );


                if(r == 1)
                {
                    // 已保存，直接返回
                    OnBack();
                }
                else if(r == 0)
                {
                    // 未保存，还原 Config，再返回
                    config = configBackup;

                    OnBack();
                }

                // r == -1 继续留在设置

                continue;

            }

            // ----------------------------------------------------------


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





            case SDLK_LEFT:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::CONFIG
                )
                {
                    ui.HandleInput(3);
                }

            }
            break;





            case SDLK_RIGHT:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::CONFIG
                )
                {
                    ui.HandleInput(4);
                }

            }
            break;





            case SDLK_s:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::CONFIG
                    &&
                    !ui.GetConfigMenu()
                    .IsConfirming()
                )
                {
                    ui.GetConfigMenu()
                    .Save();
                }

            }
            break;





            case SDLK_n:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

                    ui.GetSaveMenu()
                    .Create(
                        saveSystem,
                        "新的存档",
                        story.GetCurrentFile(),
                        ChapterNameFromScript(
                            story.GetCurrentFile()
                        ),
                        story.GetIndex(),
                        story.GetRouteManager()
                             .GetAllAffection()
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





            case SDLK_r:
            {

                if(
                    ui.GetState()
                    ==
                    UIState::SAVE
                )
                {

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
                else if(
                    ui.GetState()
                    ==
                    UIState::CONFIG
                )
                {

                    if(
                        ui.GetConfigMenu()
                        .TryExit()
                    )
                    {
                        OnBack();
                    }

                    // false 等待玩家按 Y / N / ESC

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

            if(
                ui.GetState()
                ==
                UIState::DIALOGUE
                &&
                ui.GetDialogueUI()
                .HasChoice()
            )
            {

                int idx =
                    ui.GetDialogueUI()
                    .HitTestChoice(
                        event.motion.x,
                        event.motion.y
                    );


                if(idx >= 0)
                {
                    ui.GetDialogueUI()
                    .SetChoiceIndex(idx);
                }

            }
            else
            {

                ui.HandleMouseMove(

                    event.motion.x,

                    event.motion.y

                );

            }

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
                    ui.GetDialogueUI()
                    .HasChoice()
                )
                {

                    int idx =
                        ui.GetDialogueUI()
                        .HitTestChoice(
                            mx,
                            my
                        );


                    if(idx >= 0)
                    {

                        ui.GetDialogueUI()
                        .SetChoiceIndex(idx);

                        ConfirmChoice();

                    }

                }
                else
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

                    if(
                        ui.GetState()
                        ==
                        UIState::CONFIG
                    )
                    {

                        if(
                            ui.GetConfigMenu()
                            .TryExit()
                        )
                        {
                            OnBack();
                        }

                    }
                    else
                    {
                        OnBack();
                    }

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
        st == UIState::AFFECTION
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
        st == UIState::AFFECTION;



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



    if(config.GetBGMVolume() != lastBgmVolume)
    {
        lastBgmVolume = config.GetBGMVolume();

        audioManager.SetBGMVolume(
            lastBgmVolume
        );
    }


    if(config.GetSEVolume() != lastSeVolume)
    {
        lastSeVolume = config.GetSEVolume();

        audioManager.SetSEVolume(
            lastSeVolume
        );
    }


    if(config.GetTextSpeed() != lastTextSpeed)
    {
        lastTextSpeed = config.GetTextSpeed();

        ui.GetDialogueUI()
        .SetTextSpeed(
            lastTextSpeed
        );
    }


    if(config.IsFullscreen() != lastFullscreen)
    {
        lastFullscreen = config.IsFullscreen();


        SDL_SetWindowFullscreen(
            window,
            lastFullscreen
                ? SDL_WINDOW_FULLSCREEN_DESKTOP
                : 0
        );
    }



    if(
        config.IsAutoPlay() &&
        ui.GetState() == UIState::DIALOGUE &&
        !ui.GetDialogueUI().HasChoice() &&
        ui.GetDialogueUI().Finished()
    )
    {

        autoPlayTimer += 0.016f;


        if(autoPlayTimer >= AUTO_PLAY_DELAY)
        {

            autoPlayTimer = 0.0f;

            OnAdvanceDialogue();

        }

    }
    else
    {

        autoPlayTimer = 0.0f;

    }

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

        std::map<std::string, int> aff;


        if(
            ui.GetSaveMenu()
            .Confirm(
                saveSystem,
                scriptFile,
                chapterName,
                idx,
                aff
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

                    story.GetRouteManager()
                         .SetAllAffection(aff);


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

                        currentChoiceAffection =
                            e.choiceAffection;
                    }
                    else
                    {
                        ui.GetDialogueUI()
                        .ClearChoice();

                        currentChoiceTargets.clear();

                        currentChoiceAffection.clear();
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

            currentChoiceAffection.clear();



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





        // ==========================================================
        // 进入设置时：
        //   1. 保存 Config 快照
        //   2. 重置 ConfigMenu 内部状态（dirty / confirmSave）
        // ==========================================================

        case 2:
        {

            lastState =
                UIState::START;


            configBackup = config;


            // [新增] 重置 ConfigMenu 状态
            ui.GetConfigMenu().SetConfig(
                &config
            );


            ui.SetState(
                UIState::CONFIG
            );

        }
        break;
        // ==========================================================





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
                UIState::AFFECTION
            );

        }
        break;



        // ==========================================================
        // 进入设置时：
        //   1. 保存 Config 快照
        //   2. 重置 ConfigMenu 内部状态（dirty / confirmSave）
        // ==========================================================

        case 4:
        {

            lastState =
                UIState::PAUSE;


            configBackup = config;


            // [新增] 重置 ConfigMenu 状态
            ui.GetConfigMenu().SetConfig(
                &config
            );


            ui.SetState(
                UIState::CONFIG
            );

        }
        break;
        // ==========================================================



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

        // 无操作

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



            if(e.isEndingBranch)
            {

                RouteType rt =
                    story.GetRouteManager()
                    .CheckRoute();


                std::string target;

                if(rt == RouteType::LI_JUNHAO)
                {
                    target = e.branchLiJunhao;
                }
                else if(rt == RouteType::ZHANG_HANYU)
                {
                    target = e.branchZhangHanyu;
                }
                else
                {
                    target = e.branchNormal;
                }


                if(
                    !target.empty()
                    &&
                    story.Load(target)
                )
                {
                    e = story.GetCurrentEvent();
                }

            }



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

                currentChoiceAffection =
                    e.choiceAffection;

            }
            else
            {
                currentChoiceTargets.clear();
                currentChoiceAffection.clear();
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
        UIState::AFFECTION
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



    if(idx < (int)currentChoiceAffection.size())
    {

        for(auto& change : currentChoiceAffection[idx])
        {

            story.GetRouteManager()
            .AddAffection(
                change.character,
                change.delta
            );

        }

    }



    std::string target =
        currentChoiceTargets[idx];



    ui.GetDialogueUI()
    .ClearChoice();

    currentChoiceTargets.clear();

    currentChoiceAffection.clear();



    if(target.empty())
    {

        OnAdvanceDialogue();

        return;

    }



    if(target[0] == '#')
    {

        std::string labelName =
            target.substr(1);


        if(
            story.JumpToLabel(
                labelName
            )
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

                currentChoiceAffection =
                    e.choiceAffection;

            }

        }


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

            currentChoiceAffection =
                e.choiceAffection;

        }

    }

}