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
FourYears/                 ← git 仓库根
│
├── Makefile              跨平台构建脚本
├── README.md
├── .gitattributes        行尾统一 LF
├── .gitignore
├── .vscode/              VS Code 配置
│
├── FourYears/            游戏本体
│   ├── main.cpp
│   ├── config.ini
│   ├── GameState.h
│   ├── SavePage.h
│   ├── SDL2*.dll
│   │
│   ├── core/             引擎核心
│   │   ├── Game.cpp / .h
│   │   ├── Renderer.cpp / .h
│   │   ├── ResourceManager.cpp / .h
│   │   ├── FontManager.cpp / .h
│   │   ├── TextSystem.cpp / .h
│   │   ├── ScriptPlayer.cpp / .h       （保留，未使用）
│   │   ├── SaveSystem.cpp / .h
│   │   ├── AudioManager.cpp / .h
│   │   ├── Config.cpp / .h
│   │   └── Utils.cpp / .h
│   │
│   ├── story/            剧情数据
│   │   ├── Story.cpp / .h
│   │   ├── StoryParser.cpp / .h
│   │   ├── Character.cpp / .h
│   │   ├── History.cpp / .h            （保留，未使用）
│   │   └── RouteManager.cpp / .h
│   │
│   ├── ui/               界面
│   │   ├── UIManager.cpp / .h
│   │   ├── MenuCommon.h
│   │   ├── StartMenu.cpp / .h
│   │   ├── PauseMenu.cpp / .h
│   │   ├── DialogueUI.cpp / .h
│   │   ├── SaveMenu.cpp / .h
│   │   ├── AffectionMenu.cpp / .h
│   │   └── ConfigMenu.cpp / .h
│   │
│   ├── resource/
│   │   ├── bg/           背景图
│   │   ├── character/    角色立绘
│   │   └── font/         字体
│   │
│   ├── script/           剧本（UTF-8）
│   │   └── chapter01.txt 等
│   │
│   └── save/             存档（运行时生成）
│
└── release/              打包目录（git 忽略）
```

---

# Windows（MSYS2 UCRT64）

## 安装 MSYS2

下载并安装：

https://www.msys2.org/

## 更新软件源

    pacman -Syu

## 安装依赖

    pacman -S \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-SDL2 \
    mingw-w64-ucrt-x86_64-SDL2_image \
    mingw-w64-ucrt-x86_64-SDL2_ttf \
    mingw-w64-ucrt-x86_64-SDL2_mixer \
    make

---

# Ubuntu / Debian

    sudo apt update
    sudo apt install \
    g++ make pkg-config \
    libsdl2-dev libsdl2-image-dev \
    libsdl2-ttf-dev libsdl2-mixer-dev

---

# Arch Linux

    sudo pacman -Syu
    sudo pacman -S gcc make pkg-config sdl2 sdl2_image sdl2_ttf sdl2_mixer

---

# Fedora

    sudo dnf install gcc-c++ make pkg-config \
    SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel

---

# 编译

## 一键编译

在仓库根目录执行：

    make

`Makefile` 会自动判断平台：

- **Windows（MSYS2 UCRT64）**：使用 `C:/msys64/ucrt64/bin/g++.exe`
- **Linux**：使用系统 `g++` + `pkg-config`

输出：

- Windows：`FourYears/game.exe`
- Linux：`game`

## 常用命令

    make         编译
    make run     编译并运行
    make clean   清理
    make rebuild 清理后重编

## VS Code

`.vscode/tasks.json` 已配置为直接调用 `make.exe`：

- `Ctrl + Shift + B` —— 编译
- `Ctrl + Shift + P` → `Tasks: Run Task` → `Run FourYears` / `Clean FourYears`

`.vscode/settings.json` 已配置集成终端默认使用 **MSYS2 UCRT64**。

## 注意事项

- **改了 `.h` 文件**：Makefile 目前没有头文件依赖追踪，需要手动 `make clean && make`。
- **行尾必须是 LF**：`.gitattributes` 已强制，Makefile 里如果有 CRLF 会导致编译失败（`make` 把 `\r` 传给 g++）。

---

# 运行

## Windows

    cd FourYears
    ./game.exe

## Linux

    ./game

---

# 操作说明

## 主菜单

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 确认 | Enter 或鼠标点击 |

4 项：开始游戏 / 存档 / 设置 / 退出游戏。

## 剧情对话

| 操作 | 键鼠 |
|---|---|
| 跳过当前文字 | Space 或鼠标左键 |
| 推进到下一段 | Space 或鼠标左键（文字已显示完） |
| 打开暂停菜单 | ESC |
| 选择分支 | ↑ / ↓ 或鼠标悬停切换，Enter 或鼠标点击确认 |

出现选项时，Space 和鼠标左键**不会推进剧情**。

## 暂停菜单

6 项：继续游戏 / 保存游戏 / 读取存档 / 好感度 / 设置 / 返回标题。

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

**删除**为两段式：按 Delete 后进入确认，再按 Enter 或 Delete 才真正删除，ESC 取消。

**重命名**进入输入模式：
- **支持中文输入**（输入法预编辑拼音会显示在输入框下方）
- 字母、数字、空格追加
- 退格按 UTF-8 字符边界删除
- Enter 确认，ESC 取消

## 好感度页面

显示当前所有角色的好感度数值。只读，ESC 返回。

## 设置页面

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 调整数值 | ← / → |
| 保存 | S |
| 返回 | ESC 或点击「返回 [ESC]」 |

**退出流程**：

- 没有未保存改动 → ESC 直接返回
- 有未保存改动 → 弹窗询问
  - **Y** 保存并返回
  - **N** 不保存返回（还原进设置时的配置）
  - **ESC** 取消询问

| 选项 | 范围 | 效果 |
|---|---|---|
| BGM 音量 | 0 ~ 100，步长 5 | 立即设置 SDL_mixer 音乐音量 |
| SE 音量 | 0 ~ 100，步长 5 | 立即设置 SDL_mixer 音效音量 |
| 文字速度 | 5 ~ 100 字/秒，步长 5 | 打字机速度立即变化 |
| 全屏 | 开 / 关 | 立即切换全屏模式 |
| 自动播放 | 开 / 关 | 文字显示完后等 1.5 秒自动推进 |

---

# 剧本格式

剧本是 UTF-8 纯文本，放在 `FourYears/script/` 下。

## 标签表

| 标签 | 写法 | 作用 |
|---|---|---|
| `[背景]` | `[背景] xxx.png` 或两行 | 切换背景 |
| `[立绘]` | `[立绘] xxx.png` 或两行 | 切换立绘 |
| `[BGM]` | 同上 | 预留 |
| `[SE]` | 同上 | 预留 |
| `[角色名]` | `[李君浩]` | 开始一段对话 |
| `<n>` | `<1.5>` | 等待 n 秒 |
| `[选择]` | 见下 | 进入选择事件 |
| `[标签]` | `[标签] name` | 打书签 |
| `[跳转]` | `[跳转] #name` | 跳到书签 |
| `[下一章]` | `[下一章] script/xxx.txt` | 播完自动加载下一章 |
| `[结局分支]` | 见下 | 根据好感度自动选结局 |

带参数的标签**单行和两行写法都支持**。

## 立绘顺序约定

**`[立绘]` 必须紧跟在对应角色的 `[角色名]` 之前，中间不能夹别的 `[角色名]`。**

## 选择语法

```
[选择]

1. 显示文本 | 好感度变化 -> 跳转目标

2. 显示文本
```

- `|` 后是好感度变化，可多个用 `,` 分隔，如 `李君浩 +10, 张瀚宇 -5`
- `->` 后是跳转目标：`#标签名` 跳同文件；`script/xxx.txt` 加载外部脚本；省略则不跳转
- 三段中只有显示文本必填

## 标签与跳转

```
[选择]

1. A 选项 | 李君浩 +10 -> #branch_a

2. B 选项 -> #branch_b

[标签] branch_a
... A 分支 ...
[跳转] #join

[标签] branch_b
... B 分支 ...
[跳转] #join

[标签] join
... 合流后主线 ...
```

标签名不要重名。

## 结局分支

```
[结局分支]

李君浩 -> script/li_junhao_route.txt

张瀚宇 -> script/zhang_hanyu_route.txt

单人 -> script/alone_ending.txt
```

到达这个事件时，根据当前好感度自动判断：

- 李君浩 ≥ 20 且高于张瀚宇 → 李君浩线
- 张瀚宇 ≥ 20 且高于李君浩 → 张瀚宇线
- 都 < 20 或相等 → 单人结局

阈值在 `RouteManager::CheckRoute()` 里。

## 章节切换

```
[下一章]
script/chapter02.txt
```

路径要相对工作目录 `FourYears/` 写全。

---

# 存档

目录：`FourYears/save/`

命名：`save_<毫秒时间戳>.dat`

## 文件格式（6 行）

```
显示名
脚本路径
章节显示名
index
时间
好感度（李君浩:10,张瀚宇:15）
```

## 章节显示名

`Game.cpp` 顶部 `ChapterNameFromScript()` 根据脚本路径推算。新增章节时在这里加一条映射。

## 兼容性

**格式已变更多次，旧存档不兼容。** 升级后请删除 `save/` 下所有 `.dat` 文件。

---

# 当前工程状态

## 已实现

- SDL2 初始化、窗口、渲染器
- 资源管理（纹理缓存、按需加载）
- 字体管理（多字号缓存）
- 配置读取与保存，修改即时生效
- 音频管理（BGM / SE 音量已接入 SDL_mixer）
- 剧本解析
  - `[背景]` `[立绘]` `[BGM]` `[SE]` `[选择]` `[标签]` `[跳转]` `[下一章]` `[结局分支]` `[角色名]`
  - 单行 / 两行标签参数写法
- 剧情播放
- **章节切换**
- **分支剧情**（标签 + 跳转）
- **选择系统**（键盘 + 鼠标）
- **好感度系统**（累积、结局判定、存进存档、独立页面）
- 打字机效果（标点停顿、中文省略号替换、`\n` 强制换行）
- 主菜单、暂停菜单、存档、设置、好感度
- 键鼠双输入
- 立绘固定高度 + 宽高比缩放 + 右中侧居中
- 对话框深灰半透明
- 二级菜单背景模糊
- 设置页面 S 保存 + ESC 询问
- 全屏切换（`SDL_RenderSetLogicalSize` 自动缩放）
- 自动播放
- **存档重命名支持中文输入**（拼音预编辑提示）
- **Windows 输入法智能禁用/恢复**（输入框激活时启用 IME，其他时候禁用）
- **跨平台 Makefile**（Windows / Linux 同一套命令）

---

# 注意事项

## 工作目录

从 `FourYears/` 目录运行，因为 `resource/` `script/` `save/` `config.ini` 都是相对路径。

## Windows DLL

运行时需要：

    SDL2.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_ttf.dll

以及 `ntldd` 输出的 MinGW 运行时和第三方依赖（见「发布打包」一节）。

---

# 编码规范

源代码和剧情文本统一 UTF-8。

Windows 编译必须加：

    -finput-charset=UTF-8
    -fexec-charset=UTF-8

C++ 标准：C++17。Makefile 已配置。

---

# 输入法

Windows 中文输入法激活时，字母键会被 IME 拦截，SDL 收不到 `SDLK_n` / `SDLK_c` / `SDLK_r` / `SDLK_s` 的 KEYDOWN。

**解决方案**：`Game::SetInputMode(bool)` 动态切换。

- **平时**（`enabled=false`）：`ImmAssociateContext(hwnd, NULL)` 切断 IME + `SDL_StopTextInput()`
- **输入框激活**（`enabled=true`）：`ImmAssociateContext(hwnd, oldHimc)` 恢复 IME + `SDL_StartTextInput()`

配合 `SDL_TEXTINPUT` 接收已上屏文本、`SDL_TEXTEDITING` 接收拼音预编辑文本。

**候选词窗问题**：SDL2 在游戏窗口里通常不显示 IME 候选词窗。当前方案用**输入框下方的拼音提示**代替：

```
重命名: 新的存档_
拼音: nihao
```

玩家能看到自己在打什么拼音。缺点：看不到候选字列表，选字靠拼音准确度。

---

# 字体与中文显示

项目使用 `resource/font/simhei.ttf`。另外有 `title.ttf`（标题）和 `number.ttf`（数字）。

字体缺失字形时会渲染成方框。已知位置和当前处理：

| 位置 | 字符 | 处理 |
|---|---|---|
| `TextSystem::SetText` | `…` | 替换成 `.` |
| `DialogueUI::SplitTextLine` | `\n` | 识别为强制换行 |
| `AffectionMenu::Render` | — | 只显示中文名和数字 |

如果脚本里用到其它字体缺失字符（如 `——` `「」` `·`），需要在 `TextSystem::SetText` 里继续加替换。

---

# 已知问题 / 待完善

## 存档

- **新建名字固定**：每次都是「新的存档」。
- **章节名靠硬编码**：新增章节时需要在 `ChapterNameFromScript()` 里加映射。

## 构建

- **Makefile 没有头文件依赖追踪**：改 `.h` 后需要手动 `make clean && make`。
- **VS Code 集成终端需要重启才生效**：改 `settings.json` 后要 `Reload Window` 或杀掉旧终端。

## 配置

- `config.ini` 的 `[resource]` 段在游戏内保存设置后会丢失。
- 全屏时因为逻辑分辨率 1600×900 → 物理 1920×1080 是 1.2 倍非整数缩放，文字会**轻微模糊**。

## 代码清理

- `core/ScriptPlayer.cpp / .h` 未被使用。
- `story/History.cpp / .h` 仍被编译，UI 层已改用好感度页面。
- `GameState.h` 里的 `enum class GameState` 是死代码，真正的状态机在 `ui/UIManager.h` 的 `UIState`。

---

# 发布打包

见仓库 `.gitignore` 已忽略 `release/`。

## 1. 编译发布版

    cd FourYears
    g++ -std=c++17 -O2 -s -finput-charset=UTF-8 -fexec-charset=UTF-8 \
        main.cpp \
        core/*.cpp story/*.cpp ui/*.cpp \
        -o game.exe \
        -IC:/msys64/ucrt64/include/SDL2 \
        -I. -Icore -Istory -Iui \
        -LC:/msys64/ucrt64/lib \
        -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

> 注：发布用 `-O2 -s`，日常 `make` 不带 `-s`。

## 2. 收集文件

    cd /f/gal
    mkdir -p release/FourYears/save
    cp FourYears/game.exe     release/FourYears/
    cp FourYears/config.ini   release/FourYears/
    cp -r FourYears/resource  release/FourYears/
    cp -r FourYears/script    release/FourYears/
    touch release/FourYears/save/.gitkeep

## 3. 复制依赖 DLL

    cd /f/gal/release/FourYears
    ntldd -R game.exe | grep "msys64" | awk '{print $3}' | while read -r dll; do
        cp "$dll" .
    done

`ntldd` 没装：

    pacman -S mingw-w64-ucrt-x86_64-ntldd

## 4. 打包

    cd /f/gal/release
    tar -a -c -f FourYears-vX.Y.Z.zip FourYears

## 5. 上传

GitHub Releases → Create a new release → 拖入 zip。

---

# 开发计划

1. **BGM / SE 接入实际资源**（脚本标签已解析，缺音频文件和调用）
2. **立绘系统升级**（多立绘同屏 + 位置 + 淡入淡出）
3. **剧情内容扩充**（现在只有 2 章）
4. **新建存档自定义名**
5. **剧情状态回退**（读档后 `History` 一起恢复）
6. **中文标点替换表**（`——` `「」` `·`）
7. **视觉效果**（转场、淡入淡出、立绘动画）
8. **存档缩略图**

---

# 给接手者的说明

- 先理解现有架构再改，不要因为一个 Bug 就整体重写。
- 优先小范围修改 + 编译验证，不要同时动多个无关系统。
- `.h` 声明与 `.cpp` 实现必须一致。
- 关键链路：`SaveSystem ↔ SaveMenu ↔ Game`。
- 状态机在 `ui/UIManager.h` 的 `UIState`。
- 所有键鼠输入汇聚到 `Game::OnActivateCurrentState()` / `OnAdvanceDialogue()` / `OnBack()` / `ConfirmChoice()` 四个函数。
- 剧本里 `[立绘]` 必须在对应角色台词**之前**。
- 存档格式变化后**必须删旧存档**。
- **Makefile 行尾必须是 LF**（CRLF 会导致编译失败且无错误提示）。
- **改了 `.h` 要手动 clean**。