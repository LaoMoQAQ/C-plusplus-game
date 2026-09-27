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

    sudo apt update

    sudo apt install \
    g++ \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev \
    libsdl2-mixer-dev

---

# Arch Linux

    sudo pacman -Syu

    sudo pacman -S \
    gcc \
    sdl2 \
    sdl2_image \
    sdl2_ttf \
    sdl2_mixer

---

# Fedora

    sudo dnf install \
    gcc-c++ \
    SDL2-devel \
    SDL2_image-devel \
    SDL2_ttf-devel \
    SDL2_mixer-devel

---

# 编译

## Windows（MSYS2 UCRT64）

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

主菜单 4 项：开始游戏 / 存档 / 设置 / 退出游戏。

## 剧情对话

| 操作 | 键鼠 |
|---|---|
| 跳过当前文字 | Space 或鼠标左键 |
| 推进到下一段 | Space 或鼠标左键（文字已显示完） |
| 打开暂停菜单 | ESC |
| 选择分支 | ↑ / ↓ 切换，Enter 确认 |

出现选项时，Space 和鼠标左键**不会推进剧情**，必须用 ↑ / ↓ + Enter。

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

**删除**为两段式：按 Delete 后进入确认状态，再按 Enter 或 Delete 才真正删除，ESC 取消。

**重命名**进入输入模式：
- 字母 / 数字 / 空格 追加
- 退格删除（支持中文边界删除）
- Enter 确认，ESC 取消
- **只支持英文和数字**，不支持中文输入

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

# 剧本格式

剧本是 UTF-8 纯文本，放在 `script/` 下。解析器支持以下标签：

| 标签 | 作用 |
|---|---|
| `[背景]` | 下一行写背景图文件名 |
| `[立绘]` | 下一行写立绘路径 |
| `[BGM]` | 预留，下一行写 BGM 路径 |
| `[SE]` | 预留，下一行写音效路径 |
| `[选择]` | 进入选择事件，后面跟若干条选项 |
| `[下一章]` | 下一行写下一个脚本路径，播完自动切换 |
| `[角色名]` | 普通对话，下一段是台词 |
| `<n>` | 等待 n 秒后再开始逐字显示 |

## 立绘顺序约定

**`[立绘]` 必须紧跟在对应角色的 `[角色名]` 之前，中间不能夹别的 `[角色名]`。**

正确：

```
[立绘]
li_junhao/normal.png

[李君浩]
看你的样子，好像找不到教学楼。
```

错误：

```
[李君浩]
看你的样子，好像找不到教学楼。

[立绘]
li_junhao/normal.png
```

错误写法会导致立绘滞后一句。

## 选择格式

```
[选择]

1. 陪伴四年 -> script/li_junhao_route.txt

2. 未来同行 -> script/zhang_hanyu_route.txt

3. 一个人的成长
```

`->` 前是显示文本，后是跳转目标。没有 `->` 的选项只显示、不跳转。

## 章节切换

在脚本末尾加：

```
[下一章]
script/chapter02.txt
```

播完当前脚本最后一句后，按 Space 会自动加载下一章。

---

# 存档

存档目录：`save/`

文件命名：`save_<毫秒时间戳>.dat`

## 文件格式

```
显示名
脚本路径
章节显示名
index
时间
```

例：

```
新的存档
script/chapter01.txt
第1章
7
2026-09-27 15:30:00
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
- 配置读取与保存（`config.ini`）
- 音频管理（BGM / SE，暂未接入实际资源）
- 剧本解析
  - 支持 `[背景]`、`[立绘]`、`[BGM]`、`[SE]`、`[选择]`、`[下一章]`、`[角色名]`
  - 支持 `<>` 等待标记
  - 自动清理 BOM / `\r`
- 剧情播放（`Story` + `DialogueUI` + `TextSystem`）
- **章节切换**：脚本末尾 `[下一章]` 自动加载下一章
- **选择系统**：选项显示在对话框内，↑ / ↓ 切换，Enter 确认跳转
- 打字机效果（标点停顿、中文省略号替换、`\n` 强制换行）
- 主菜单（标题 + 副标题 + 4 项菜单）
- 暂停菜单（6 项）
- 存档系统
  - 新建 / 复制 / 删除（含二次确认） / 读取 / 重命名
  - 列表显示 `显示名 [章节名] 时间`，按保存时间倒序
  - 保存脚本路径 + index，支持跨章节读档
  - 自动跳过 0 字节空文件
  - 毫秒级时间戳命名
- 设置菜单（BGM / SE 音量、文字速度、全屏、自动播放、保存）
- 历史记录（自动清理方框字符）
- 键鼠双输入
- 立绘固定高度 + 宽高比缩放 + 右中侧居中
- 对话框：深灰半透明矩形
- 二级菜单背景模糊（近似高斯）
- Windows 输入法禁用

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

## Windows DLL

运行时需要 SDL2 相关 DLL：

    SDL2.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_ttf.dll

已随项目放在 `FourYears/` 下。

---

# 编码规范

源代码和剧情文本统一 UTF-8。

Windows 编译必须加：

    -finput-charset=UTF-8
    -fexec-charset=UTF-8

C++ 标准：C++17。

---

# VS Code

编译任务：`.vscode/tasks.json`

C/C++ 配置：`.vscode/c_cpp_properties.json`

使用 `Ctrl + Shift + B` 调用编译任务。

---

# 输入法

Windows 中文输入法激活时，字母键会被 IME 拦截，SDL 收不到 `SDLK_n` / `SDLK_c` / `SDLK_r` 的 KEYDOWN，导致游戏内字母快捷键全部失效。

`Game::Init()` 中做了两层处理：

1. **SDL 层**：`SDL_SetHint(SDL_HINT_IME_SHOW_UI, "0")` + `SDL_StopTextInput()` + `SDL_EventState(SDL_TEXTINPUT/SDL_TEXTEDITING, SDL_DISABLE)`
2. **Win32 层**：通过 `SDL_GetWindowWMInfo` 拿到底层 `HWND`，动态加载 `imm32.dll`，调用 `ImmAssociateContext(hwnd, NULL)` 切断窗口与 IME 的关联

**代价**：重命名输入框只支持英文和数字，无法输入中文。

如果以后要支持中文输入，需要：

    ImmAssociateContext(hwnd, oldHimc);   // 恢复 IME
    SDL_StartTextInput();
    SDL_EventState(SDL_TEXTINPUT, SDL_ENABLE);

并处理 `SDL_TEXTINPUT` 事件。输入框关闭时再切回禁用状态。

因此做中文输入框时，需要把 `HWND` 和旧的 `HIMC` 保存为 `Game` 的成员变量。

---

# 字体与中文显示

项目使用 `resource/font/simhei.ttf`。

另外有 `title.ttf`（标题）和 `number.ttf`（数字）。

字体缺失字形时会渲染成方框。已知位置和当前处理：

| 位置 | 字符 | 处理 |
|---|---|---|
| `TextSystem::SetText` | `…` | 替换成 `.` |
| `DialogueUI::SplitTextLine` | `\n` | 识别为强制换行，不传给 SDL_ttf |
| `HistoryMenu::Render` | `…` `\n` | 显示前替换 |

如果脚本里用到其它字体缺失字符（如 `——`、`「」`、`·`），需要在对应位置继续加替换。

**不要仅仅因为出现方框就立即换字体。**

---

# 已知问题 / 待完善

## 存档

- **新建名字固定**：每次都是「新的存档」，无法在新建时自定义。
- **重命名不支持中文**：因为禁用了 IME。
- **章节名靠硬编码**：新增章节时需要在 `ChapterNameFromScript()` 里加映射。

## 配置

`Config.cpp` 读取的键名与初始 `config.ini` 的键名不完全一致，首次启动部分配置读不到。`[resource]` 段在 `Save` 后会丢失。

## 代码清理

- `ui/DialogueUI.cpp` 里有两份 `SplitTextLine`：自由函数 + 成员函数。自由函数是死代码。
- `GameState.h` 里的 `enum class GameState` 是死代码，真正的状态机在 `ui/UIManager.h` 的 `UIState`。
- `core/ScriptPlayer.cpp / .h` 未被使用。

## 模糊效果

`Renderer.h` 里 `BLUR_DIVISOR = 4`。像素感偏重时可以：

- 增大 `BLUR_DIVISOR`（更糊）
- 改成多遍降采样
- 对静态背景预先出模糊图

---

# 开发计划

1. **中文输入框**（恢复 IME + SDL_TEXTINPUT）
2. **新建存档自定义名**
3. **BGM / SE 接入实际资源**
4. **设置页面配置修复**（键名对齐）
5. **剧情状态恢复**（读档后 `History` 一起恢复）
6. **中文标点替换表**（`——` `「」` `·`）
7. **视觉效果**（转场、淡入淡出、立绘动画）
8. **存档缩略图**

---

# 给接手者的说明

- 先理解现有架构再改，不要因为一个 Bug 就整体重写。
- 优先小范围修改 + 编译验证，不要同时动多个无关系统。
- `.h` 声明与 `.cpp` 实现必须一致，注意不要留下重复定义。
- 关键链路：`SaveSystem ↔ SaveMenu ↔ Game`。
- 状态机在 `ui/UIManager.h` 的 `UIState`，不在 `GameState.h`（后者是死代码）。
- 所有键鼠输入最终汇聚到 `Game::OnActivateCurrentState()` / `Game::OnAdvanceDialogue()` / `Game::OnBack()` / `Game::ConfirmChoice()` 四个函数，避免逻辑分叉。
- 菜单坐标统一放在各菜单 `.h` 的 `static constexpr` 或 `ui/MenuCommon.h`，`Render` 和鼠标命中检测共用同一份常量，避免「画在这、点在那」。
- 剧本里 `[立绘]` 必须在对应角色台词**之前**，否则立绘会滞后一句。