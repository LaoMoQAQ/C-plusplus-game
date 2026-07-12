# 环境部署与编译

## 开发环境

| 项目 | 版本 |
|------|------|
| 开发语言 | C++17 |
| 图形库 | SDL2 |
| 图片库 | SDL2_image |
| 字体库 | SDL2_ttf |
| 编译器 | GCC (g++) |
| IDE | Visual Studio Code |

---

# Windows 平台

## 环境部署

### 1. 安装 MSYS2

下载并安装：

> https://www.msys2.org/

安装完成后打开 **MSYS2 UCRT64**。

---

### 2. 更新软件源

```bash
pacman -Syu
```

关闭终端，重新打开 **MSYS2 UCRT64**。

---

### 3. 安装 GCC 与 SDL2

```bash
pacman -S \
mingw-w64-ucrt-x86_64-gcc \
mingw-w64-ucrt-x86_64-SDL2 \
mingw-w64-ucrt-x86_64-SDL2_image \
mingw-w64-ucrt-x86_64-SDL2_ttf
```

---

## 编译

进入项目目录：

```powershell
cd F:\gal\FourYears
```

执行编译：

```powershell
C:\msys64\ucrt64\bin\g++.exe main.cpp StoryParser.cpp ^
-o game.exe ^
-IC:/msys64/ucrt64/include/SDL2 ^
-LC:/msys64/ucrt64/lib ^
-lmingw32 ^
-lSDL2main ^
-lSDL2 ^
-lSDL2_image ^
-lSDL2_ttf
```

---

## 运行

PowerShell：

```powershell
.\game.exe
```

CMD：

```cmd
game.exe
```

---

# Linux 平台（Ubuntu）

## 环境部署

更新软件源：

```bash
sudo apt update
```

安装 GCC：

```bash
sudo apt install g++
```

安装 SDL2：

```bash
sudo apt install libsdl2-dev
```

安装 SDL2_image：

```bash
sudo apt install libsdl2-image-dev
```

安装 SDL2_ttf：

```bash
sudo apt install libsdl2-ttf-dev
```

也可以一次安装：

```bash
sudo apt install \
g++ \
libsdl2-dev \
libsdl2-image-dev \
libsdl2-ttf-dev
```

---

## 编译

进入项目目录：

```bash
cd FourYears
```

执行编译：

```bash
g++ main.cpp StoryParser.cpp \
-o game \
-lSDL2 \
-lSDL2_image \
-lSDL2_ttf
```

---

## 运行

```bash
./game
```

---

# 项目目录

```text
FourYears/
│
├── main.cpp
├── StoryParser.cpp
├── StoryParser.h
├── Story.h
│
├── script/
│   ├── chapter01.txt
│   ├── chapter02.txt
│   ├── ending.txt
│   ├── li_junhao_route.txt
│   ├── zhang_hanyu_route.txt
│   └── alone_route.txt
│
├── resource/
│   ├── bg/
│   ├── character/
│   └── font/
│
└── game.exe（Windows）
```

---

# 依赖

- GCC ≥ 10
- SDL2
- SDL2_image
- SDL2_ttf

项目采用 C++17 标准开发，可在 Windows（MSYS2 UCRT64）和 Linux（Ubuntu）环境下完成编译与运行。
