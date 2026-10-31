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

## 增量编译与头文件依赖

每个 `.cpp` 单独编译到 `build/obj/**.o`，并自动生成 `.d` 依赖文件。

**修改任何 `.h` 之后直接 `make` 即可**，不需要再 `make clean`。

## 如果 `make` 命令找不到

Windows 下 `make` 默认不在 PATH 里，需要显式调用：

    C:/msys64/usr/bin/make.exe

或者在 MSYS2 UCRT64 终端里执行（该终端已把 `/usr/bin` 加进 PATH）。
`Makefile` 内部已经强制使用 MSYS2 的 bash 并补全了 `/usr/bin`，
所以从 PowerShell / cmd / VS Code 任务调用都不会缺 `rm`、`mkdir`。

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

固定 10 格槽位，2 列 x 5 行。

| 操作 | 键鼠 |
|---|---|
| 切换槽位 | ↑ ↓（本列）/ ← →（换列），或鼠标悬停 |
| 读档 | Enter 或点击槽位 |
| 新建 | N |
| 复制 | C（复制到下一个槽位） |
| 删除 | Delete（两段式确认） |
| 重命名 | R |

`N` / `R` 只在**存档管理**页可用（从暂停菜单进入）。
主菜单的"存档"是只读的读档页。

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
| `[BGM]` | `[BGM] xxx.mp3` | 切换背景音乐（`stop` 停止），带淡入淡出 |
| `[SE]` | `[SE] xxx.wav` | 一次性音效 |
| `[角色名]` | `[李君浩]` | 开始一段对话 |
| `<n>` | `<1.5>` | 等待 n 秒 |
| `[选择]` | 见下 | 进入选择事件 |
| `[标签]` | `[标签] name` | 打书签 |
| `[跳转]` | `[跳转] #name` | 跳到书签 |
| `[下一章]` | `[下一章] script/xxx.txt` | 播完自动加载下一章 |
| `[结局分支]` | 见下 | 根据好感度自动选结局 |
| `[结局]` | `[结局]` | 结局标题，按旁白念出 |
| `[END]` | `[END]` | 剧情结束，自动回到标题画面 |

带参数的标签**单行和两行写法都支持**。

## 立绘约定

**`[立绘 ...]` 必须紧跟在对应角色的 `[角色名]` 之前**，中间不能夹别的 `[角色名]`。

立绘有**三个位置**：
- 左（屏幕 25% 处）
- 中（屏幕 50% 处）
- 右（屏幕 75% 处，默认）

换立绘时**自动淡入淡出**。

## 立绘资源现状

| 文件 | 尺寸 | 说明 |
|---|---|---|
| `wuhongtao/normal.png` | 363x665 | 主角·平常 |
| `wuhongtao/happy.png` | 363x665 | 主角·开心（**占位，与 normal 完全相同**） |
| `wuhongtao/sad.png` | 500x1002 | 主角·难过 |
| `li_junhao/normal.png` | 381x654 | 李君浩·平常（缺其他表情） |
| `zhang_hanyu/normal.png` | 362x670 | 张瀚宇·平常（缺其他表情） |

`happy.png` 虽然是占位图，但剧本里有多处引用，**不要删**——
删了会导致该位置立绘加载失败、不显示。补美术时直接覆盖即可。

## 用 AI 生成立绘

工具链在 `E:\fouryears workspace\imagegen\`，完整文档见该目录的 `README.md`。

**现状：能跑通，但产出达不到本项目要求。**

已稳定做到：
- 全身站姿（ControlNet OpenPose 硬约束）
- 角色一致性（同 seed + 只改表情词）
- 透明背景（泛洪填充抠图）

做不到：
- 去不掉模型强加的装饰（飘带、墨迹、月牙）
- 服装颜色不受控（`red t-shirt` 常变黑）
- 分辨率上限 512x768

根因是**显存**：显卡标称 8 GB，被桌面占用后实测只剩 2 GB。
业界做法（LoRA 微调 / SDXL）都需要 6~8 GB 以上。

**游戏代码不关心立绘怎么来的**，只要满足：
RGBA PNG、透明背景、高度约 700px、
放在 `FourYears/resource/character/<角色>/<表情>.png`。

---

## BGM 淡入淡出

`AudioManager` 换曲走"先淡出旧曲、再换曲淡入"，各 500 ms。

SDL_mixer 只有一条音乐通道，做不了真正的交叉淡化，
所以是顺序过渡而不是同时叠加。

`Game::Update()` 每帧调 `audioManager.Update(delta)` 推进切换。

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

## 剧情结束

结局文件末尾写 `[END]`。播完最后一句后自动回到标题画面。

没有 `[END]` 且没有 `[下一章]` 时，播到最后一句也会回标题。

---

# 存档

目录：`FourYears/save/`

## 固定 10 格槽位

| 文件 | 内容 |
|---|---|
| `slot1.dat` ~ `slot10.dat` | 存档正文（7 行纯文本） |
| `slot1.png` ~ `slot10.png` | 缩略图（可选） |

空槽位没有对应的 `.dat` 文件。

界面是 2 列 x 5 行。键盘 ↑↓ 在本列内移动，←→ 切换列；鼠标悬停选中。

## 文件格式（7 行）

```
显示名
脚本路径
章节显示名
index
时间
好感度（李君浩:10,张瀚宇:15）
好感度历史（李君浩|10|好，麻烦你了,张瀚宇|15|主动加个微信）
```

第 7 行（历史）是后加的，格式 `角色|变化量|来源`，记录之间用 `,` 分隔。
来源文本里的 `,` 和 `|` 会在写入时替换成空格，避免破坏格式。

**旧存档只有 6 行，读的时候 getline 失败就当作空历史，不影响读档。**

缩略图**不写进这 7 行**，单独存成同名的 `.png`。
这样旧存档（没有图）依然能读，只是列表里显示占位框。

## 缩略图

按 `N` 新建存档时，会把当前剧情画面截下来缩成 320x180，输完名字后
和 `.dat` 一起落盘。

实现上 `Game::Render()` 在 `Present()` 之前调 `SDL_RenderReadPixels`
把整帧缓存到 `framePixels`，`CaptureThumbnail()` 只做缩放。
**不要在按键处理里直接读像素**——那时当前帧还没画出来。

## 旧存档迁移

旧版本用 `save_<毫秒时间戳>.dat` 无限列表。新版启动时会把这些文件
按写入时间倒序填进空槽位，然后改名为 `slotN.dat`。

槽位填满后剩下的旧文件保持原样，不会删除。

## 章节显示名

`Game.cpp` 顶部 `ChapterNameFromScript()` **用数字解析**：
- `chapter01.txt` → `第1章`
- `chapter10.txt` → `第10章`
- 结局文件走特判

**新增章节不需要改代码。**

---

# 注意事项

## 行尾

项目用 `.gitattributes` 统一成 LF，提交时 git 会自动转换。

注意：**当前工具链（GCC 16 / GNU Make 4.4）下，CRLF 并不会导致编译失败**，
这条经验来自旧工具链，已经无法复现。保留 LF 约定是为了 diff 干净，
不用再当成"踩坑"来防。

## 帧率与时间

主循环用 `SDL_GetPerformanceCounter()` 算真实 delta，所有动画和
打字速度都按 delta 推进，帧率无关。

垂直同步优先；检测到 Present 不阻塞（窗口被遮挡、远程桌面等情况）
时自动兜底限到 60 FPS。实测稳定 59~60 FPS。

---
## 工作目录

**必须从 `FourYears/` 目录运行**，所有资源路径都是相对这个目录的。

从仓库根目录直接跑 `FourYears/game.exe` 不会崩，但会：
字体加载失败、剧本打不开、背景全黑——一个纯黑窗口。
`.vscode/tasks.json` 里的 "Run FourYears" 任务已经设置了正确的 `cwd`。

## Windows DLL

运行时需要：

    SDL2.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_ttf.dll

以及 `ntldd` 输出的 MinGW 运行时和第三方依赖。

---

# 打包发布

## 一键打包

    make release

产物：`release/FourYears-v<版本号>.zip`（版本号在 `Makefile` 顶部的 `VERSION`）。

这个目标会：清理 `release/` → 复制 exe / config.ini / resource / script →
用 `ntldd` 自动收集全部依赖 DLL → 打包成 zip。

## ⚠️ 必须用系统自带的 tar

Windows 下打包用的是 **`C:/Windows/System32/tar.exe`**（bsdtar），
它支持按扩展名自动选择 zip 格式。

**不要用 MSYS2 里的 GNU tar**（`/usr/bin/tar`）。GNU tar 的 `-a` 只支持
`.tar.gz` / `.tar.bz2` 之类，**不支持 zip**，执行
`tar -a -c -f xxx.zip dir` 会产出一个**改了后缀的 tar 包**。
玩家下载后双击解压会报"文件已损坏"。

验证方法：

    python -c "import zipfile; zipfile.ZipFile('release/FourYears-v0.5.0.zip').testzip()"

没抛异常才是真 zip。

## 上传 Release

1. 打开 GitHub 仓库 → 右侧 Releases → Draft a new release
2. Tag 填新版本号（如 `v0.5.0`）
3. Title 写发布标题
4. Description 写更新日志（Markdown）
5. 把 zip 拖入附件区
6. Publish release

## 发版前本地自测

解压 zip 到任意目录，从 `FourYears/` 里跑 `game.exe`，确认：
主菜单正常、能进游戏、BGM/音效正常、能存档读档、中文输入正常、
**点窗口 X 能正常退出且没有崩溃弹窗**。

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