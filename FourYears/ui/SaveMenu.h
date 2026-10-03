#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include "../core/Renderer.h"
#include "../core/SaveSystem.h"

#include "../SavePage.h"

#include "MenuCommon.h"

#include <SDL.h>

#include <vector>
#include <string>
#include <map>


class SaveMenu
{

public:

    SaveMenu();

    void Render(Renderer& renderer);
    void HandleInput(int key);

    void SetPage(SavePage page);
    void Refresh(SaveSystem& saveSystem);

    int GetChoice() const;
    void Reset();

    bool Confirm(
        SaveSystem& saveSystem,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& outIndex,
        std::map<std::string, int>& outAffection
    );

    void Create(
        SaveSystem& saveSystem,
        const std::string& name,
        const std::string& scriptFile,
        const std::string& chapterName,
        int index,
        const std::map<std::string, int>& affection
    );

    void Delete(SaveSystem& saveSystem);
    void Copy(SaveSystem& saveSystem);

    SaveData GetCurrentSave();
    SavePage GetPage() const;

    void HandleMouseMove(int x, int y);
    MenuMouseResult HandleMouseClick(int x, int y);

    void BeginDelete();
    void CancelDelete();
    void ConfirmDelete(SaveSystem& saveSystem);
    bool IsConfirmingDelete() const;



    // ==========================================================
    // 输入模式（重命名 / 新建命名）
    // ==========================================================
    //
    // 两种模式共用同一套输入机制：
    //   - 字符走 SDL_TEXTINPUT / AppendRenameText
    //   - Enter / ESC / Backspace 走 HandleRenameKey
    //   - IME 预编辑走 SetEditingText
    //
    // IsRenaming() 在任一模式下返回 true，
    // Game 用它来切换 IME 和转发输入。

    // 进入重命名模式（改已有存档的显示名）
    void BeginRename();

    // 进入新建模式（给新存档起名字）
    // 四个参数是创建时需要的上下文，由 Game 提供
    void BeginCreate(
        const std::string& scriptFile,
        const std::string& chapterName,
        int index,
        const std::map<std::string, int>& affection
    );

    void HandleRenameKey(SDL_Keycode sym, SaveSystem& saveSystem);
    void AppendRenameText(const std::string& utf8);
    void CancelRename();
    void ConfirmRename(SaveSystem& saveSystem);

    // 是否正在输入（重命名 或 新建）
    bool IsRenaming() const;

    void SetEditingText(const std::string& utf8);

    // 每帧推进高亮块滑动
    void Update();



private:

    SavePage page;
    int choice;

    std::vector<std::string> saves;
    std::vector<SaveData> saveDataList;

    bool confirmingDelete = false;


    // ---- 输入模式 ----

    bool renaming = false;   // 重命名已有存档
    bool creating = false;   // 新建存档命名

    std::string renameBuffer;        // 输入框内容
    std::string renameTargetFile;    // 重命名时的目标文件
    std::string editingText;         // IME 预编辑（拼音）


    // ---- 新建模式下的上下文 ----

    std::string pendingScriptFile;
    std::string pendingChapterName;
    int pendingIndex = 0;
    std::map<std::string, int> pendingAffection;


    static constexpr int LIST_X   = 500;
    static constexpr int LIST_Y   = 200;
    static constexpr int LIST_GAP = 50;
    static constexpr int LIST_W   = 900;
    static constexpr int LIST_H   = 45;


    // 高亮块显示位置
    float displayHighlightY = 200.0f;

    static constexpr float HL_ANIM_SPEED      = 0.25f;
    static constexpr float HL_ANIM_SPEED_FAST = 0.55f;

    float hlSpeed = HL_ANIM_SPEED;

};


#endif