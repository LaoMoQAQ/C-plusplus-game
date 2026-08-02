#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include "../core/Renderer.h"
#include "../core/SaveSystem.h"

#include "../SavePage.h"

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


    void Confirm(
        SaveSystem& saveSystem
    );


    // 新增


    void Create(
        SaveSystem& saveSystem
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



private:


    SavePage page;



    int choice;



    std::vector<std::string> saves;



    // 保存真实数据

    std::vector<SaveData> saveDataList;


};


#endif