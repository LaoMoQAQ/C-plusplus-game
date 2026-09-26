#include "Game.h"

#include <iostream>

#include <ctime>

#include <SDL_ttf.h>

// [新增] 用于禁用输入法。
// SDL_syswm.h 提供 SDL_GetWindowWMInfo，
// 拿到底层 Win32 HWND。
#include <SDL_syswm.h>

// [新增] Win32 API：
//   HWND, HMODULE, LoadLibraryA, GetProcAddress
//   HIMC, ImmAssociateContext
// main.cpp 里已 include <windows.h>，
// 这里再 include 一次是安全的（有 include guard）。
#include <windows.h>



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






    // ============================================================
    // [新增] 禁用中文输入法拦截键盘事件
    // ============================================================
    //
    // 问题背景：
    //   中文输入法激活时，字母键会先被 IME 拦截，
    //   SDL 收不到 SDLK_n / SDLK_c / SDLK_r 的 KEYDOWN，
    //   导致游戏内这些快捷键全部失效。
    //   Enter / ESC / 方向键不受影响，因为它们不走 IME。
    //
    // 为什么不能只靠 SDL 层：
    //   SDL_StopTextInput() / SDL_EventState(SDL_TEXTINPUT, ...)
    //   只是告诉 SDL 不要发 TEXTINPUT 事件，
    //   但系统 IME 依然会拦截按键。
    //   必须用 Win32 的 ImmAssociateContext(hwnd, NULL)
    //   把窗口和 IME 彻底切断。
    //
    // 未来做"重命名输入框"时：
    //   需要临时恢复 IME 才能输入中文。
    //   恢复方法：
    //     1. 保留 ImmAssociateContext 返回的旧 HIMC
    //     2. 输入框打开时 ImmAssociateContext(hwnd, 旧HIMC)
    //     3. 输入框关闭时再 ImmAssociateContext(hwnd, NULL)

    // 第 1 层：SDL 层
    // 隐藏 IME 候选窗口，关闭 SDL 的文本输入事件
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


    // 第 2 层：Win32 层
    // 切断窗口与 IME 的关联，这是彻底解决的关键
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


            // 动态加载 imm32.dll，避免改 tasks.json
            HMODULE hImm =
                LoadLibraryA(
                    "imm32.dll"
                );


            if(hImm)
            {

                // ImmAssociateContext 函数签名：
                //   HIMC ImmAssociateContext(HWND, HIMC)
                //
                // 传 NULL 作为第二个参数，
                // 就是把 IME 上下文从窗口上解除。
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

                // 不 FreeLibrary，
                // 保持 imm32 常驻进程，
                // 方便将来做输入框时再调用。

            }

        }

    }

#endif

    // ============================================================







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






    // 预加载主菜单背景
    resourceManager.LoadTexture(

        renderer.GetSDLRenderer(),

        "resource/bg/main_menu.png"

    );



    // 把 FontManager 绑定给 StartMenu
    ui.GetStartMenu().SetFontManager(
        &fontManager
    );



    // 绑定 Config 给 ConfigMenu
    ui.GetConfigMenu().SetConfig(
        &config
    );



    // 绑定 History 给 HistoryMenu
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


            switch(
                event.key.keysym.sym
            )
            {


            // 上
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
                else
                {

                    ui.HandleInput(1);

                }

            }
            break;





            // 下
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

                    ui.GetSaveMenu()
                    .Create(
                        saveSystem,
                        "新的存档",
                        1,
                        story.GetIndex()
                    );

                }

            }
            break;





            // 复制存档
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





            // 删除存档
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

                    SaveData data =
                        ui.GetSaveMenu()
                        .GetCurrentSave();


                    if(
                        !data.filename.empty()
                    )
                    {

                        std::string newName =
                            "存档_"
                            +
                            std::to_string(
                                std::time(nullptr)
                            );


                        ui.GetSaveMenu()
                        .Rename(
                            saveSystem,
                            newName
                        );

                    }

                }

            }
            break;





            // Enter
            case SDLK_RETURN:
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
            break;





            // ESC
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





            // 空格推进剧情
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





        // 鼠标移动
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





        // 鼠标左键
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

                OnAdvanceDialogue();

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
        renderer.DrawTexture(
            currentCharacter,
            520,
            80
        );
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

        int ch = 0;
        int idx = 0;


        if(
            ui.GetSaveMenu()
            .Confirm(
                saveSystem,
                ch,
                idx
            )
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


                ui.SetState(
                    UIState::DIALOGUE
                );

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


        // 0 开始游戏
        case 0:
        {

            story.SetIndex(0);



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





        // 1 存档
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





        // 2 设置
        case 2:
        {

            lastState =
                UIState::START;


            ui.SetState(
                UIState::CONFIG
            );

        }
        break;





        // 3 退出游戏
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





    // 设置菜单：
    // 只有选中"保存设置"时，Enter 才真正保存
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