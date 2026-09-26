# Four Years

一款使用 C++17 与 SDL2 开发的中文视觉小说（Galgame）项目。

> 四年 · 我们的青春

---

# 环境部署与编译

## 开发环境

| 项目 | 版本 |
|------|------|
| 开发语言 | C++17 |
| 编译器 | GCC (g++) |
| 图形库 | SDL2 |
| 图片库 | SDL2_image |
| 字体库 | SDL2_ttf |
| 音频库 | SDL2_mixer |
| IDE | Visual Studio Code |
| 构建环境 | MSYS2 UCRT64 |

---

# 项目结构

```text
FourYears/
│
├── core/                 引擎核心：主循环、渲染、资源、存档、配置
│   ├── Game.cpp / .h
│   ├── Renderer.cpp / .h
│   ├── ResourceManager.cpp / .h
│   ├── FontManager.cpp / .h
│   ├── TextSystem.cpp / .h
│   ├── ScriptPlayer.cpp / .h
│   ├── SaveSystem.cpp / .h
│   ├── AudioManager.cpp / .h
│   ├── Config.cpp / .h
│   └── Utils.cpp / .h
│
├── story/                剧情数据：解析、角色、路线、历史
│   ├── Story.cpp / .h
│   ├── StoryParser.cpp / .h
│   ├── Character.cpp / .h
│   ├── History.cpp / .h
│   └── RouteManager.cpp / .h
│
├── ui/                   界面：各菜单页面与状态切换
│   ├── UIManager.cpp / .h
│   ├── MenuCommon.h
│   ├── StartMenu.cpp / .h
│   ├── PauseMenu.cpp / .h
│   ├── DialogueUI.cpp / .h
│   ├── SaveMenu.cpp / .h
│   ├── HistoryMenu.cpp / .h
│   └── ConfigMenu.cpp / .h
│
├── resource/             资源
│   ├── bg/               背景图
│   ├── character/        角色立绘
│   └── font/             字体
│
├── script/               剧本（UTF-8）
│   └── chapter01.txt 等
│
├── save/                 存档（运行时生成）
│
├── .vscode/              编译与 IntelliSense 配置
│
├── main.cpp
├── config.ini
├── GameState.h
├── SavePage.h
└── SDL2*.dll
```

---

# Windows（MSYS2 UCRT64）

## 安装 MSYS2

下载并安装：

https://www.msys2.org/

安装完成后打开：

MSYS2 UCRT64

## 更新软件源

执行：

    pacman -Syu

如果提示关闭终端，请关闭后重新打开 MSYS2 UCRT64，然后再次执行：

    pacman -Syu

## 安装依赖

    pacman -S \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-SDL2 \
    mingw-w64-ucrt-x86_64-SDL2_image \
    mingw-w64-ucrt-x86_64-SDL2_ttf \
    mingw-w64-ucrt-x86_64-SDL2_mixer

---

# Ubuntu / Debian

## 安装依赖

    sudo apt update

    sudo apt install \
    g++ \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev \
    libsdl2-mixer-dev

---

# Arch Linux

## 安装依赖

    sudo pacman -Syu

    sudo pacman -S \
    gcc \
    sdl2 \
    sdl2_image \
    sdl2_ttf \
    sdl2_mixer

---

# Fedora

## 安装依赖

    sudo dnf install \
    gcc-c++ \
    SDL2-devel \
    SDL2_image-devel \
    SDL2_ttf-devel \
    SDL2_mixer-devel

---

# 编译

## Windows（MSYS2 UCRT64）

当前项目由多个 C++ 源文件组成，需要将 core、story 和 ui 中的源文件一起编译。

在项目根目录执行：

    C:\msys64\ucrt64\bin\g++.exe -std=c++17 -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp core\Game.cpp core\Renderer.cpp core\ResourceManager.cpp core\TextSystem.cpp core\ScriptPlayer.cpp core\SaveSystem.cpp core\AudioManager.cpp core\FontManager.cpp core\Config.cpp core\Utils.cpp story\Character.cpp story\History.cpp story\RouteManager.cpp story\Story.cpp story\StoryParser.cpp ui\UIManager.cpp ui\DialogueUI.cpp ui\StartMenu.cpp ui\PauseMenu.cpp ui\SaveMenu.cpp ui\HistoryMenu.cpp ui\ConfigMenu.cpp -o game.exe -IC:\msys64\ucrt64\include\SDL2 -I. -Icore -Istory -Iui -LC:\msys64\ucrt64\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

也可以使用 VS Code 的编译任务：

    Ctrl + Shift + B

---

## Linux（Ubuntu / Arch / Fedora）

    g++ -std=c++17 main.cpp core/Game.cpp core/Renderer.cpp core/ResourceManager.cpp core/TextSystem.cpp core/ScriptPlayer.cpp core/SaveSystem.cpp core/AudioManager.cpp core/FontManager.cpp core/Config.cpp core/Utils.cpp story/Character.cpp story/History.cpp story/RouteManager.cpp story/Story.cpp story/StoryParser.cpp ui/UIManager.cpp ui/DialogueUI.cpp ui/StartMenu.cpp ui/PauseMenu.cpp ui/SaveMenu.cpp ui/HistoryMenu.cpp ui/ConfigMenu.cpp -o game -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

> 注：Windows 专用的输入法禁用逻辑（见「输入法」一节）通过 `#ifdef _WIN32` 包裹，Linux 编译时会自动跳过。

---

# 运行

## Windows

    .\game.exe

## Linux

    ./game

---

# 操作说明

## 主菜单

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 确认 | Enter 或鼠标点击 |

## 剧情对话

| 操作 | 键鼠 |
|---|---|
| 跳过当前文字 | Space 或鼠标左键 |
| 推进到下一段 | Space 或鼠标左键（文字已显示完） |
| 打开暂停菜单 | ESC |

## 暂停菜单

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 确认 | Enter 或鼠标点击 |
| 返回 | ESC 或点击「返回 [ESC]」 |

## 存档页面

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 读取存档 | Enter 或点击存档项 |
| 新建存档 | N |
| 复制存档 | C |
| 删除存档 | Delete |
| 重命名存档 | R |
| 返回 | ESC 或点击「返回 [ESC]」 |

删除存档为**两段式**：按 Delete 后进入确认状态，底部提示「确认删除？Enter 确认 / ESC 取消」。再按 Enter 或 Delete 才会真正删除，按 ESC 取消。鼠标点击存档项等同确认删除。

## 设置页面

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 调整数值 | ← / → |
| 保存设置 | Enter（选中「保存设置」时）或点击该项 |
| 返回 | ESC 或点击「返回 [ESC]」 |

## 历史记录

| 操作 | 键鼠 |
|---|---|
| 上下滚动 | ↑ / ↓ |
| 返回 | ESC 或点击「返回 [ESC]」 |

---

# 当前工程状态

目前已经具备一个基本可运行的视觉小说框架。

核心模块：

    C++17
       │
       └── SDL2
            ├── SDL2_image
            ├── SDL2_ttf
            └── SDL2_mixer

            ↓

        Four Years

        ┌───────────────┐
        │    Game       │
        └───────┬───────┘
                │
        ┌───────┼────────┐
        ↓       ↓        ↓
      Story     UI      Save
        │       │        │
        ↓       ↓        ↓
     剧情系统  菜单系统  存档系统

## 已实现

- SDL2 初始化、窗口、渲染器
- 资源管理（纹理缓存）
- 字体管理（多字号缓存，标题 / 副标题 / 正文各自字号）
- 配置读取与保存（`config.ini`）
- 音频管理（BGM / SE，暂未接入实际资源）
- 剧本解析（`StoryParser`）
  - 支持 `[背景]`、`[立绘]`、`[BGM]`、`[SE]`、`[选择]`、`[角色名]`
  - 支持 `<>` 等待标记
  - 自动清理 BOM / `\r`
- 剧情播放（`Story` + `DialogueUI` + `TextSystem`）
- 打字机效果
  - 标点额外停顿
  - 中文省略号 `…` 替换
  - `\n` 强制换行处理（避免渲染成方框）
- 主菜单（主标题 + 副标题 + 4 项菜单）
- 暂停菜单（6 项）
- 存档系统
  - 新建 / 复制 / 删除（含二次确认） / 读取
  - 列表显示 `显示名 [第N章] 时间`
  - 按保存时间倒序
  - 自动跳过 0 字节空文件
  - 毫秒级时间戳命名，避免同秒覆盖
- 设置菜单（BGM / SE 音量、文字速度、全屏、自动播放、保存）
- 历史记录
- 键鼠双输入
- 二级菜单背景模糊（近似高斯：缩小再放大）
  - START 进入 → 主菜单背景
  - PAUSE 进入 → 当前剧情背景
- Windows 输入法禁用（避免中文输入法拦截字母键）

---

# 注意事项

## 工作目录

程序运行时需要保证以下目录和文件能够被正确访问：

    resource/
    script/
    save/
    config.ini

推荐从项目根目录运行：

    F:\gal\FourYears

---

# Windows DLL

Windows 运行时需要 SDL2 相关 DLL。

当前项目目录包含：

    SDL2.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_ttf.dll

运行 game.exe 时必须确保这些 DLL 能够被程序找到。

---

# 编码规范

项目源代码统一使用：

    UTF-8

剧情文本统一使用：

    UTF-8

Windows 编译建议使用：

    -finput-charset=UTF-8
    -fexec-charset=UTF-8

C++ 标准：

    C++17

---

# VS Code

项目使用 Visual Studio Code 进行开发。

编译任务位于：

    .vscode/tasks.json

C/C++ 配置位于：

    .vscode/c_cpp_properties.json

使用：

    Ctrl + Shift + B

可以调用项目编译任务。

---

# 输入法

Windows 中文输入法激活时，字母键会被 IME 拦截，SDL 收不到 `SDLK_n` / `SDLK_c` / `SDLK_r` 的 KEYDOWN，导致游戏内字母快捷键全部失效。Enter / ESC / 方向键不受影响。

`Game::Init()` 中做了两层处理：

1. **SDL 层**：`SDL_SetHint(SDL_HINT_IME_SHOW_UI, "0")` + `SDL_StopTextInput()` + `SDL_EventState(SDL_TEXTINPUT/SDL_TEXTEDITING, SDL_DISABLE)`
2. **Win32 层**：通过 `SDL_GetWindowWMInfo` 拿到底层 `HWND`，动态加载 `imm32.dll`，调用 `ImmAssociateContext(hwnd, NULL)` 切断窗口与 IME 的关联

这样即使中文输入法激活，字母键也能正常送到 SDL。

后续做文本输入框时，需要临时恢复 IME：

    ImmAssociateContext(hwnd, oldHimc);   // 恢复
    SDL_StartTextInput();

关闭输入框时再切断：

    ImmAssociateContext(hwnd, NULL);
    SDL_StopTextInput();

因此做输入框那一步时，需要把 `HWND` 和旧的 `HIMC` 保存为 `Game` 的成员变量。

---

# 字体与中文显示

项目使用 `resource/font/simhei.ttf`（黑体，UTF-8 中文字体）。

另外有 `title.ttf`（标题）和 `number.ttf`（数字）。

字体缺失字形时会渲染成方框。当前处理方式：

- `TextSystem::SetText` 里把中文省略号 `…` 替换成 `.`
- `DialogueUI::SplitTextLine` 把 `\n` 识别为强制换行，不把它传给 `SDL_ttf`

如果脚本里用到其它字体缺失字符（如 `——`、`「」`、`·`），需要在 `TextSystem::SetText` 的替换表里继续加分支。**不要仅仅因为出现方框就立即换字体。**

---

# 已知问题 / 待完善

## 存档

- **重命名只是占位**：只改文件名，不改文件第一行的显示名，UI 上看到的还是旧名。真正生效需要文本输入框。
- **新建名字固定**：每次新建都是「新的存档」，无法自定义。
- **`chapter` 字段未被使用**：当前只加载 `chapter01.txt`，读档时不按 `chapter` 切章节。

## 章节切换

`Story` 目前只加载 `script/chapter01.txt`。剧情推进到末尾后 `Next()` 返回 `false`，不会自动加载下一章。`chapter02.txt` 等文件已存在但未接入。

## 配置

`Config.cpp` 读取的键名（`bgm` / `se` / `textSpeed` / `fullscreen` / `autoPlay`）与初始 `config.ini` 中的键名不完全一致，因此首次启动时部分配置读不到，会使用默认值。`[resource]` 段在 `Save` 后会丢失。

## 代码清理

`ui/DialogueUI.cpp` 里有两份 `SplitTextLine`：一个自由函数、一个成员函数。自由函数是死代码，未被调用，可以安全删除。

`GameState.h` 里的 `enum class GameState` 也是死代码，没有任何地方使用，真正的状态机在 `ui/UIManager.h` 的 `UIState`。

## 模糊效果

当前模糊是「缩小再放大」的近似高斯，`Renderer.h` 里 `BLUR_DIVISOR = 4`。像素感偏重时可以：

- 增大 `BLUR_DIVISOR`（更糊）
- 改成多遍降采样（代码更多、更平滑）
- 或对静态背景预先出一张模糊图

---

# 开发计划

按优先级：

1. **重命名输入框**（引入 `SDL_TEXTINPUT`，临时恢复 IME）
2. **新建存档自定义名**
3. **章节切换**（`Story` 多章加载 + 脚本跳转指令）
4. **剧情状态恢复**（读档后 `History` 也一起恢复）
5. **中文显示优化**（标点替换表、描边、字距）
6. **视觉效果**（转场、淡入淡出、立绘动画）
7. **存档缩略图**

---

# 给接手者的说明

- 先理解现有架构再改，不要因为一个 Bug 就整体重写。
- 优先小范围修改 + 编译验证，不要同时动多个无关系统。
- `.h` 声明与 `.cpp` 实现必须一致，注意不要留下重复定义。
- 关键链路：`SaveSystem ↔ SaveMenu ↔ Game`。
- 状态机在 `ui/UIManager.h` 的 `UIState`，不在 `GameState.h`（后者是死代码）。
- 所有键鼠输入最终汇聚到 `Game::OnActivateCurrentState()` / `Game::OnAdvanceDialogue()` / `Game::OnBack()` 三个函数，避免逻辑分叉。
- 菜单坐标统一放在各菜单 `.h` 的 `static constexpr` 或 `ui/MenuCommon.h`，`Render` 和鼠标命中检测共用同一份常量，避免「画在这、点在那」。