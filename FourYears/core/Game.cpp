#include "Game.h"

#include <iostream>

#include <ctime>

#include <SDL_ttf.h>

// [修改] Windows 专有头文件条件编译
#ifdef _WIN32
#include <SDL_syswm.h>
#include <windows.h>
#endif



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






    // ==========================================================
    // [修改] 保存 HWND 和旧 HIMC，供 SetInputMode 使用
    // ==========================================================

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


            // 保存 HWND
            winHwnd = (void*)hwnd;


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

                    // 把 IME 从窗口解除，并记住旧句柄
                    HIMC old =
                        pImmAssociateContext(
                            hwnd,
                            NULL
                        );

                    winOldHimc = (void*)old;

                }

            }

        }

    }

#endif

    // ==========================================================






    // 初始化时禁用文本输入，字母键全部走 KEYDOWN
    SDL_StopTextInput();

    SDL_EventState(
        SDL_TEXTINPUT,
        SDL_DISABLE
    );

    SDL_EventState(
        SDL_TEXTEDITING,
        SDL_DISABLE
    );






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









// ==========================================================
// 切换输入模式
// ==========================================================
//
// 只在状态切换时调一次（HandleEvents 开头检测）。
//
// enabled = true  -> 恢复 IME + SDL_TEXTINPUT（输入框用）
// enabled = false -> 禁用 IME + SDL_TEXTINPUT（快捷键用）

void Game::SetInputMode(bool enabled)
{

#ifdef _WIN32

    if(winHwnd)
    {

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

                if(enabled)
                {
                    pImmAssociateContext(
                        (HWND)winHwnd,
                        (HIMC)winOldHimc
                    );
                }
                else
                {
                    pImmAssociateContext(
                        (HWND)winHwnd,
                        NULL
                    );
                }

            }

        }

    }

#endif


    // ==========================================================
    // SDL 层
    // ==========================================================
    //
    // [新增] IME 候选窗提示：
    //   输入框激活时显示候选词窗口（中文输入时能看到候选词）。
    //   输入框关闭时隐藏，避免字母键误触发候选窗。
    //
    // 注意：SDL_SetHint 必须在 StartTextInput 之前调，
    // 否则本次切换不会生效。

    if(enabled)
    {

        // 显示 IME 候选窗
        SDL_SetHint(
            SDL_HINT_IME_SHOW_UI,
            "1"
        );


        SDL_EventState(
            SDL_TEXTINPUT,
            SDL_ENABLE
        );

        SDL_EventState(
            SDL_TEXTEDITING,
            SDL_ENABLE
        );

        SDL_StartTextInput();

    }
    else
    {

    // [修改] 不再在这里强制设 0。
    // 让 SetInputMode 在切换时自己控制。
    // 有些 SDL 版本对同一个 hint 只读一次，
    // Init 里设 0 之后 SetInputMode 改 1 就不生效了。

    SDL_StopTextInput();

    SDL_EventState(
        SDL_TEXTINPUT,
        SDL_DISABLE
    );

    SDL_EventState(
        SDL_TEXTEDITING,
        SDL_DISABLE
    );

    }

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

        bool renaming =

        ui.GetState() == UIState::SAVE &&
        ui.GetSaveMenu().IsRenaming();


    if(renaming != lastRenaming)
    {
        SetInputMode(renaming);
        lastRenaming = renaming;


        // ==========================================================
        // [新增] 告诉 IME 输入框在屏幕上的位置
        // ==========================================================
        //
        // Windows 的 IME 候选窗会贴着这个矩形显示。
        // 不设置的话，候选窗可能画在窗口外或者干脆不出现。
        //
        // 坐标是窗口坐标，不是逻辑坐标。
        // 但 SDL2 在设置了 RenderSetLogicalSize 后，
        // SDL_SetTextInputRect 用的仍是窗口坐标，
        // 所以这里要按物理窗口尺寸换算。
        //
        // 简单起见，直接用窗口中心偏下：
        //   x = 窗口宽的 25%
        //   y = 窗口高的 85%
        //   w = 窗口宽的 50%
        //   h = 40

        if(renaming)
        {
            int winW = 0;
            int winH = 0;

            SDL_GetWindowSize(
                window,
                &winW,
                &winH
            );


            SDL_Rect rect;

            rect.x = winW / 4;
            rect.y = (int)(winH * 0.85f);
            rect.w = winW / 2;
            rect.h = 40;


            SDL_SetTextInputRect(&rect);
        }

        // ==========================================================
    } 


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





                // ==========================================================
        // SDL_TEXTINPUT：输入框接收"已上屏"的文本
        // ==========================================================
        //
        // 输入法里选中汉字后，这里收到 "你好"。

        if(
            event.type ==
            SDL_TEXTINPUT
        )
        {

            if(renaming)
            {
                ui.GetSaveMenu()
                .AppendRenameText(
                    event.text.text
                );
            }

        }

        // ==========================================================


        // ==========================================================
        // [新增] SDL_TEXTEDITING：输入法预编辑（拼音）
        // ==========================================================
        //
        // 玩家在输入法里敲 "nihao" 还没选字时，
        // 这里收到 "nihao"。
        // 用来在输入框下方实时显示，让玩家知道在打什么。
        //
        // 上屏（SDL_TEXTINPUT）后，SDL 会发一个空字符串的
        // TEXTEDITING，表示预编辑结束。

        if(
            event.type ==
            SDL_TEXTEDITING
        )
        {

            if(renaming)
            {
                ui.GetSaveMenu()
                .SetEditingText(
                    event.edit.text
                );
            }

        }

        // ==========================================================




        if(
            event.type ==
            SDL_KEYDOWN
        )
        {

            // ----------------------------------------------------------
            // 重命名输入模式拦截（只处理控制键）
            // ----------------------------------------------------------
            //
            // 字符输入走 SDL_TEXTINPUT，
            // KEYDOWN 只处理 Enter / ESC / Backspace。

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
            // 设置询问状态拦截
            // ----------------------------------------------------------

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
                    OnBack();
                }
                else if(r == 0)
                {
                    config = configBackup;

                    OnBack();
                }

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





        case 2:
        {

            lastState =
                UIState::START;


            configBackup = config;


            ui.GetConfigMenu().SetConfig(
                &config
            );


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



        case 4:
        {

            lastState =
                UIState::PAUSE;


            configBackup = config;


            ui.GetConfigMenu().SetConfig(
                &config
            );


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