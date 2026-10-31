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


// ==========================================================
// SaveMenu
// ==========================================================
//
// 固定 10 格存档槽位，2 列 x 5 行。
//
// 每格显示：缩略图（有就画，没有画占位框）+ 存档名 + 章节 + 时间。
// 空槽位显示"空槽位 N"。
//
// 键盘：↑↓ 切换槽位（按列走，左右键切列）
// 鼠标：悬停高亮，点击选中

class SaveMenu
{

public:

    SaveMenu();

    void Render(Renderer& renderer);
    void HandleInput(int key);

    void SetPage(SavePage page);
    void Refresh(SaveSystem& saveSystem);

    // Game 在初始化时绑定，Render 用它取缩略图纹理。
    void SetSaveSystem(SaveSystem* system);

    // 当前选中的槽位号 1~10
    int GetChoice() const;
    void Reset();

    // 读档：把当前槽位内容取出来。
    // 空槽位返回 false。
    bool Confirm(
        SaveSystem& saveSystem,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& outIndex,
        std::map<std::string, int>& outAffection,
        std::vector<AffectionRecord>& outRecords
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

    // 进入新建模式（给当前选中的槽位写存档）
    void BeginCreate(
        const std::string& scriptFile,
        const std::string& chapterName,
        int index,
        const std::map<std::string, int>& affection,
        const std::vector<AffectionRecord>& records
    );

    void HandleRenameKey(SDL_Keycode sym, SaveSystem& saveSystem);
    void AppendRenameText(const std::string& utf8);
    void CancelRename();
    void ConfirmRename(SaveSystem& saveSystem);

    // 是否正在输入（重命名 或 新建）
    bool IsRenaming() const;

    void SetEditingText(const std::string& utf8);

    // 每帧推进高亮块滑动
    void Update(float delta);



    // ==========================================================
    // 缩略图
    // ==========================================================
    //
    // Game 在创建存档时把当前帧的像素交给这里暂存，
    // 等玩家输入完名字、真正落盘时一起写进去。

    void SetPendingThumbnail(
        const std::vector<unsigned char>& rgba,
        int w, int h
    );



private:

    // 槽位号(1~10) -> 屏幕矩形
    SDL_Rect SlotRect(int slot) const;

    // 屏幕坐标 -> 槽位号，没命中返回 0
    int  HitTest(int x, int y) const;

    // 按像素宽度截断文字，超出用 "..." 收尾
    std::string FitText(const std::string& text, int maxWidth) const;

    SavePage page;
    int choice;             // 当前槽位号 1~10

    std::vector<SaveData> saveDataList;   // 固定 10 条

    // 用于取缩略图纹理。由 Game 绑定，可为空。
    SaveSystem* saveSystem = nullptr;

    bool confirmingDelete = false;


    // ---- 输入模式 ----

    bool renaming = false;   // 重命名已有存档
    bool creating = false;   // 新建存档命名

    std::string renameBuffer;        // 输入框内容
    std::string editingText;         // IME 预编辑（拼音）


    // ---- 新建模式下的上下文 ----

    std::string pendingScriptFile;
    std::string pendingChapterName;
    int pendingIndex = 0;
    std::map<std::string, int> pendingAffection;
    std::vector<AffectionRecord> pendingRecords;

    std::vector<unsigned char> pendingThumbnail;   // RGBA32
    int pendingThumbW = 0;
    int pendingThumbH = 0;


    // ---- 布局（2 列 x 5 行）----

    static constexpr int COLS = 2;
    static constexpr int ROWS = 5;

    static constexpr int CELL_W   = 690;
    static constexpr int CELL_H   = 118;
    static constexpr int CELL_GAP_X = 60;
    static constexpr int CELL_GAP_Y = 12;

    static constexpr int GRID_X = 70;
    static constexpr int GRID_Y = 186;

    // 缩略图尺寸（16:9）
    static constexpr int THUMB_W = 152;
    static constexpr int THUMB_H = 86;

    // 文字相对单元格的偏移。
    // 正文是 32 号字，行距必须大于 32，否则两行会叠在一起。
    static constexpr int TEXT_DX = THUMB_W + 20;
    static constexpr int TEXT_DY = 8;
    static constexpr int TEXT_GAP = 34;

    // 文字区可用宽度（由 Game 里实测的字宽推算，留一点余量）
    static constexpr int TEXT_MAX_W = CELL_W - TEXT_DX - 12;


    // 高亮块显示位置（平滑后的左上角）
    float displayX = (float)GRID_X;
    float displayY = (float)GRID_Y;

    static constexpr float HL_ANIM_SPEED      = 0.25f;
    static constexpr float HL_ANIM_SPEED_FAST = 0.55f;

    float hlSpeed = HL_ANIM_SPEED;

};


#endif
