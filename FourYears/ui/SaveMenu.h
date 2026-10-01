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



    void BeginDelete();

    void CancelDelete();

    void ConfirmDelete(
        SaveSystem& saveSystem
    );

    bool IsConfirmingDelete() const;



    void BeginRename();

    void HandleRenameKey(
        SDL_Keycode sym,
        SaveSystem& saveSystem
    );

    void AppendRenameText(
        const std::string& utf8
    );

    void CancelRename();

    void ConfirmRename(
        SaveSystem& saveSystem
    );

    bool IsRenaming() const;


    // ==========================================================
    // [新增] 预编辑文本（IME 正在输入的拼音）
    // ==========================================================
    //
    // SDL_TEXTEDITING 事件里带的是"还没上屏"的内容，
    // 例如输入法里刚敲的 "nihao"。
    // 用来在输入框下方显示，让玩家知道自己正在打什么。
    //
    // 上屏后（SDL_TEXTINPUT）会清空。

    void SetEditingText(
        const std::string& utf8
    );

    // ==========================================================



private:


    SavePage page;



    int choice;



    std::vector<std::string> saves;



    std::vector<SaveData> saveDataList;



    bool confirmingDelete = false;



    bool renaming = false;
    std::string renameBuffer;
    std::string renameTargetFile;


    // [新增] IME 预编辑文本
    std::string editingText;



    static constexpr int LIST_X = 500;
    static constexpr int LIST_Y = 200;
    static constexpr int LIST_GAP = 50;
    static constexpr int LIST_W = 900;
    static constexpr int LIST_H = 45;

};


#endif