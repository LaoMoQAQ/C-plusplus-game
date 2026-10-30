# ==========================================================
# Four Years - Makefile
# ==========================================================
# 用法：
#   make          编译（增量）
#   make run      编译并运行
#   make clean    清理
#   make rebuild  清理后重编
#   make release  打包免安装 zip
#
# 依赖追踪：
#   每个 .cpp 单独编译成 build/obj/**.o，同时用 -MMD -MP 生成 .d。
#   修改 .h 后直接 make 会重新编译依赖它的 .cpp，不需要 make clean。
#
# Shell：
#   Windows 下强制用 MSYS2 的 bash，避免依赖 PATH 里是否有 rm / mkdir。
# ==========================================================

VERSION := 0.5.0

ifeq ($(OS),Windows_NT)

    PLATFORM := windows
    SHELL    := C:/msys64/usr/bin/bash.exe

    # 从 PowerShell / cmd 调用 make 时，继承的 PATH 里没有 MSYS2 的 /usr/bin，
    # 会导致 rm / mkdir / cp 全部找不到。这里显式补上。
    export PATH := /usr/bin:/usr/local/bin:$(PATH)

    CXX      := C:/msys64/ucrt64/bin/g++.exe
    WINDRES  := C:/msys64/ucrt64/bin/windres.exe
    NTLDD    := C:/msys64/ucrt64/bin/ntldd.exe

    SDL_CFLAGS := -IC:/msys64/ucrt64/include/SDL2
    SDL_LIBS   := -LC:/msys64/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

    OUT      := FourYears/game.exe
    ZIP_TOOL := C:/Windows/System32/tar.exe

else

    PLATFORM := unix

    CXX      := g++
    WINDRES  :=
    NTLDD    :=

    SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer)
    SDL_LIBS   := $(shell pkg-config --libs   sdl2 SDL2_image SDL2_ttf SDL2_mixer)

    OUT      := game
    ZIP_TOOL := zip

endif

CXXFLAGS := -std=c++17 -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8
DEPFLAGS := -MMD -MP

# -------- 头文件搜索路径（单行，不要续行） --------
INC := -IFourYears -IFourYears/core -IFourYears/story -IFourYears/ui

# -------- 源文件清单（单行，不要续行） --------
SRC := FourYears/main.cpp FourYears/core/Game.cpp FourYears/core/Renderer.cpp FourYears/core/ResourceManager.cpp FourYears/core/TextSystem.cpp FourYears/core/SaveSystem.cpp FourYears/core/AudioManager.cpp FourYears/core/FontManager.cpp FourYears/core/Config.cpp FourYears/story/Character.cpp FourYears/story/History.cpp FourYears/story/RouteManager.cpp FourYears/story/Story.cpp FourYears/story/StoryParser.cpp FourYears/ui/UIManager.cpp FourYears/ui/DialogueUI.cpp FourYears/ui/StartMenu.cpp FourYears/ui/PauseMenu.cpp FourYears/ui/SaveMenu.cpp FourYears/ui/AffectionMenu.cpp FourYears/ui/ConfigMenu.cpp

# -------- 目标文件 --------
OBJ_DIR := build/obj
OBJ := $(patsubst FourYears/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

# -------- 图标资源（可选） --------
# 只有 icon.rc 和 icon.ico 同时存在时才编译。
ICON_RC  := $(wildcard FourYears/resource/icon.rc)
ICON_ICO := $(wildcard FourYears/resource/icon.ico)

RES_OBJ :=
ifneq ($(ICON_RC),)
    ifneq ($(ICON_ICO),)
        RES_OBJ := $(OBJ_DIR)/resource/icon.o
    endif
endif

# -------- 目标 --------
.PHONY: all run clean rebuild release help

all: $(OUT)

# 链接
$(OUT): $(OBJ) $(RES_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(OBJ) $(RES_OBJ) -o $(OUT) $(SDL_CFLAGS) $(SDL_LIBS)
	@echo "构建完成: $(OUT)"

# 编译：每个 .cpp -> .o（同时产出 .d）
$(OBJ_DIR)/%.o: FourYears/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INC) $(SDL_CFLAGS) -c $< -o $@

# 图标资源
$(OBJ_DIR)/resource/icon.o: $(ICON_RC) $(ICON_ICO)
	@mkdir -p $(dir $@)
	$(WINDRES) -I FourYears/resource $(ICON_RC) -O coff -o $@

run: $(OUT)
	cd FourYears && ./game.exe

clean:
	rm -rf build
	rm -f $(OUT)

rebuild: clean all

# ==========================================================
# 打包发布
# ==========================================================
#
# Windows 下必须用系统自带的 bsdtar（C:/Windows/System32/tar.exe），
# 它支持按扩展名自动选择 zip 格式。
# MSYS2 里的 GNU tar 不支持 zip，用它会产出一个改了后缀的 tar 包。
#
# 用法：make release
# 产物：release/FourYears-v$(VERSION).zip

RELEASE_DIR := release/FourYears
RELEASE_ZIP := release/FourYears-v$(VERSION).zip

release: $(OUT)
	@echo "==> 重建 release 目录"
	rm -rf release
	mkdir -p $(RELEASE_DIR)/save
	cp $(OUT) $(RELEASE_DIR)/
	cp FourYears/config.ini $(RELEASE_DIR)/
	cp -r FourYears/resource $(RELEASE_DIR)/
	cp -r FourYears/script $(RELEASE_DIR)/
	touch $(RELEASE_DIR)/save/.gitkeep
	@echo "==> 收集运行时 DLL"
	cd $(RELEASE_DIR) && $(NTLDD) -R game.exe | grep "msys64" | awk '{print $$3}' | while read -r dll; do cp "$$dll" .; done
	@echo "==> 打包 zip"
	$(ZIP_TOOL) -a -c -f $(RELEASE_ZIP) -C release FourYears
	@echo "完成: $(RELEASE_ZIP)"

help:
	@echo "make          编译（增量）"
	@echo "make run      编译并运行"
	@echo "make clean    清理"
	@echo "make rebuild  清理后重编"
	@echo "make release  打包 release/FourYears-v$(VERSION).zip"

# 头文件依赖（放在最后，首次构建时文件还不存在，-include 会静默忽略）
-include $(DEP)
