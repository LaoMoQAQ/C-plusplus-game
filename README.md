# Four Years

一款使用 C++17 与 SDL2 开发的中文视觉小说（Galgame）项目。

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

FourYears/

├── core/

│   ├── AudioManager.cpp / AudioManager.h

│   ├── Config.cpp / Config.h

│   ├── FontManager.cpp / FontManager.h

│   ├── Game.cpp / Game.h

│   ├── Renderer.cpp / Renderer.h

│   ├── ResourceManager.cpp / ResourceManager.h

│   ├── SaveSystem.cpp / SaveSystem.h

│   ├── ScriptPlayer.cpp / ScriptPlayer.h

│   ├── TextSystem.cpp / TextSystem.h

│   └── Utils.cpp / Utils.h

│

├── story/

│   ├── Character.cpp / Character.h

│   ├── History.cpp / History.h

│   ├── RouteManager.cpp / RouteManager.h

│   ├── Story.cpp / Story.h

│   └── StoryParser.cpp / StoryParser.h

│

├── ui/

│   ├── ConfigMenu.cpp / ConfigMenu.h

│   ├── DialogueUI.cpp / DialogueUI.h

│   ├── HistoryMenu.cpp / HistoryMenu.h

│   ├── PauseMenu.cpp / PauseMenu.h

│   ├── SaveMenu.cpp / SaveMenu.h

│   ├── StartMenu.cpp / StartMenu.h

│   └── UIManager.cpp / UIManager.h

│

├── resource/

│   ├── bg/

│   ├── character/

│   └── font/

│

├── script/

│   ├── chapter01.txt

│   ├── chapter02.txt

│   ├── ending.txt

│   ├── alone_ending.txt

│   ├── li_junhao_route.txt

│   └── zhang_hanyu_route.txt

│

├── save/

│

├── .vscode/

│   ├── c_cpp_properties.json

│   └── tasks.json

│

├── GameState.h

├── SavePage.h

├── config.ini

└── main.cpp

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

---

# 运行

## Windows

    .\game.exe

## Linux

    ./game

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