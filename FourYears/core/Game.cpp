#include "Game.h"

#include <iostream>
#include <ctime>
#include <SDL_ttf.h>

#ifdef _WIN32
#include <SDL_syswm.h>
#include <windows.h>
#endif



static constexpr int CHARACTER_HEIGHT   = 700;
static constexpr int CHARACTER_CENTER_X = 1220;



// ==========================================================
// 章节名映射
// ==========================================================

static std::string ChapterNameFromScript(
    const std::string& script
)
{
    if(script.find("chapter01") != std::string::npos) return "第1章";
    if(script.find("chapter02") != std::string::npos) return "第2章";
    if(script.find("chapter03") != std::string::npos) return "第3章";
    if(script.find("chapter04") != std::string::npos) return "第4章";
    if(script.find("chapter05") != std::string::npos) return "第5章";

    // 结局相关的判定放最后（文件名匹配更宽松）
    if(script.find("alone_ending") != std::string::npos) return "个人结局";
    if(script.find("li_junhao_route") != std::string::npos) return "李君浩线";
    if(script.find("zhang_hanyu_route") != std::string::npos) return "张瀚宇线";

    // ending 放最后：会匹配到 ending.txt
    if(script.find("ending") != std::string::npos) return "结局";

    return "未知";
}



#ifdef _WIN32

typedef HIMC (WINAPI *PFN_ImmAssociateContext)(HWND, HIMC);

static PFN_ImmAssociateContext GetImmAssociateContext()
{
    HMODULE hImm = LoadLibraryA("imm32.dll");

    if(!hImm)
        return nullptr;

    return (PFN_ImmAssociateContext)
        GetProcAddress(hImm, "ImmAssociateContext");
}

#endif



// ==========================================================
// 构造 / 析构
// ==========================================================

Game::Game()
{
    running  = false;
    window   = nullptr;
    currentBackground = nullptr;

    lastState = UIState::START;
}



Game::~Game()
{
    Quit();
}



// ==========================================================
// Init
// ==========================================================

bool Game::Init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cout << "SDL初始化失败\n";
        return false;
    }

    if(TTF_Init() != 0)
    {
        std::cout
            << "TTF初始化失败:"
            << TTF_GetError()
            << std::endl;
        return false;
    }


    config.Load("config.ini");


    window = SDL_CreateWindow(
        "Four Years",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.GetWidth(),
        config.GetHeight(),
        SDL_WINDOW_SHOWN
    );

    if(!window)
        return false;



#ifdef _WIN32

    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);

        if(SDL_GetWindowWMInfo(window, &wmInfo))
        {
            HWND hwnd = wmInfo.info.win.window;

            winHwnd = (void*)hwnd;

            PFN_ImmAssociateContext p =
                GetImmAssociateContext();

            if(p)
            {
                HIMC old = p(hwnd, NULL);
                winOldHimc = (void*)old;
            }
        }
    }

#endif



    SDL_StopTextInput();
    SDL_EventState(SDL_TEXTINPUT,   SDL_DISABLE);
    SDL_EventState(SDL_TEXTEDITING, SDL_DISABLE);



    if(!renderer.Init(window))
        return false;



    SDL_RenderSetLogicalSize(
        renderer.GetSDLRenderer(),
        config.GetWidth(),
        config.GetHeight()
    );



    resourceManager.Init();
    audioManager.Init();



    fontManager.Load("resource/font/simhei.ttf");
    saveSystem.Init();

    renderer.SetFont(fontManager.GetFont());



    story.Load("script/chapter01.txt");



    for(auto& e : story.events)
    {
        if(!e.background.empty())
        {
            resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                "resource/bg/" + e.background
            );
        }

        // 立绘三个位置
        if(!e.characterLeft.empty() && e.characterLeft != "clear")
        {
            resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                "resource/character/" + e.characterLeft
            );
        }

        if(!e.characterCenter.empty() && e.characterCenter != "clear")
        {
            resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                "resource/character/" + e.characterCenter
            );
        }

        if(!e.characterRight.empty() && e.characterRight != "clear")
        {
            resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                "resource/character/" + e.characterRight
            );
        }
    }

    resourceManager.LoadTexture(
        renderer.GetSDLRenderer(),
        "resource/bg/main_menu.png"
    );



    // ---- 绑定依赖 ----

    ui.GetStartMenu().SetFontManager(&fontManager);
    ui.GetConfigMenu().SetConfig(&config);
    ui.GetAffectionMenu().SetRouteManager(&story.GetRouteManager());

    // [新增] 打字音效
    ui.GetDialogueUI().SetTypingSound(
        &audioManager,
        "resource/se/typing.wav"
    );



    // ---- 应用初始配置 ----

    lastBgmVolume  = config.GetBGMVolume();
    lastSeVolume   = config.GetSEVolume();
    lastTextSpeed  = config.GetTextSpeed();
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



    ui.SetState(UIState::START);

    running = true;

    return true;
}



// ==========================================================
// SetInputMode
// ==========================================================

void Game::SetInputMode(bool enabled)
{
#ifdef _WIN32

    if(winHwnd)
    {
        PFN_ImmAssociateContext p =
            GetImmAssociateContext();

        if(p)
        {
            if(enabled)
            {
                p((HWND)winHwnd, (HIMC)winOldHimc);
            }
            else
            {
                p((HWND)winHwnd, NULL);
            }
        }
    }

#endif


    if(enabled)
    {
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

        SDL_EventState(SDL_TEXTINPUT,   SDL_ENABLE);
        SDL_EventState(SDL_TEXTEDITING, SDL_ENABLE);

        SDL_StartTextInput();
    }
    else
    {
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "0");

        SDL_StopTextInput();

        SDL_EventState(SDL_TEXTINPUT,   SDL_DISABLE);
        SDL_EventState(SDL_TEXTEDITING, SDL_DISABLE);
    }
}



// ==========================================================
// EnterSavePage
// ==========================================================

void Game::EnterSavePage(SavePage page)
{
    ui.GetSaveMenu().SetPage(page);
    ui.GetSaveMenu().Refresh(saveSystem);

    ui.SetState(UIState::SAVE);
}



// ==========================================================
// IsRenaming
// ==========================================================

bool Game::IsRenaming()
{
    return ui.GetState() == UIState::SAVE &&
           ui.GetSaveMenu().IsRenaming();
}



// ==========================================================
// SetSprite
// ==========================================================
//
// 设置某个位置的立绘目标。
// path 为空或 "clear" 表示清除该位置。
//
// 纹理变化时 alpha 从 0 开始，实现淡入。
// 不变时只更新目标 alpha。

void Game::SetSprite(int slot, const std::string& path)
{
    if(slot < 0 || slot > 2)
        return;


    ActiveSprite& sp = sprites[slot];


    // 清除
    if(path.empty() || path == "clear")
    {
        sp.targetAlpha = 0.0f;
        return;
    }


    // 加载纹理
    std::string fullPath = "resource/character/" + path;

    SDL_Texture* tex = resourceManager.GetTexture(fullPath);

    if(!tex)
    {
        tex = resourceManager.LoadTexture(
            renderer.GetSDLRenderer(),
            fullPath
        );
    }

    if(!tex)
        return;


    // 换了纹理：从透明开始淡入
    if(tex != sp.texture)
    {
        sp.texture = tex;
        sp.alpha = 0.0f;
    }

    sp.targetAlpha = 1.0f;
}



// ==========================================================
// 主循环
// ==========================================================

void Game::Run()
{
    while(running)
    {
        HandleEvents();
        Update();
        Render();
        SDL_Delay(16);
    }
}



// ==========================================================
// HandleEvents
// ==========================================================

void Game::HandleEvents()
{
    // ==========================================================
    // 过渡期间锁输入。
    // ==========================================================
    //
    // 只响应关闭窗口，其他键鼠事件直接清空队列。
    // 避免玩家在闪黑期间连按 ESC 导致状态抖动。

    if(ui.IsTransitioning())
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = false;
        }
        return;
    }


    bool renaming = IsRenaming();

    if(renaming != lastRenaming)
    {
        SetInputMode(renaming);
        lastRenaming = renaming;

        if(renaming)
        {
            int winW = 0, winH = 0;
            SDL_GetWindowSize(window, &winW, &winH);

            SDL_Rect rect;
            rect.x = winW / 4;
            rect.y = (int)(winH * 0.85f);
            rect.w = winW / 2;
            rect.h = 40;

            SDL_SetTextInputRect(&rect);
        }
    }


    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            running = false;
            continue;
        }


        if(event.type == SDL_TEXTINPUT ||
           event.type == SDL_TEXTEDITING)
        {
            HandleTextInput();
        }
        else if(event.type == SDL_KEYDOWN)
        {
            HandleKeyDown();
        }
        else if(event.type == SDL_MOUSEMOTION ||
                event.type == SDL_MOUSEBUTTONDOWN)
        {
            HandleMouseEvent();
        }
    }
}



// ==========================================================
// HandleTextInput
// ==========================================================

void Game::HandleTextInput()
{
    if(!IsRenaming())
        return;


    if(event.type == SDL_TEXTINPUT)
    {
        ui.GetSaveMenu().AppendRenameText(event.text.text);
    }
    else if(event.type == SDL_TEXTEDITING)
    {
        ui.GetSaveMenu().SetEditingText(event.edit.text);
    }
}



// ==========================================================
// HandleKeyDown
// ==========================================================

void Game::HandleKeyDown()
{
    UIState st = ui.GetState();


    // ---- 1. 重命名输入模式 ----

    if(st == UIState::SAVE && ui.GetSaveMenu().IsRenaming())
    {
        ui.GetSaveMenu().HandleRenameKey(
            event.key.keysym.sym,
            saveSystem
        );
        return;
    }


    // ---- 2. 设置询问模式 ----

    if(st == UIState::CONFIG && ui.GetConfigMenu().IsConfirming())
    {
        int r = ui.GetConfigMenu().HandleConfirmKey(
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

        return;
    }


    // ---- 3. 通用键盘 ----

    switch(event.key.keysym.sym)
    {
    case SDLK_UP:
        if(st == UIState::SAVE)
        {
            ui.GetSaveMenu().HandleInput(1);
        }
        else if(st == UIState::DIALOGUE &&
                ui.GetDialogueUI().HasChoice())
        {
            ui.GetDialogueUI().MoveChoice(-1);
        }
        else
        {
            ui.HandleInput(1);
        }
        break;


    case SDLK_DOWN:
        if(st == UIState::SAVE)
        {
            ui.GetSaveMenu().HandleInput(2);
        }
        else if(st == UIState::DIALOGUE &&
                ui.GetDialogueUI().HasChoice())
        {
            ui.GetDialogueUI().MoveChoice(1);
        }
        else
        {
            ui.HandleInput(2);
        }
        break;


    case SDLK_LEFT:
        if(st == UIState::CONFIG)
            ui.HandleInput(3);
        break;


    case SDLK_RIGHT:
        if(st == UIState::CONFIG)
            ui.HandleInput(4);
        break;


    case SDLK_s:
        if(st == UIState::CONFIG &&
           !ui.GetConfigMenu().IsConfirming())
        {
            ui.GetConfigMenu().Save();
        }
        break;


    case SDLK_n:
        if(st == UIState::SAVE)
        {
            // [修改] 进入输入模式让玩家起名，而不是直接创建
            ui.GetSaveMenu().BeginCreate(
                story.GetCurrentFile(),
                ChapterNameFromScript(story.GetCurrentFile()),
                story.GetIndex(),
                story.GetRouteManager().GetAllAffection()
            );
        }
        break;


    case SDLK_c:
        if(st == UIState::SAVE)
            ui.GetSaveMenu().Copy(saveSystem);
        break;


    case SDLK_DELETE:
        if(st == UIState::SAVE)
        {
            if(ui.GetSaveMenu().IsConfirmingDelete())
                ui.GetSaveMenu().ConfirmDelete(saveSystem);
            else
                ui.GetSaveMenu().BeginDelete();
        }
        break;


    case SDLK_r:
        if(st == UIState::SAVE)
            ui.GetSaveMenu().BeginRename();
        break;


    case SDLK_RETURN:
        if(st == UIState::DIALOGUE &&
           ui.GetDialogueUI().HasChoice())
        {
            ConfirmChoice();
        }
        else if(st == UIState::SAVE &&
                ui.GetSaveMenu().IsConfirmingDelete())
        {
            ui.GetSaveMenu().ConfirmDelete(saveSystem);
        }
        else
        {
            OnActivateCurrentState();
        }
        break;


    case SDLK_ESCAPE:
        if(st == UIState::SAVE &&
           ui.GetSaveMenu().IsConfirmingDelete())
        {
            ui.GetSaveMenu().CancelDelete();
        }
        else if(st == UIState::CONFIG)
        {
            if(ui.GetConfigMenu().TryExit())
                OnBack();
        }
        else
        {
            OnBack();
        }
        break;


    case SDLK_SPACE:
        if(st == UIState::DIALOGUE)
            OnAdvanceDialogue();
        break;
    }
}



// ==========================================================
// HandleMouseEvent
// ==========================================================

void Game::HandleMouseEvent()
{
    UIState st = ui.GetState();


    if(event.type == SDL_MOUSEMOTION)
    {
        if(st == UIState::DIALOGUE &&
           ui.GetDialogueUI().HasChoice())
        {
            int idx = ui.GetDialogueUI().HitTestChoice(
                event.motion.x,
                event.motion.y
            );

            if(idx >= 0)
                ui.GetDialogueUI().SetChoiceIndex(idx);
        }
        else
        {
            ui.HandleMouseMove(
                event.motion.x,
                event.motion.y
            );
        }
        return;
    }


    if(event.type == SDL_MOUSEBUTTONDOWN &&
       event.button.button == SDL_BUTTON_LEFT)
    {
        int mx = event.button.x;
        int my = event.button.y;


        if(st == UIState::DIALOGUE)
        {
            if(ui.GetDialogueUI().HasChoice())
            {
                int idx = ui.GetDialogueUI().HitTestChoice(mx, my);

                if(idx >= 0)
                {
                    ui.GetDialogueUI().SetChoiceIndex(idx);
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
            MenuMouseResult r = ui.HandleMouseClick(mx, my);

            if(r == MenuMouseResult::ACTIVATE)
            {
                if(st == UIState::SAVE &&
                   ui.GetSaveMenu().IsConfirmingDelete())
                {
                    ui.GetSaveMenu().ConfirmDelete(saveSystem);
                }
                else
                {
                    OnActivateCurrentState();
                }
            }
            else if(r == MenuMouseResult::BACK)
            {
                if(st == UIState::CONFIG)
                {
                    if(ui.GetConfigMenu().TryExit())
                        OnBack();
                }
                else
                {
                    OnBack();
                }
            }
        }
    }
}



// ==========================================================
// UpdateScene
// ==========================================================
//
// 根据当前 StoryEvent 更新背景 / 立绘 / BGM / SE。

void Game::UpdateScene()
{
    StoryEvent e = story.GetCurrentEvent();


    // ---- 背景 ----

    if(!e.background.empty())
    {
        std::string path = "resource/bg/" + e.background;

        SDL_Texture* tex = resourceManager.GetTexture(path);

        if(!tex)
        {
            tex = resourceManager.LoadTexture(
                renderer.GetSDLRenderer(),
                path
            );
        }

        if(tex)
            currentBackground = tex;
    }


    // ---- 立绘（三个位置） ----

    // 非空才更新，空 = 不变
    if(!e.characterLeft.empty())
        SetSprite(0, e.characterLeft);

    if(!e.characterCenter.empty())
        SetSprite(1, e.characterCenter);

    if(!e.characterRight.empty())
        SetSprite(2, e.characterRight);


    // ---- BGM ----

    if(!e.bgm.empty() && e.bgm != lastBgmPath)
    {
        lastBgmPath = e.bgm;

        if(e.bgm == "stop")
        {
            audioManager.StopBGM();
        }
        else
        {
            audioManager.PlayBGM("resource/bgm/" + e.bgm);
        }
    }


    // ---- SE ----

    if(!e.se.empty())
    {
        audioManager.PlaySE("resource/se/" + e.se);
    }
}



// ==========================================================
// Render
// ==========================================================

void Game::Render()
{
    renderer.Clear();

    UIState st = ui.GetState();


    bool useGameBackground = false;

    if(st == UIState::DIALOGUE || st == UIState::PAUSE)
    {
        useGameBackground = true;
    }
    else if(st == UIState::SAVE ||
            st == UIState::CONFIG ||
            st == UIState::AFFECTION)
    {
        useGameBackground = (lastState == UIState::PAUSE);
    }


    bool needBlur =
        st == UIState::PAUSE ||
        st == UIState::SAVE ||
        st == UIState::CONFIG ||
        st == UIState::AFFECTION;


    SDL_Texture* menuBg = resourceManager.GetTexture(
        "resource/bg/main_menu.png"
    );

    SDL_Texture* bgToDraw = nullptr;

    if(useGameBackground)
        bgToDraw = currentBackground ? currentBackground : menuBg;
    else
        bgToDraw = menuBg;


    if(bgToDraw)
    {
        if(needBlur)
            renderer.DrawBlurTexture(bgToDraw, 0, 0);
        else
            renderer.DrawTexture(bgToDraw, 0, 0);
    }


    // 立绘：只在 DIALOGUE 状态画
    if(st == UIState::DIALOGUE)
    {
        // 三个位置的中心 X
        // 左：屏幕 25%  中：50%  右：75%
        static const int slotCenterX[3] = { 400, 800, 1200 };

        for(int i = 0; i < 3; i++)
        {
            ActiveSprite& sp = sprites[i];

            if(!sp.texture) continue;
            if(sp.alpha < 0.01f) continue;


            int texW = 0, texH = 0;
            SDL_QueryTexture(
                sp.texture,
                nullptr, nullptr,
                &texW, &texH
            );

            if(texW <= 0 || texH <= 0) continue;


            int drawH = CHARACTER_HEIGHT;
            int drawW = texW * drawH / texH;

            int drawX = slotCenterX[i] - drawW / 2;
            int drawY = (config.GetHeight() - drawH) / 2;


            // 应用透明度
            SDL_SetTextureAlphaMod(
                sp.texture,
                (Uint8)(sp.alpha * 255.0f)
            );

            renderer.DrawTexture(
                sp.texture,
                drawX, drawY,
                drawW, drawH
            );

            // 恢复，避免影响其他绘制
            SDL_SetTextureAlphaMod(sp.texture, 255);
        }
    }


    // 把当前背景告诉 DialogueUI，它用来做对话框的背景模糊
    ui.GetDialogueUI().SetBackgroundTexture(currentBackground);

    ui.Render(renderer);

    renderer.Present();
}



// ==========================================================
// Update
// ==========================================================

void Game::Update()
{
    // 推进 UI 过渡动画
    ui.Update();

    ui.GetDialogueUI().Update();


    // ---- 立绘淡入淡出 ----

    for(int i = 0; i < 3; i++)
    {
        ActiveSprite& sp = sprites[i];

        if(sp.texture == nullptr)
            continue;

        float diff = sp.targetAlpha - sp.alpha;

        if(diff > -0.01f && diff < 0.01f)
        {
            sp.alpha = sp.targetAlpha;
        }
        else
        {
            sp.alpha += diff * SPRITE_FADE_SPEED;
        }
    }


    // ---- 配置同步 ----

    if(config.GetBGMVolume() != lastBgmVolume)
    {
        lastBgmVolume = config.GetBGMVolume();
        audioManager.SetBGMVolume(lastBgmVolume);
    }

    if(config.GetSEVolume() != lastSeVolume)
    {
        lastSeVolume = config.GetSEVolume();
        audioManager.SetSEVolume(lastSeVolume);
    }

    if(config.GetTextSpeed() != lastTextSpeed)
    {
        lastTextSpeed = config.GetTextSpeed();
        ui.GetDialogueUI().SetTextSpeed(lastTextSpeed);
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


    // ---- 自动播放 ----

    if(config.IsAutoPlay() &&
       ui.GetState() == UIState::DIALOGUE &&
       !ui.GetDialogueUI().HasChoice() &&
       ui.GetDialogueUI().Finished())
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



// ==========================================================
// Quit
// ==========================================================

void Game::Quit()
{
    running = false;

    if(window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    SDL_Quit();
}



// ==========================================================
// OnActivateCurrentState
// ==========================================================

void Game::OnActivateCurrentState()
{
    UIState st = ui.GetState();


    // ---- 存档界面：读档 ----

    if(st == UIState::SAVE)
    {
        std::string scriptFile;
        std::string chapterName;
        int idx = 0;
        std::map<std::string, int> aff;

        if(ui.GetSaveMenu().Confirm(
            saveSystem,
            scriptFile,
            chapterName,
            idx,
            aff
        ))
        {
            if(!scriptFile.empty() && story.Load(scriptFile))
            {
                if(idx >= 0 && idx < (int)story.events.size())
                {
                    story.GetRouteManager().SetAllAffection(aff);
                    story.SetIndex(idx);

                    UpdateScene();

                    StoryEvent e = story.GetCurrentEvent();

                    ui.GetDialogueUI().SetSpeaker(e.name);
                    ui.GetDialogueUI().SetText(e.text, e.waitTime);

                    if(e.isChoice)
                    {
                        ui.GetDialogueUI().ShowChoice(e.choices);
                        currentChoiceTargets   = e.choiceTargets;
                        currentChoiceAffection = e.choiceAffection;
                    }
                    else
                    {
                        ui.GetDialogueUI().ClearChoice();
                        currentChoiceTargets.clear();
                        currentChoiceAffection.clear();
                    }

                    ui.SetState(UIState::DIALOGUE);
                }
            }
        }
        return;
    }


    // ---- 主菜单 ----

    if(st == UIState::START)
    {
        int choice = ui.GetStartMenu().GetChoice();

        switch(choice)
        {
        case 0:
        {
            story.Load("script/chapter01.txt");

            // 换章节时重置 BGM 状态
            lastBgmPath.clear();

            ui.GetDialogueUI().ClearChoice();
            currentChoiceTargets.clear();
            currentChoiceAffection.clear();

            StoryEvent e = story.GetCurrentEvent();

            UpdateScene();

            ui.GetDialogueUI().SetSpeaker(e.name);
            ui.GetDialogueUI().SetText(e.text, e.waitTime);

            ui.SetState(UIState::DIALOGUE);
            break;
        }

        case 1:
        {
            lastState = UIState::START;
            EnterSavePage(SavePage::LOAD);
            break;
        }

        case 2:
        {
            lastState = UIState::START;
            configBackup = config;
            ui.GetConfigMenu().SetConfig(&config);
            ui.SetState(UIState::CONFIG);
            break;
        }

        case 3:
        {
            running = false;
            break;
        }
        }
        return;
    }


    // ---- 暂停菜单 ----

    if(st == UIState::PAUSE)
    {
        int choice = ui.GetPauseMenu().GetChoice();

        switch(choice)
        {
        case 0:
            ui.SetState(UIState::DIALOGUE);
            break;

        case 1:
            lastState = UIState::PAUSE;
            EnterSavePage(SavePage::MANAGE);
            break;

        case 2:
            lastState = UIState::PAUSE;
            EnterSavePage(SavePage::LOAD);
            break;

        case 3:
            lastState = UIState::PAUSE;
            ui.SetState(UIState::AFFECTION);
            break;

        case 4:
            lastState = UIState::PAUSE;
            configBackup = config;
            ui.GetConfigMenu().SetConfig(&config);
            ui.SetState(UIState::CONFIG);
            break;

        case 5:
            ui.SetState(UIState::START);
            break;
        }
        return;
    }
}



// ==========================================================
// OnAdvanceDialogue
// ==========================================================

void Game::OnAdvanceDialogue()
{
    if(ui.GetDialogueUI().HasChoice())
        return;

    if(!ui.GetDialogueUI().Finished())
    {
        ui.GetDialogueUI().Skip();
        return;
    }

    if(story.Next())
    {
        StoryEvent e = story.GetCurrentEvent();


        if(e.isEndingBranch)
        {
            RouteType rt = story.GetRouteManager().CheckRoute();

            std::string target;

            if(rt == RouteType::LI_JUNHAO)
                target = e.branchLiJunhao;
            else if(rt == RouteType::ZHANG_HANYU)
                target = e.branchZhangHanyu;
            else
                target = e.branchNormal;

            if(!target.empty() && story.Load(target))
                e = story.GetCurrentEvent();
        }


        UpdateScene();

        ui.GetDialogueUI().SetSpeaker(e.name);
        ui.GetDialogueUI().SetText(e.text, e.waitTime);


        if(e.isChoice)
        {
            ui.GetDialogueUI().ShowChoice(e.choices);
            currentChoiceTargets   = e.choiceTargets;
            currentChoiceAffection = e.choiceAffection;
        }
        else
        {
            currentChoiceTargets.clear();
            currentChoiceAffection.clear();
        }
    }
}



// ==========================================================
// OnBack
// ==========================================================

void Game::OnBack()
{
    UIState st = ui.GetState();

    if(st == UIState::DIALOGUE)
        ui.SetState(UIState::PAUSE);
    else if(st == UIState::PAUSE)
        ui.SetState(UIState::DIALOGUE);
    else if(st == UIState::SAVE)
        ui.SetState(lastState);
    else if(st == UIState::CONFIG)
        ui.SetState(lastState);
    else if(st == UIState::AFFECTION)
        ui.SetState(lastState);
}



// ==========================================================
// ConfirmChoice
// ==========================================================

void Game::ConfirmChoice()
{
    int idx = ui.GetDialogueUI().GetChoiceIndex();

    if(idx < 0 || idx >= (int)currentChoiceTargets.size())
        return;


    if(idx < (int)currentChoiceAffection.size())
    {
        for(auto& change : currentChoiceAffection[idx])
        {
            story.GetRouteManager().AddAffection(
                change.character,
                change.delta
            );
        }
    }


    std::string target = currentChoiceTargets[idx];

    ui.GetDialogueUI().ClearChoice();
    currentChoiceTargets.clear();
    currentChoiceAffection.clear();


    if(target.empty())
    {
        OnAdvanceDialogue();
        return;
    }


    if(target[0] == '#')
    {
        std::string labelName = target.substr(1);

        if(story.JumpToLabel(labelName))
        {
            StoryEvent e = story.GetCurrentEvent();

            UpdateScene();

            ui.GetDialogueUI().SetSpeaker(e.name);
            ui.GetDialogueUI().SetText(e.text, e.waitTime);

            if(e.isChoice)
            {
                ui.GetDialogueUI().ShowChoice(e.choices);
                currentChoiceTargets   = e.choiceTargets;
                currentChoiceAffection = e.choiceAffection;
            }
        }
        return;
    }


    if(story.Load(target))
    {
        StoryEvent e = story.GetCurrentEvent();

        UpdateScene();

        ui.GetDialogueUI().SetSpeaker(e.name);
        ui.GetDialogueUI().SetText(e.text, e.waitTime);

        if(e.isChoice)
        {
            ui.GetDialogueUI().ShowChoice(e.choices);
            currentChoiceTargets   = e.choiceTargets;
            currentChoiceAffection = e.choiceAffection;
        }
    }
}