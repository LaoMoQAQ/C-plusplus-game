#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include "../core/Renderer.h"
#include "../core/SaveSystem.h"

#include "../SavePage.h"

#include "MenuCommon.h"

#include <SDL.h>

#include <vector>
#include <string>


class SaveMenu
{

public:


    SaveMenu();



    void Render(
        Renderer& renderer
    );



    void HandleInput(
        int key
    );



    void SetPage(
        SavePage page
    );



    void Refresh(
        SaveSystem& saveSystem
    );



    int GetChoice() const;



    void Reset();



    // [修改] 带出 chapterName
    bool Confirm(
        SaveSystem& saveSystem,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& outIndex
    );



    // [修改] chapter -> chapterName
    void Create(
        SaveSystem& saveSystem,
        const std::string& name,
        const std::string& scriptFile,
        const std::string& chapterName,
        int index
    );



    void Delete(
        SaveSystem& saveSystem
    );



    void Copy(
        SaveSystem& saveSystem
    );



    SaveData GetCurrentSave();


    SavePage GetPage() const;



    void HandleMouseMove(
        int x,
        int y
    );

    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );



    // ==========================================================
    // 删除确认
    // ==========================================================

    void BeginDelete();

    void CancelDelete();

    void ConfirmDelete(
        SaveSystem& saveSystem
    );

    bool IsConfirmingDelete() const;



    // ==========================================================
    // [新增] 重命名输入模式
    // ==========================================================
    //
    // 按 R 进入输入模式：
    //   - 字母/数字/空格追加
    //   - 退格删除
    //   - Enter 确认
    //   - ESC 取消
    //
    // 不依赖 SDL_TEXTINPUT，所以不需要恢复 IME。
    // 输入内容只支持英文和数字。

    void BeginRename();

    void HandleRenameKey(
        SDL_Keycode sym,
        SaveSystem& saveSystem
    );

    void CancelRename();

    void ConfirmRename(
        SaveSystem& saveSystem
    );

    bool IsRenaming() const;

    // ==========================================================



private:


    SavePage page;



    int choice;



    std::vector<std::string> saves;



    std::vector<SaveData> saveDataList;



    bool confirmingDelete = false;



    // 重命名状态
    bool renaming = false;

    // 正在编辑的名字
    std::string renameBuffer;

    // 正在被重命名的存档文件名
    std::string renameTargetFile;



    static constexpr int LIST_X = 500;
    static constexpr int LIST_Y = 200;
    static constexpr int LIST_GAP = 50;
    static constexpr int LIST_W = 900;
    static constexpr int LIST_H = 45;

};


#endif