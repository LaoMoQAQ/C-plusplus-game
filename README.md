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
│   ├── SavePage.h
│   ├── SDL2*.dll
│   │
│   ├── core/             引擎核心
│   │   ├── Game.cpp / .h
│   │   ├── Renderer.cpp / .h
│   │   ├── ResourceManager.cpp / .h
│   │   ├── FontManager.cpp / .h
│   │   ├── TextSystem.cpp / .h
│   │   ├── SaveSystem.cpp / .h
│   │   ├── AudioManager.cpp / .h
│   │   └── Config.cpp / .h
│   │
│   ├── story/            剧情数据
│   │   ├── Story.cpp / .h
│   │   ├── StoryParser.cpp / .h
│   │   ├── Character.cpp / .h
│   │   ├── History.cpp / .h
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
│   │   ├── font/         字体
│   │   ├── bgm/          背景音乐
│   │   └── se/           音效
│   │
│   ├── script/           剧本（UTF-8）
│   │   ├── chapter01.txt ~ chapter10.txt
│   │   ├── ending.txt
│   │   ├── li_junhao_route.txt
│   │   ├── zhang_hanyu_route.txt
│   │   └── alone_ending.txt
│   │
│   └── save/             存档（运行时生成）
│
└── release/              打包目录（git 忽略）
```

---

# Windows（MSYS2 UCRT64）

## 安装 MSYS2

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

`Makefile` 会自动判断平台。输出：

- Windows：`FourYears/game.exe`
- Linux：`game`

---

# 运行

## Windows

    cd FourYears
    ./game.exe

## Linux

    ./game

---

# 操作说明

## 通用

| 操作 | 键鼠 |
|---|---|
| 上下选择 | ↑ / ↓ 或鼠标悬停 |
| 确认 | Enter 或鼠标点击 |
| 返回 | ESC 或点击「返回 [ESC]」 |

## 剧情对话

| 操作 | 键鼠 |
|---|---|
| 跳过当前文字 / 推进 | Space 或鼠标左键 |
| 打开暂停菜单 | ESC |
| 选择分支 | ↑ / ↓ 或鼠标悬停，Enter 或鼠标点击确认 |

## 存档页面

| 操作 | 键鼠 |
|---|---|
| 读档 | Enter 或点击存档项 |
| 新建 | N |
| 复制 | C |
| 删除 | Delete（两段式确认） |
| 重命名 | R |

新建 / 重命名支持中文输入，输入法拼音会显示在输入框下方。

## 好感度页面

显示好感度进度条和变化历史。↑ / ↓ 或鼠标滚轮翻页。

## 设置页面

| 操作 | 键鼠 |
|---|---|
| 调整数值 | ← / → |
| 保存 | S |

| 选项 | 范围 |
|---|---|
| BGM 音量 | 0 ~ 100 |
| SE 音量 | 0 ~ 100 |
| 文字速度 | 5 ~ 60 字/秒 |
| 全屏 | 开 / 关 |
| 自动播放 | 开 / 关 |

退出时若有未保存改动会弹窗询问：Y 保存 / N 不保存 / ESC 取消。

---

# 剧本格式

剧本是 UTF-8 纯文本，放在 `FourYears/script/` 下。

## 标签表

| 标签 | 写法 | 作用 |
|---|---|---|
| `[背景]` | `[背景] xxx.png` | 切换背景 |
| `[立绘]` | `[立绘] xxx.png` | 右位置立绘 |
| `[立绘 左]` | `[立绘 左] xxx.png` | 左位置立绘 |
| `[立绘 中]` | `[立绘 中] xxx.png` | 中位置立绘 |
| `[立绘 右]` | `[立绘 右] xxx.png` | 右位置立绘 |
| 立绘清除 | `[立绘 左] clear` | 清除该位置立绘 |
| `[BGM]` | `[BGM] xxx.mp3` | 切换背景音乐（`stop` 停止） |
| `[SE]` | `[SE] xxx.wav` | 一次性音效 |
| `[角色名]` | `[李君浩]` | 开始一段对话 |
| `<n>` | `<1.5>` | 等待 n 秒 |
| `[选择]` | 见下 | 进入选择事件 |
| `[标签]` | `[标签] name` | 打书签 |
| `[跳转]` | `[跳转] #name` | 跳到书签 |
| `[下一章]` | `[下一章] script/xxx.txt` | 播完自动加载下一章 |
| `[结局分支]` | 见下 | 根据好感度自动选结局 |

带参数的标签**单行和两行写法都支持**。

## 立绘约定

**`[立绘 ...]` 必须紧跟在对应角色的 `[角色名]` 之前**，中间不能夹别的 `[角色名]`。

立绘有**三个位置**：
- 左（屏幕 25% 处）
- 中（屏幕 50% 处）
- 右（屏幕 75% 处，默认）

换立绘时**自动淡入淡出**。

## 选择语法

```
[选择]

1. 显示文本 | 好感度变化 -> 跳转目标

2. 显示文本
```

- `|` 后是好感度变化，可多个用 `,` 分隔
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

判定规则（`RouteManager::CheckRoute()`）：

- **阈值**：70
- **好感度上限**：100

| 情况 | 结局 |
|---|---|
| 李君浩 ≥ 70 且高于张瀚宇 | 李君浩线 |
| 张瀚宇 ≥ 70 且高于李君浩 | 张瀚宇线 |
| 都 < 70 或相等 | 单人结局 |

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

`Game.cpp` 顶部 `ChapterNameFromScript()` **用数字解析**：
- `chapter01.txt` → `第1章`
- `chapter10.txt` → `第10章`
- 结局文件走特判

**新增章节不需要改代码。**

---

# 注意事项

## 工作目录

从 `FourYears/` 目录运行，所有路径都是相对路径。

## Windows DLL

运行时需要：

    SDL2.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_ttf.dll

以及 `ntldd` 输出的 MinGW 运行时和第三方依赖。

---

# 编码规范

源代码和剧情文本统一 UTF-8。

Windows 编译必须加：

    -finput-charset=UTF-8
    -fexec-charset=UTF-8

C++ 标准：C++17。Makefile 已配置。

---

# 输入法

Windows 中文输入法激活时，字母键会被 IME 拦截。`Game::SetInputMode(bool)` 动态切换：

- **平时**：切断 IME + `SDL_StopTextInput()`
- **输入框激活**：恢复 IME + `SDL_StartTextInput()`

配合 `SDL_TEXTINPUT` 接收已上屏文本、`SDL_TEXTEDITING` 接收拼音预编辑文本。

**候选词窗**：SDL2 在游戏窗口里通常不显示 IME 候选词窗。当前方案用**输入框下方的拼音提示**代替：

```
新建存档: _
拼音: xinjiancundang
```

---

# 字体与中文显示

使用 `resource/font/simhei.ttf`。

字体缺失字形时会渲染成方框。当前处理：

| 位置 | 字符 | 处理 |
|---|---|---|
| `TextSystem::SetText` | `…` `—` `「」` `『』` `·` | 替换成 ASCII |
| `DialogueUI::SplitTextLine` | `\n` | 识别为强制换行 |

---