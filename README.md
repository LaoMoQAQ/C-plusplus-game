# 环境部署与编译

## 开发环境

| 项目 | 版本 |
|------|------|
| 开发语言 | C++17 |
| 编译器 | GCC (g++) |
| 图形库 | SDL2 |
| 图片库 | SDL2_image |
| 字体库 | SDL2_ttf |
| IDE | Visual Studio Code |

---

# Windows（MSYS2 UCRT64）

## 安装 MSYS2

下载并安装：

https://www.msys2.org/

安装完成后打开 **MSYS2 UCRT64**。

---

## 更新软件源

```bash
pacman -Syu
```

关闭终端后重新打开 **MSYS2 UCRT64**。

---

## 安装依赖

```bash
pacman -S \
mingw-w64-ucrt-x86_64-gcc \
mingw-w64-ucrt-x86_64-SDL2 \
mingw-w64-ucrt-x86_64-SDL2_image \
mingw-w64-ucrt-x86_64-SDL2_ttf
```

---

## 编译

```powershell
cd F:\gal\FourYears

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

# Ubuntu / Debian

## 安装依赖

```bash
sudo apt update

sudo apt install \
g++ \
libsdl2-dev \
libsdl2-image-dev \
libsdl2-ttf-dev
```

---

## 编译

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

# Arch Linux

## 安装依赖

```bash
sudo pacman -Syu

sudo pacman -S \
gcc \
sdl2 \
sdl2_image \
sdl2_ttf
```

---

## 编译

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

# Fedora

## 安装依赖

```bash
sudo dnf install \
gcc-c++ \
SDL2-devel \
SDL2_image-devel \
SDL2_ttf-devel
```

---

## 编译

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
├── game.exe      # Windows 编译生成
└── game          # Linux 编译生成
```

## 说明

本项目采用 **C++17** 开发，基于 **SDL2、SDL2_image、SDL2_ttf** 实现图形界面、图片加载和字体渲染。

支持的平台包括：

- Windows（MSYS2 UCRT64）
- Ubuntu / Debian
- Arch Linux
- Fedora

不同平台仅依赖安装方式不同，编译命令保持一致，仅生成的可执行文件名称有所区别（Windows 为 `game.exe`，Linux 为 `game`）。
