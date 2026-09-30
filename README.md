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
│   ├── ScriptPlayer.cpp / .h        （保留，未使用）
│   ├── SaveSystem.cpp / .h
│   ├── AudioManager.cpp / .h
│   ├── Config.cpp / .h
│   └── Utils.cpp / .h
│
├── story/                剧情数据：解析、角色、路线、历史
│   ├── Story.cpp / .h
│   ├── StoryParser.cpp / .h
│   ├── Character.cpp / .h
│   ├── History.cpp / .h             （保留，未使用）
│   └── RouteManager.cpp / .h
│
├── ui/                   界面：各菜单页面与状态切换
│   ├── UIManager.cpp / .h
│   ├── MenuCommon.h
│   ├── StartMenu.cpp / .h
│   ├── PauseMenu.cpp / .h
│   ├── DialogueUI.cpp / .h
│   ├── SaveMenu.cpp / .h
│   ├── AffectionMenu.cpp / .h
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

    C:\msys64\ucrt64\bin\g++.exe -std=c++17 -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp core\Game.cpp core\Renderer.cpp core\ResourceManager.cpp core\TextSystem.cpp core\ScriptPlayer.cpp core\SaveSystem.cpp core\AudioManager.cpp core\FontManager.cpp core\Config.cpp core\Utils.cpp story\Character.cpp story\History.cpp story\RouteManager.cpp story\Story.cpp story\StoryParser.cpp ui\UIManager.cpp ui\DialogueUI.cpp ui\StartMenu.cpp ui\PauseMenu.cpp ui\SaveMenu.cpp ui\AffectionMenu.cpp ui\ConfigMenu.cpp -o game.exe -IC:\msys64\ucrt64\include\SDL2 -I. -Icore -Istory -Iui -LC:\msys64\ucrt64\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

也可以使用 VS Code 的编译任务：

    Ctrl + Shift + B

---

## Linux（Ubuntu / Arch / Fedora）

    g++ -std=c++17 main.cpp core/Game.cpp core/Renderer.cpp core/ResourceManager.cpp core/TextSystem.cpp core/ScriptPlayer.cpp core/SaveSystem.cpp core/AudioManager.cpp core/FontManager.cpp core/Config.cpp core/Utils.cpp story/Character.cpp story/History.cpp story/RouteManager.cpp story/Story.cpp story/StoryParser.cpp ui/UIManager.cpp ui/DialogueUI.cpp ui/StartMenu.cpp ui/PauseMenu.cpp ui/SaveMenu.cpp ui/AffectionMenu.cpp ui/ConfigMenu.cpp -o game -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

> 注：Windows 专用的输入法禁用逻辑（见「输入法」一节）通过 `#ifdef _WIN32` 包裹，Linux 编译时会自动跳过。

---

# 发布打包

## 1. 编译发布版

比日常编译多 `-O2 -s`：

    C:/msys64/ucrt64/bin/g++.exe \
      -std=c++17 -O2 -s \
      -finput-charset=UTF-8 -fexec-charset=UTF-8 \
      main.cpp \
      core/Game.cpp core/Renderer.cpp core/ResourceManager.cpp \
      core/TextSystem.cpp core/ScriptPlayer.cpp core/SaveSystem.cpp \
      core/AudioManager.cpp core/FontManager.cpp core/Config.cpp core/Utils.cpp \
      story/Character.cpp story/History.cpp story/RouteManager.cpp \
      story/Story.cpp story/StoryParser.cpp \
      ui/UIManager.cpp ui/DialogueUI.cpp ui/StartMenu.cpp \
      ui/PauseMenu.cpp ui/SaveMenu.cpp ui/AffectionMenu.cpp ui/ConfigMenu.cpp \
      -o game.exe \
      -IC:/msys64/ucrt64/include/SDL2 \
      -I. -Icore -Istory -Iui \
      -LC:/msys64/ucrt64/lib \
      -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

## 2. 收集文件

    cd /f/gal
    mkdir -p release/FourYears/save
    cp FourYears/game.exe     release/FourYears/
    cp FourYears/config.ini   release/FourYears/
    cp -r FourYears/resource  release/FourYears/
    cp -r FourYears/script    release/FourYears/
    touch release/FourYears/save/.gitkeep

复制所有依赖 DLL：

    cd release/FourYears
    ntldd -R game.exe | grep "msys64" | awk '{print $3}' | while read -r dll; do
        cp "$dll" .
    done

> `read` 必须加 `-r`，否则 bash 会吃掉路径中的反斜杠。

`ntldd` 没装：

    pacman -S mingw-w64-ucrt-x86_64-ntldd

## 3. 打包

    cd /f/gal/release
    tar -a -c -f FourYears-v0.1.0.zip FourYears

## 自检

**把 zip 解压到干净目录，双击 `game.exe` 试运行**。能进主菜单、能开游戏、能存档读档，才算通过。

## 上传

用 GitHub Release 上传，`.gitignore` 已排除 `release/`。

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
| 选择分支 | ↑ / ↓ 或鼠标悬停切换，Enter 或鼠标点击确认 |

出现选项时，Space 和鼠标左键**不会推进剧情**。

## 暂停菜单

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 确认 | Enter 或鼠标点击 |
| 返回 | ESC 或点击「返回 [ESC]」 |

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
- 字母 / 数字 / 空格 追加
- 退格删除（支持 UTF-8 边界）
- Enter 确认，ESC 取消
- **只支持英文和数字**

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
  - **ESC** 取消询问，留在设置页

各选项作用：

| 选项 | 范围 | 效果 |
|---|---|---|
| BGM 音量 | 0 ~ 100，步长 5 | 立即设置 SDL_mixer 音乐音量 |
| SE 音量 | 0 ~ 100，步长 5 | 立即设置 SDL_mixer 音效音量 |
| 文字速度 | 5 ~ 100 字/秒，步长 5 | 打字机速度立即变化 |
| 全屏 | 开 / 关 | 立即切换全屏模式 |
| 自动播放 | 开 / 关 | 文字显示完后等 1.5 秒自动推进 |

---

# 剧本格式

剧本是 UTF-8 纯文本，放在 `script/` 下。

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

> 带参数的标签**单行和两行写法都支持**：
> ```
> [标签] ch01_help
> ```
> 等价于
> ```
> [标签]
> ch01_help
> ```

## 立绘顺序约定

**`[立绘]` 必须紧跟在对应角色的 `[角色名]` 之前，中间不能夹别的 `[角色名]`。**

正确：

```
[立绘]
li_junhao/normal.png

[李君浩]
看你的样子……
```

错误（会导致立绘滞后一句）：

```
[李君浩]
看你的样子……

[立绘]
li_junhao/normal.png
```

## 选择语法

```
[选择]

1. 显示文本 | 好感度变化 -> 跳转目标

2. 显示文本
```

- **显示文本**：选项在屏幕上显示的文字
- **`|` 好感度变化**：可写多个用 `,` 分隔，如 `李君浩 +10, 张瀚宇 -5`。可省略
- **`->` 跳转目标**：可省略
  - `-> #标签名` → 跳到同文件书签
  - `-> script/xxx.txt` → 加载外部脚本
  - 省略 → 不做跳转，直接推进下一句

例子：

```
[选择]

1. 好，麻烦你了 | 李君浩 +10 -> #ch01_help

2. 谢谢，我自己找找看 -> #ch01_alone

3. 沉默（不加好感度，不跳转）
```

## 标签与跳转

同文件分支写法：

```
[选择]

1. A 选项 | 李君浩 +10 -> #branch_a

2. B 选项 -> #branch_b

[标签] branch_a

... A 分支剧情 ...

[跳转] #join

[标签] branch_b

... B 分支剧情 ...

[跳转] #join

[标签] join

... 两分支合流后的主线 ...
```

- `[标签] name` 定义一个书签，不生成对话
- `[跳转] #name` 跳到书签，不显示到对话框
- 选项里的 `-> #name` 效果与 `[跳转]` 相同

**标签名不要重名**，重名会覆盖。

## 结局分支

```
[结局分支]

李君浩 -> script/li_junhao_route.txt

张瀚宇 -> script/zhang_hanyu_route.txt

单人 -> script/alone_ending.txt
```

到达这个事件时，自动根据当前好感度判断：

- 李君浩好感度 ≥ 20 且高于张瀚宇 → 李君浩线
- 张瀚宇好感度 ≥ 20 且高于李君浩 → 张瀚宇线
- 两人都 < 20，或两人相等 → 单人结局

阈值在 `RouteManager::CheckRoute()` 里，当前是 `20`。

## 章节切换

```
[下一章]
script/chapter02.txt
```

播完当前脚本最后一句后，按 Space 自动加载下一章。**路径要相对工作目录 `FourYears/` 写全**（例如 `script/chapter02.txt`，不能只写 `chapter02.txt`）。

---

# 存档

目录：`save/`

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

例：

```
新的存档
script/chapter01.txt
第1章
7
2026-09-27 15:30:00
李君浩:10,张瀚宇:15
```

- `脚本路径` 是读档时 `Story::Load()` 用的，用于恢复玩家所处的章节
- `index` 是脚本 `events` 数组里的下标
- `好感度` 用 `名字:数值,名字:数值` 序列化

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
- 配置读取与保存，且**修改即时生效**
- 音频管理（BGM / SE 音量已接入 SDL_mixer）
- 剧本解析
  - 支持 `[背景]` `[立绘]` `[BGM]` `[SE]` `[选择]` `[标签]` `[跳转]` `[下一章]` `[结局分支]` `[角色名]`
  - 支持 `<>` 等待标记
  - 自动清理 BOM / `\r`
  - 单行 / 两行标签参数写法都支持
- 剧情播放（`Story` + `DialogueUI` + `TextSystem`）
- **章节切换**：脚本末尾 `[下一章]` 自动加载
- **分支剧情**：`[标签]` / `[跳转]` / `-> #标签`
- **选择系统**
  - 选项显示在对话框内
  - ↑ / ↓ 或鼠标悬停切换
  - Enter 或鼠标点击确认
  - 每个选项可绑定好感度变化
- **好感度系统**
  - `RouteManager` 累积
  - `[结局分支]` 根据好感度自动选线
  - 好感度存进存档
  - 好感度页面显示
- 打字机效果（标点停顿、中文省略号替换、`\n` 强制换行）
- 主菜单、暂停菜单、存档、设置、好感度
- 键鼠双输入
- 立绘固定高度 + 宽高比缩放 + 右中侧居中
- 对话框：深灰半透明矩形
- 二级菜单背景模糊（近似高斯）
- 设置页面：S 保存 + ESC 询问
- 全屏切换（`SDL_RenderSetLogicalSize` 自动缩放）
- 自动播放（1.5 秒间隔）
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

以及 `ntldd` 输出的 MinGW 运行时和第三方依赖（见「发布打包」一节）。

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

Windows 中文输入法激活时，字母键会被 IME 拦截，SDL 收不到 `SDLK_n` / `SDLK_c` / `SDLK_r` / `SDLK_s` 的 KEYDOWN，导致字母快捷键失效。

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

项目使用 `resource/font/simhei.ttf`。另外有 `title.ttf`（标题）和 `number.ttf`（数字）。

字体缺失字形时会渲染成方框。已知位置和当前处理：

| 位置 | 字符 | 处理 |
|---|---|---|
| `TextSystem::SetText` | `…` | 替换成 `.` |
| `DialogueUI::SplitTextLine` | `\n` | 识别为强制换行，不传给 SDL_ttf |
| `AffectionMenu::Render` | — | 只显示中文角色名和数字，安全 |

如果脚本里用到其它字体缺失字符（如 `——`、`「」`、`·`），需要在 `TextSystem::SetText` 的替换表里继续加分支。

**不要仅仅因为出现方框就立即换字体。**

---

# 已知问题 / 待完善

## 存档

- **新建名字固定**：每次都是「新的存档」，无法在新建时自定义。
- **重命名不支持中文**：因为禁用了 IME。
- **章节名靠硬编码**：新增章节时需要在 `ChapterNameFromScript()` 里加映射。

## 配置

- `config.ini` 的 `[resource]` 段在游戏内保存设置后会丢失（`Config::Save` 不写这段）。当前资源路径硬编码在 `Game::Init()` 里，不影响运行。
- 全屏时因为逻辑分辨率 1600×900 → 物理 1920×1080 是 1.2 倍非整数缩放，文字会**轻微模糊**。

## 代码清理

- `core/ScriptPlayer.cpp / .h` 未被使用。
- `story/History.cpp / .h` 仍被编译，但 UI 层已不再显示历史记录。
- `GameState.h` 里的 `enum class GameState` 是死代码，真正的状态机在 `ui/UIManager.h` 的 `UIState`。

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
4. **剧情状态恢复**（读档后 `History` 一起恢复）
5. **中文标点替换表**（`——` `「」` `·`）
6. **视觉效果**（转场、淡入淡出、立绘动画）
7. **存档缩略图**

---

# 给接手者的说明

- 先理解现有架构再改，不要因为一个 Bug 就整体重写。
- 优先小范围修改 + 编译验证，不要同时动多个无关系统。
- `.h` 声明与 `.cpp` 实现必须一致，注意不要留下重复定义。
- 关键链路：`SaveSystem ↔ SaveMenu ↔ Game`。
- 状态机在 `ui/UIManager.h` 的 `UIState`，不在 `GameState.h`（后者是死代码）。
- 所有键鼠输入最终汇聚到 `Game::OnActivateCurrentState()` / `Game::OnAdvanceDialogue()` / `Game::OnBack()` / `Game::ConfirmChoice()` 四个函数，避免逻辑分叉。
- 菜单坐标统一放在各菜单 `.h` 的 `static constexpr` 或 `ui/MenuCommon.h`，`Render` 和鼠标命中检测共用同一份常量。
- 剧本里 `[立绘]` 必须在对应角色台词**之前**，否则立绘会滞后一句。
- 剧本里 `[标签]` / `[跳转]` 支持单行写法，脚本简洁优先用单行。
- 存档格式变化后**必须删旧存档**，否则 `Story::Load()` 恢复章节时 index 会对不上。