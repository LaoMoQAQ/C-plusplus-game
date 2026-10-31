#include "Game.h"

#include <iostream>
#include <ctime>
#include <SDL_ttf.h>

#ifdef _WIN32
#include <SDL_syswm.h>
#include <windows.h>
#include <cstdlib>
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
    // chapterXX -> 第XX章
    size_t pos = script.find("chapter");

    if(pos != std::string::npos)
    {
        size_t start = pos + 7;  // "chapter" 7 个字符

        std::string numStr;

        while(
            start < script.size() &&
            script[start] >= '0' &&
            script[start] <= '9'
        )
        {
            numStr += script[start];
            start++;
        }

        if(!numStr.empty())
        {
            int num = std::atoi(numStr.c_str());
            return "第" + std::to_string(num) + "章";
        }
    }

    // 结局特判
    if(script.find("alone_ending")      != std::string::npos) return "个人结局";
    if(script.find("li_junhao_route")   != std::string::npos) return "李君浩线";
    if(script.find("zhang_hanyu_route") != std::string::npos) return "张瀚宇线";
    if(script.find("ending")            != std::string::npos) return "结局";

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
    Shutdown();
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
    ui.GetSaveMenu().SetSaveSystem(&saveSystem);

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

// ==========================================================
// Run
// ==========================================================
//
// 帧率策略：
//   1. 优先靠 Renderer 的垂直同步（D3D/OpenGL 都支持）。
//   2. vsync 不一定生效——窗口被遮挡、远程桌面、部分驱动下
//      Present 会立刻返回，实测能跑到 300+ FPS 空转。
//      所以再加一个兜底限帧：只有"这一帧跑得太快"时才补 sleep，
//      vsync 正常工作时它永远不会触发，不引入额外延迟。
//
// 之前这里是 SDL_Delay(16) 无条件等待，和 vsync 叠加后
// 实际帧率被压到 ~50 FPS，且文字速度、自动播放都按 0.016 写死，
// 导致手感比配置值慢约 18%。

void Game::Run()
{
    Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 prev = SDL_GetPerformanceCounter();

    const float TARGET_FRAME = 1.0f / 60.0f;

    while(running)
    {
        Uint64 now = SDL_GetPerformanceCounter();

        deltaTime = (float)(now - prev) / (float)freq;
        prev = now;

        // 限制单帧步长：切后台、拖窗口、断点回来时
        // delta 可能是好几秒，直接喂进去会让动画瞬移。
        if(deltaTime > 0.1f) deltaTime = 0.1f;
        if(deltaTime < 0.0f) deltaTime = 0.0f;

        HandleEvents();
        Update();
        Render();


        // ---- 兜底限帧 ----

        float elapsed = (float)(SDL_GetPerformanceCounter() - now) / (float)freq;

        if(elapsed < TARGET_FRAME)
        {
            Uint32 sleepMs = (Uint32)((TARGET_FRAME - elapsed) * 1000.0f);

            if(sleepMs > 0)
                SDL_Delay(sleepMs);
        }
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
        // [新增] 鼠标滚轮
        else if(event.type == SDL_MOUSEWHEEL)
        {
            // SDL 的 wheel.y：正数=向上滚（远离用户），负数=向下
            // 我们约定：正数=向下
            ui.HandleMouseWheel(-event.wheel.y);
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
        if(st == UIState::SAVE &&
           ui.GetSaveMenu().GetPage() == SavePage::MANAGE)
        {
            // 先把当前帧截下来，等玩家输完名字一起存进槽位
            CaptureThumbnail();

            // 进入输入模式让玩家起名，而不是直接创建
            ui.GetSaveMenu().BeginCreate(
                story.GetCurrentFile(),
                ChapterNameFromScript(story.GetCurrentFile()),
                story.GetIndex(),
                story.GetRouteManager().GetAllAffection(),
                story.GetRouteManager().GetRecords()
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
        if(st == UIState::SAVE &&
           ui.GetSaveMenu().GetPage() == SavePage::MANAGE)
        {
            ui.GetSaveMenu().BeginRename();
        }
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
    //
    // AudioManager 内部会判断同一首不重播，这里只负责把
    // 事件里的路径转成完整路径。

    if(!e.bgm.empty())
    {
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
        // 暂停菜单用毛玻璃（模糊 + 白色磨砂），其它二级菜单用普通模糊。
        //
        // 暂停菜单原本只叠一层暗色遮罩，看起来发闷。
        // 毛玻璃的浅色磨砂更透亮，也更符合"暂停"这种临时中断的感觉。
        //
        // 磨砂强度不能太高：40 会让浅色背景整片发白，
        // 菜单文字（白色）糊在背景里看不清。18 刚好有玻璃感又不刺眼。
        if(st == UIState::PAUSE)
            renderer.DrawFrostedTexture(bgToDraw, 0, 0, 18);
        else if(needBlur)
            renderer.DrawBlurTexture(bgToDraw, 0, 0);
        else
            renderer.DrawTexture(bgToDraw, 0, 0);
    }


    // 立绘：只在 DIALOGUE 状态画
    //
    // 暂停/存档/设置等菜单下不画立绘，只保留模糊背景。
    // （曾经在 PAUSE 下也画，但菜单本身已有暗色遮罩，
    //   叠上立绘反而让界面更乱，恢复成原来的行为。）
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


    // 剧情画面才缓存像素，供存档缩略图使用。
    // 必须在 Present 之前读，Present 之后渲染目标内容未定义。
    if(st == UIState::DIALOGUE)
        GrabFrame();


    renderer.Present();
}



// ==========================================================
// Update
// ==========================================================

void Game::Update()
{
    float delta = deltaTime;


    // 推进 UI 过渡动画
    ui.Update(delta);

    ui.GetDialogueUI().Update(delta);


    // ---- 音频淡入淡出 ----

    audioManager.Update(delta);


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
            sp.alpha += diff * SmoothFactor(SPRITE_FADE_SPEED, delta);
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
        autoPlayTimer += delta;

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

// ==========================================================
// Quit / Shutdown
// ==========================================================
//
// Quit 只负责停主循环，不碰任何资源。
//
// Shutdown 负责按正确的逆序释放：
//   1. 立绘纹理、背景纹理（都是 ResourceManager 的，交给它统一释放）
//   2. FontManager      —— 必须在 TTF_Quit 之前
//   3. ResourceManager  —— 必须在 SDL_Quit 之前（SDL_DestroyTexture + IMG_Quit）
//   4. AudioManager     —— 必须在 SDL_Quit 之前（Mix_CloseAudio）
//   5. Renderer         —— 必须在所有纹理之后
//   6. 最后才是 TTF_Quit / SDL_Quit
//
// 之前的实现在 Quit 里就调了 TTF_Quit/SDL_Quit，
// 而成员析构要等到之后才发生，导致 FontManager 在 freetype 卸载后
// 调 TTF_CloseFont，退出时必崩（libfreetype 里 SIGSEGV）。

void Game::Quit()
{
    running = false;
}




void Game::Shutdown()
{
    if(shutdownDone)
        return;

    shutdownDone = true;

    running = false;


    // ---- 先让 UI / 剧情侧丢掉对纹理的引用 ----

    for(int i = 0; i < 3; i++)
    {
        sprites[i].texture = nullptr;
        sprites[i].alpha = 0.0f;
        sprites[i].targetAlpha = 0.0f;
    }

    currentBackground = nullptr;


    // ---- 子系统（顺序不能改） ----

    saveSystem.ClearThumbnails();
    fontManager.Shutdown();
    resourceManager.Destroy();
    audioManager.Clean();
    renderer.Shutdown();


    // ---- 窗口 ----

    if(window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }


    // ---- 最后关闭库 ----

    TTF_Quit();
    SDL_Quit();
}



// ==========================================================
// CaptureThumbnail
// ==========================================================
//
// 把"最近一次渲染完成的画面"缩成 16:9 的存档缩略图，交给 SaveMenu 暂存。
//
// 注意：不能在按键处理里直接 SDL_RenderReadPixels。
// 那时当前帧还没画出来，读到的是上一帧甚至空白。
// 所以 Render() 每帧结束时顺手把像素缓存下来，这里只做缩放。

void Game::CaptureThumbnail()
{
    const int THUMB_W = 320;
    const int THUMB_H = 180;

    if(framePixels.empty() || frameW <= 0 || frameH <= 0)
        return;


    std::vector<unsigned char> thumb(
        (size_t)THUMB_W * THUMB_H * 4
    );

    for(int y = 0; y < THUMB_H; y++)
    {
        int sy = y * frameH / THUMB_H;
        if(sy >= frameH) sy = frameH - 1;

        for(int x = 0; x < THUMB_W; x++)
        {
            int sx = x * frameW / THUMB_W;
            if(sx >= frameW) sx = frameW - 1;

            size_t src = ((size_t)sy * frameW + sx) * 4;
            size_t dst = ((size_t)y * THUMB_W + x) * 4;

            thumb[dst + 0] = framePixels[src + 0];
            thumb[dst + 1] = framePixels[src + 1];
            thumb[dst + 2] = framePixels[src + 2];
            thumb[dst + 3] = 255;
        }
    }


    ui.GetSaveMenu().SetPendingThumbnail(thumb, THUMB_W, THUMB_H);
}




// ==========================================================
// GrabFrame
// ==========================================================
//
// 在 Present 之前把整帧读回来缓存。
// 只在剧情画面（DIALOGUE）下读，其他状态没意义。

void Game::GrabFrame()
{
    SDL_Renderer* r = renderer.GetSDLRenderer();

    if(!r)
        return;

    int w = config.GetWidth();
    int h = config.GetHeight();

    if(w <= 0 || h <= 0)
        return;

    size_t need = (size_t)w * h * 4;

    if(framePixels.size() != need)
    {
        framePixels.resize(need);
        frameW = w;
        frameH = h;
    }

    SDL_RenderReadPixels(
        r,
        nullptr,
        SDL_PIXELFORMAT_RGBA32,
        framePixels.data(),
        w * 4
    );
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
        std::vector<AffectionRecord> recs;

        if(ui.GetSaveMenu().Confirm(
            saveSystem,
            scriptFile,
            chapterName,
            idx,
            aff,
            recs
        ))
        {
            if(!scriptFile.empty() && story.Load(scriptFile))
            {
                if(idx >= 0 && idx < (int)story.events.size())
                {
                    story.GetRouteManager().SetAllAffection(aff);
                    story.GetRouteManager().SetRecords(recs);
                    story.SetIndex(idx);

                    // 读档后立绘也要清掉，重新按事件重建
                    for(int i = 0; i < 3; i++)
                    {
                        sprites[i].texture = nullptr;
                        sprites[i].alpha = 0.0f;
                        sprites[i].targetAlpha = 0.0f;
                    }

                    currentBackground = nullptr;
                    autoPlayTimer = 0.0f;

                    if(!ApplyCurrentEvent())
                        ReturnToTitle();
                    else
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

            // 新游戏必须清空上一轮的好感度与历史，
            // 否则二周目会带着上周目的数值直接锁死结局。
            story.GetRouteManager().SetAllAffection({});
            story.GetRouteManager().ClearRecords();

            // 立绘清干净，避免上一轮残留
            for(int i = 0; i < 3; i++)
            {
                sprites[i].texture = nullptr;
                sprites[i].alpha = 0.0f;
                sprites[i].targetAlpha = 0.0f;
            }

            currentBackground = nullptr;

            ui.GetDialogueUI().ClearChoice();
            currentChoiceTargets.clear();
            currentChoiceAffection.clear();
            currentChoiceTexts.clear();

            autoPlayTimer = 0.0f;

            if(!ApplyCurrentEvent())
                ReturnToTitle();
            else
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
            ui.GetAffectionMenu().Reset();   // [新增] 重置滚动
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
// ApplyCurrentEvent
// ==========================================================
//
// 把 story 的当前事件同步到画面上。
//
// 返回 false 表示剧情已经结束（事件带 isEnd，或者 story 已经走到头），
// 调用方负责回标题。
//
// 之前这段逻辑在 OnActivateCurrentState / OnAdvanceDialogue /
// ConfirmChoice 里各抄了一遍，四处不一致，顺手合并。

bool Game::ApplyCurrentEvent()
{
    StoryEvent e = story.GetCurrentEvent();


    // ---- 结束标记 ----

    if(e.isEnd)
    {
        return false;
    }


    UpdateScene();

    ui.GetDialogueUI().SetSpeaker(e.name);
    ui.GetDialogueUI().SetText(e.text, e.waitTime);


    if(e.isChoice)
    {
        ui.GetDialogueUI().ShowChoice(e.choices);
        currentChoiceTargets   = e.choiceTargets;
        currentChoiceAffection = e.choiceAffection;
        currentChoiceTexts     = e.choices;
    }
    else
    {
        ui.GetDialogueUI().ClearChoice();
        currentChoiceTargets.clear();
        currentChoiceAffection.clear();
        currentChoiceTexts.clear();
    }


    return true;
}




// ==========================================================
// ReturnToTitle
// ==========================================================

void Game::ReturnToTitle()
{
    ui.GetDialogueUI().ClearChoice();

    currentChoiceTargets.clear();
    currentChoiceAffection.clear();
    currentChoiceTexts.clear();

    // 背景保持最后一张，立绘淡出
    for(int i = 0; i < 3; i++)
        sprites[i].targetAlpha = 0.0f;

    autoPlayTimer = 0.0f;

    ui.SetState(UIState::START);
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


    if(!story.Next())
    {
        // 剧本真的走完了：回标题，别把最后一句挂在屏幕上
        ReturnToTitle();
        return;
    }


    StoryEvent e = story.GetCurrentEvent();


    // ---- 结局分支 ----

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


    if(!ApplyCurrentEvent())
    {
        ReturnToTitle();
    }
}



// ==========================================================
// OnBack
// ==========================================================

void Game::OnBack()
{
    UIState st = ui.GetState();

    if(st == UIState::DIALOGUE)
    {
        // 每次打开暂停菜单都回到第一项。
        // 不重置的话会保留上次的选择（比如上次点了"返回标题"，
        // 下次按 ESC 高亮块还停在那里，容易误触）。
        ui.GetPauseMenu().Reset();
        ui.SetState(UIState::PAUSE);
    }
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
        // 取选项文本作为来源描述
        std::string source;

        if(idx < (int)currentChoiceTexts.size())
            source = currentChoiceTexts[idx];


        for(auto& change : currentChoiceAffection[idx])
        {
            story.GetRouteManager().AddAffection(
                change.character,
                change.delta,
                source
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
            if(!ApplyCurrentEvent())
                ReturnToTitle();
        }
        else
        {
            // 标签写错时以前是静默卡住，玩家只会觉得"点了没反应"。
            // 这里退化成继续推进，至少不会锁死。
            std::cout
                << "跳转失败，标签不存在: #"
                << labelName
                << std::endl;

            OnAdvanceDialogue();
        }
        return;
    }


    if(story.Load(target))
    {
        if(!ApplyCurrentEvent())
            ReturnToTitle();
    }
    else
    {
        std::cout << "剧本加载失败: " << target << std::endl;

        OnAdvanceDialogue();
    }
}