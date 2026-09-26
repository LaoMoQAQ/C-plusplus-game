#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include "../core/Renderer.h"
#include "../core/SaveSystem.h"

#include "../SavePage.h"

#include "MenuCommon.h"

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



    bool Confirm(
        SaveSystem& saveSystem,
        int& outChapter,
        int& outIndex
    );



    void Create(
        SaveSystem& saveSystem,
        const std::string& name,
        int chapter,
        int index
    );



    void Delete(
        SaveSystem& saveSystem
    );



    void Copy(
        SaveSystem& saveSystem
    );



    void Rename(
        SaveSystem& saveSystem,
        const std::string& name
    );



    SaveData GetCurrentSave();


    SavePage GetPage() const;



    // 鼠标
    void HandleMouseMove(
        int x,
        int y
    );

    MenuMouseResult HandleMouseClick(
        int x,
        int y
    );



    // [新增] 删除确认
    //
    // 流程：
    //   按 Delete -> BeginDelete() 进入确认状态
    //   再按 Enter 或 Delete -> ConfirmDelete() 真删
    //   按 ESC -> CancelDelete() 取消
    //
    // 目的：避免误按 Delete 直接把存档删掉。
    void BeginDelete();

    void CancelDelete();

    void ConfirmDelete(
        SaveSystem& saveSystem
    );

    bool IsConfirmingDelete() const;



private:


    SavePage page;



    int choice;



    std::vector<std::string> saves;



    std::vector<SaveData> saveDataList;



    // [新增] 是否处于"确认删除"状态
    bool confirmingDelete = false;



    // 存档列表坐标。
    // Render 和鼠标命中都用这几个常量。
    static constexpr int LIST_X = 500;
    static constexpr int LIST_Y = 200;
    static constexpr int LIST_GAP = 50;
    static constexpr int LIST_W = 900;    // [修改] 加宽，容纳章节+时间
    static constexpr int LIST_H = 45;

};


#endif