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



    // [修改] 带出 affection
    bool Confirm(
        SaveSystem& saveSystem,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& outIndex,
        std::map<std::string, int>& outAffection
    );



    // [修改] 加 affection 参数
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

    void CancelRename();

    void ConfirmRename(
        SaveSystem& saveSystem
    );

    bool IsRenaming() const;



private:


    SavePage page;



    int choice;



    std::vector<std::string> saves;



    std::vector<SaveData> saveDataList;



    bool confirmingDelete = false;



    bool renaming = false;
    std::string renameBuffer;
    std::string renameTargetFile;



    static constexpr int LIST_X = 500;
    static constexpr int LIST_Y = 200;
    static constexpr int LIST_GAP = 50;
    static constexpr int LIST_W = 900;
    static constexpr int LIST_H = 45;

};


#endif