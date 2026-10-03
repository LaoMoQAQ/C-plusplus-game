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
│   │   ├── font/         字体
│   │   ├── bgm/          背景音乐
│   │   └── se/           音效
│   │
│   ├── script/           剧本（UTF-8）
│   │   ├── chapter01.txt ~ chapter05.txt
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

- `Ctrl + Shift + B` —— 编译
- `Ctrl + Shift + P` → `Tasks: Run Task` → `Run FourYears` / `Clean FourYears`

`.vscode/settings.json` 已配置集成终端默认使用 **MSYS2 UCRT64**。

## 注意事项

- **改了 `.h` 文件**：Makefile 目前没有头文件依赖追踪，需要手动 `make clean && make`。
- **行尾必须是 LF**：`.gitattributes` 已强制。如果 Makefile 里出现 CRLF，会导致编译失败且 g++ 不报错。

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
| 选择分支 | ↑ / ↓ 或鼠标悬停，Enter 或鼠标点击确认 |

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

**新建 / 重命名**进入输入模式：
- 支持中文输入（输入法预编辑拼音显示在输入框下方）
- 字母、数字、空格追加
- 退格按 UTF-8 字符边界删除
- Enter 确认，ESC 取消
- 限长 30 UTF-8 字节

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
| BGM 音量 | 0 ~ 100，步长 5 | 立即设置音乐音量 |
| SE 音量 | 0 ~ 100，步长 5 | 立即设置音效音量 |
| 文字速度 | 5 ~ 60 字/秒，步长 5 | 打字机速度立即变化 |
| 全屏 | 开 / 关 | 立即切换全屏模式 |
| 自动播放 | 开 / 关 | 文字显示完后等 1.5 秒自动推进 |

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

判定规则（`RouteManager::CheckRoute()`，阈值 20）：

- 李君浩 ≥ 20 且高于张瀚宇 → 李君浩线
- 张瀚宇 ≥ 20 且高于李君浩 → 张瀚宇线
- 都 < 20 或相等 → 单人结局

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

`Game.cpp` 顶部 `ChapterNameFromScript()` 根据脚本路径推算。**新增章节时在这里加一条映射。**

## 兼容性

**格式已变更多次，旧存档不兼容。** 升级后请删除 `save/` 下所有 `.dat` 文件。

---

# 当前工程状态

## 已实现

### 引擎层
- SDL2 初始化、窗口、渲染器
- 资源管理（纹理缓存、按需加载）
- 字体管理（多字号缓存）
- 配置读取与保存，修改即时生效
- 音频管理（BGM / SE 缓存，音量接入 SDL_mixer）
- 跨平台 Makefile（Windows / Linux 同一套命令）
- Windows 输入法智能禁用 / 恢复

### 剧情层
- 剧本解析：`[背景]` `[立绘 左/中/右]` `[BGM]` `[SE]` `[选择]` `[标签]` `[跳转]` `[下一章]` `[结局分支]` `[角色名]`
- 单行 / 两行标签参数写法
- 章节切换（`[下一章]` 自动加载）
- 分支剧情（标签 + 跳转）
- 选择系统（键盘 + 鼠标）
- 好感度系统（累积、结局判定、存进存档、独立页面）
- 中文标点替换（`——` `「」` `『』` `·`）

### 表现层
- 打字机效果（标点停顿、`\n` 强制换行、每字一个音效）
- **多立绘同屏**（左 / 中 / 右三个位置独立管理）
- **立绘淡入淡出**（换表情或换角色时渐变）
- 对话框：圆角 + 背景模糊 + 半透明灰底 + 高度平滑动画
- 菜单：全屏遮罩 + 蓝色圆角高亮块 + 平滑滑动
- 状态切换黑幕过渡
- 全屏自动缩放（`SDL_RenderSetLogicalSize`）
- 自动播放

### 存档
- 新建 / 复制 / 删除（含二次确认） / 读取 / 重命名
- **新建和重命名支持中文输入**（拼音预编辑提示）
- 列表显示 `显示名 [章节名] 时间`，按保存时间倒序
- 自动跳过 0 字节空文件

### 内容
- 五章主线剧情 + 三种结局

---

# 注意事项

## 工作目录

从 `FourYears/` 目录运行，`resource/` `script/` `save/` `config.ini` 都是相对路径。

## Windows DLL

运行时需要：

    SDL2.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_ttf.dll

以及 `ntldd` 输出的 MinGW 运行时和第三方依赖（见「发布打包」）。

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

- **平时**：`ImmAssociateContext(hwnd, NULL)` 切断 IME + `SDL_StopTextInput()`
- **输入框激活**：`ImmAssociateContext(hwnd, oldHimc)` 恢复 IME + `SDL_StartTextInput()`

配合 `SDL_TEXTINPUT` 接收已上屏文本、`SDL_TEXTEDITING` 接收拼音预编辑文本。

**候选词窗**：SDL2 在游戏窗口里通常不显示 IME 候选词窗。当前方案用**输入框下方的拼音提示**代替：

```
新建存档: _
拼音: xinjiancundang
```

玩家能看到拼音，选字靠输入法自身的准确度。

---

# 字体与中文显示

使用 `resource/font/simhei.ttf`。另外有 `title.ttf`（标题）和 `number.ttf`（数字）。

字体缺失字形时会渲染成方框。当前处理：

| 位置 | 字符 | 处理 |
|---|---|---|
| `TextSystem::SetText` | `…` `—` `「」` `『』` `·` | 替换成 ASCII |
| `DialogueUI::SplitTextLine` | `\n` | 识别为强制换行 |

**不要仅仅因为出现方框就立即换字体。**

---

# 已知问题 / 待完善

## 内容

- 立绘表情少（每个角色基本只有 normal，吴鸿涛多两张）
- 音效资源少（只有打字音效）

## 构建

- **Makefile 没有头文件依赖追踪**：改 `.h` 后需要手动 `make clean && make`。
- **VS Code 集成终端需要重启才生效**：改 `settings.json` 后要 `Reload Window`。

## 配置

- 全屏时逻辑分辨率 1600×900 → 物理 1920×1080 是 1.2 倍非整数缩放，文字会**轻微模糊**。

## 代码

- `story/History.cpp / .h` 保留但 UI 层不再使用（为将来"剧情回退"预留）。

---

# 发布打包

## 1. 编译发布版

    cd /f/gal
    make clean
    make

（`Makefile` 里已含 `-O2 -s`）

## 2. 收集文件

    rm -rf /f/gal/release
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

1. **立绘表情扩充**（happy / sad / angry 等）——纯美术活
2. **BGM 淡入淡出**（切换时渐变）
3. **背景 / 立绘转场效果**
4. **存档缩略图**
5. **CG 鉴赏 / 音乐鉴赏**
6. **多存档槽（固定 10 格）**
7. **移动端 / Linux 打包**

---

# 给接手者的说明

- 先理解现有架构再改，不要因为一个 Bug 就整体重写。
- 优先小范围修改 + 编译验证，不要同时动多个无关系统。
- `.h` 声明与 `.cpp` 实现必须一致。
- 关键链路：`SaveSystem ↔ SaveMenu ↔ Game`。
- 状态机在 `ui/UIManager.h` 的 `UIState`。
- 所有键鼠输入汇聚到 `Game::OnActivateCurrentState()` / `OnAdvanceDialogue()` / `OnBack()` / `ConfirmChoice()`。
- 剧本里 `[立绘 ...]` 必须在对应角色台词**之前**。
- 存档格式变化后**必须删旧存档**。
- **Makefile 行尾必须是 LF**（CRLF 会导致编译失败且无错误提示）。
- **改了 `.h` 要手动 clean**。