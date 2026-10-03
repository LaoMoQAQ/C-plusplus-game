# ==========================================================
# Four Years - Makefile
# ==========================================================
# 用法：
#   make         编译
#   make run     编译并运行
#   make clean   清理
#   make rebuild 清理后重编
#
# Windows (MSYS2 UCRT64) 和 Linux 共用同一个 Makefile。
# 图标资源（.rc）只在 Windows 且文件存在时才编译。

# -------- 平台分支 --------
ifeq ($(OS),Windows_NT)

    CXX        := C:/msys64/ucrt64/bin/g++.exe
    WINDRES    := C:/msys64/ucrt64/bin/windres.exe
    SDL_CFLAGS := -IC:/msys64/ucrt64/include/SDL2
    SDL_LIBS   := -LC:/msys64/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
    OUT        := FourYears/game.exe
    RUN_CMD    := cd FourYears && ./game.exe
    CLEAN_CMD  := rm -f FourYears/game.exe FourYears/resource/icon.o

else

    CXX        := g++
    WINDRES    :=
    SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer)
    SDL_LIBS   := $(shell pkg-config --libs   sdl2 SDL2_image SDL2_ttf SDL2_mixer)
    OUT        := game
    RUN_CMD    := ./game
    CLEAN_CMD  := rm -f game

endif

CXXFLAGS := -std=c++17 -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8

# -------- 头文件搜索路径（单行，不要续行） --------
INC := -IFourYears -IFourYears/core -IFourYears/story -IFourYears/ui

# -------- 源文件清单（单行，不要续行） --------
SRC := FourYears/main.cpp FourYears/core/Game.cpp FourYears/core/Renderer.cpp FourYears/core/ResourceManager.cpp FourYears/core/TextSystem.cpp FourYears/core/SaveSystem.cpp FourYears/core/AudioManager.cpp FourYears/core/FontManager.cpp FourYears/core/Config.cpp FourYears/story/Character.cpp FourYears/story/History.cpp FourYears/story/RouteManager.cpp FourYears/story/Story.cpp FourYears/story/StoryParser.cpp FourYears/ui/UIManager.cpp FourYears/ui/DialogueUI.cpp FourYears/ui/StartMenu.cpp FourYears/ui/PauseMenu.cpp FourYears/ui/SaveMenu.cpp FourYears/ui/AffectionMenu.cpp FourYears/ui/ConfigMenu.cpp

# -------- 图标资源（可选） --------
# 只有 icon.rc 和 icon.ico 同时存在时才编译。
# 缺任何一个就自动跳过，不影响其他功能。

ICON_RC  := $(wildcard FourYears/resource/icon.rc)
ICON_ICO := $(wildcard FourYears/resource/icon.ico)

RES_OBJ :=
ifneq ($(ICON_RC),)
    ifneq ($(ICON_ICO),)
        RES_OBJ := FourYears/resource/icon.o
    endif
endif

# -------- 目标 --------
.PHONY: all run clean rebuild help

all: $(OUT)

$(OUT): $(SRC) $(RES_OBJ)
	$(CXX) $(CXXFLAGS) $(SRC) $(RES_OBJ) -o $(OUT) $(INC) $(SDL_CFLAGS) $(SDL_LIBS)

# 编译 .rc 为 .o（仅 Windows 且文件存在时）
ifneq ($(RES_OBJ),)
$(RES_OBJ): $(ICON_RC) $(ICON_ICO)
	$(WINDRES) -I FourYears/resource $(ICON_RC) -O coff -o $@
endif

run: $(OUT)
	$(RUN_CMD)

clean:
	$(CLEAN_CMD)

rebuild: clean all

help:
	@echo "make         编译"
	@echo "make run     编译并运行"
	@echo "make clean   清理"
	@echo "make rebuild 清理后重编"