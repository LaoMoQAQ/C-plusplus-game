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

### 更新软件源

```bash
pacman -Syu
```

关闭终端后重新打开 **MSYS2 UCRT64**。

### 安装依赖

```bash
pacman -S \
mingw-w64-ucrt-x86_64-gcc \
mingw-w64-ucrt-x86_64-SDL2 \
mingw-w64-ucrt-x86_64-SDL2_image \
mingw-w64-ucrt-x86_64-SDL2_ttf
```

---

# Ubuntu / Debian

安装依赖：

```bash
sudo apt update

sudo apt install \
g++ \
libsdl2-dev \
libsdl2-image-dev \
libsdl2-ttf-dev
```

---

# Arch Linux

安装依赖：

```bash
sudo pacman -Syu

sudo pacman -S \
gcc \
sdl2 \
sdl2_image \
sdl2_ttf
```

---

# Fedora

安装依赖：

```bash
sudo dnf install \
gcc-c++ \
SDL2-devel \
SDL2_image-devel \
SDL2_ttf-devel
```

---

# 编译

## Windows（MSYS2 UCRT64）

```powershell
C:\msys64\ucrt64\bin\g++.exe -std=c++17 main.cpp StoryParser.cpp -o game.exe -IC:/msys64/ucrt64/include/SDL2 -LC:/msys64/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
```

## Linux（Ubuntu / Arch / Fedora）

```bash
g++ -std=c++17 main.cpp StoryParser.cpp -o game -lSDL2 -lSDL2_image -lSDL2_ttf
```

---

# 运行

## Windows

```powershell
.\game.exe
```

## Linux

```bash
./game
```
