#include "SaveMenu.h"

#include <iostream>



SaveMenu::SaveMenu()
{

    choice=0;

    page=SavePage::LOAD;

}




void SaveMenu::SetPage(
    SavePage value
)
{

    page=value;

    choice=0;

    // 切页时顺便取消删除确认
    confirmingDelete=false;

}





void SaveMenu::Refresh(
    SaveSystem& saveSystem
)
{

    saves.clear();

    saveDataList.clear();



    saveDataList =
        saveSystem.GetSaveList();



    // [修改] 每行拼成：
    //   displayName  [第N章]  时间
    // 之前只显示 displayName，
    // 看不出章节和保存时间。
    for(auto& data : saveDataList)
    {

        std::string line = data.displayName;

        line += "  [第";
        line += std::to_string(data.chapter);
        line += "章]  ";
        line += data.time;

        saves.push_back(line);

    }



    if(
        saves.empty()
    )
    {

        saves.push_back(
            "没有存档"
        );

    }



    choice=0;

    confirmingDelete=false;

}





void SaveMenu::Render(
    Renderer& renderer
)
{


    if(
        page==SavePage::LOAD
    )
    {

        renderer.DrawText(
            "读取存档",
            600,
            100
        );

    }

    else
    {

        renderer.DrawText(
            "存档管理",
            600,
            100
        );

    }




    for(
        int i=0;
        i<(int)saves.size();
        i++
    )
    {

        std::string text;


        if(
            i==choice
        )
        {

            text=
            "> "
            +
            saves[i];

        }
        else
        {

            text=
            saves[i];

        }



        renderer.DrawText(
            text,
            LIST_X,
            LIST_Y+i*LIST_GAP
        );

    }




    // [修改] 底部提示区域：
    // 处于删除确认时，显示确认提示；
    // 否则显示常规操作提示。
    if(confirmingDelete && !saveDataList.empty())
    {

        SaveData data = GetCurrentSave();

        if(!data.filename.empty())
        {

            std::string msg =
                "确认删除 \""
                + data.displayName
                + "\" ？   Enter 确认 / ESC 取消";

            renderer.DrawText(
                msg,
                380,
                800
            );

        }

    }
    else
    {

        if(
            page==SavePage::LOAD
        )
        {

            renderer.DrawText(
                "Enter 读取存档  N 新建  C 复制  Delete 删除  R 重命名",
                300,
                750
            );

        }
        else
        {

            renderer.DrawText(
                "N 新建  C 复制  Delete 删除  R 重命名",
                350,
                750
            );

        }

    }


    // 返回按钮
    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );

}





void SaveMenu::HandleInput(
    int key
)
{


    if(
        saves.empty()
    )
    {
        return;
    }




    // 上

    if(
        key==1
    )
    {

        choice--;


        if(
            choice<0
        )
        {

            choice=
            saves.size()-1;

        }

    }




    // 下

    else if(
        key==2
    )
    {

        choice++;


        if(
            choice>=
            (int)saves.size()
        )
        {

            choice=0;

        }

    }

}





int SaveMenu::GetChoice() const
{

    return choice;

}





void SaveMenu::Reset()
{

    choice=0;

    confirmingDelete=false;

}





SaveData SaveMenu::GetCurrentSave()
{

    if(
        choice>=0
        &&
        choice<(int)saveDataList.size()
    )
    {

        return saveDataList[choice];

    }



    return SaveData();

}





void SaveMenu::Create(
    SaveSystem& saveSystem,
    const std::string& name,
    int chapter,
    int index
)
{

    saveSystem.CreateSave(
        name,
        chapter,
        index
    );



    Refresh(
        saveSystem
    );

}





void SaveMenu::Delete(
    SaveSystem& saveSystem
)
{

    // [说明] 直接删除的逻辑保留在 ConfirmDelete()，
    // Delete() 保留只是为了避免破坏其他调用点。
    // 现在 Game 走的是 BeginDelete / ConfirmDelete 流程。

    SaveData data=
        GetCurrentSave();



    if(
        data.filename.empty()
    )
    {
        return;
    }



    saveSystem.DeleteSave(
        data.filename
    );



    Refresh(
        saveSystem
    );

}





void SaveMenu::Copy(
    SaveSystem& saveSystem
)
{


    SaveData data=
        GetCurrentSave();



    if(
        data.filename.empty()
    )
    {
        return;
    }



    saveSystem.CopySave(
        data.filename
    );



    Refresh(
        saveSystem
    );

}





void SaveMenu::Rename(
    SaveSystem& saveSystem,
    const std::string& name
)
{


    SaveData data=
        GetCurrentSave();



    if(
        data.filename.empty()
    )
    {
        return;
    }




    saveSystem.RenameSave(
        data.filename,
        name
    );



    Refresh(
        saveSystem
    );

}





SavePage SaveMenu::GetPage() const
{

    return page;

}





bool SaveMenu::Confirm(
    SaveSystem& saveSystem,
    int& outChapter,
    int& outIndex
)
{

    if(
        saves.empty()
    )
    {
        return false;
    }



    if(
        page==SavePage::LOAD
    )
    {

        SaveData data =
            GetCurrentSave();



        if(
            data.filename.empty()
        )
        {
            return false;
        }



        return saveSystem.LoadSave(
            data.filename,
            outChapter,
            outIndex
        );

    }



    return false;

}





void SaveMenu::HandleMouseMove(
    int x,
    int y
)
{

    for(int i=0;i<(int)saves.size();i++)
    {

        int ix = LIST_X;
        int iy = LIST_Y + i * LIST_GAP;

        if(
            x >= ix &&
            x <  ix + LIST_W &&
            y >= iy &&
            y <  iy + LIST_H
        )
        {
            choice = i;
            return;
        }

    }

}





MenuMouseResult SaveMenu::HandleMouseClick(
    int x,
    int y
)
{

    // 返回按钮
    if(
        x >= UILayout::BACK_X &&
        x <  UILayout::BACK_X + UILayout::BACK_W &&
        y >= UILayout::BACK_Y &&
        y <  UILayout::BACK_Y + UILayout::BACK_H
    )
    {
        // [新增] 处于删除确认时，
        // 点"返回"等同取消确认，不离开页面。
        if(confirmingDelete)
        {
            confirmingDelete = false;
            return MenuMouseResult::NONE;
        }

        return MenuMouseResult::BACK;
    }


    // [新增] 处于删除确认时，
    // 点击任意存档项等同"确认删除"
    if(confirmingDelete)
    {
        // 具体确认动作由 Game 处理，
        // 这里只返回 ACTIVATE 信号。
        return MenuMouseResult::ACTIVATE;
    }


    // 存档项：点击 = 激活（走 Confirm 读档）
    for(int i=0;i<(int)saves.size();i++)
    {

        int ix = LIST_X;
        int iy = LIST_Y + i * LIST_GAP;

        if(
            x >= ix &&
            x <  ix + LIST_W &&
            y >= iy &&
            y <  iy + LIST_H
        )
        {
            choice = i;
            return MenuMouseResult::ACTIVATE;
        }

    }


    return MenuMouseResult::NONE;

}





// ==========================================================
// [新增] 删除确认
// ==========================================================

void SaveMenu::BeginDelete()
{

    // 没有可删除的目标时不进入确认状态
    if(GetCurrentSave().filename.empty())
    {
        return;
    }


    confirmingDelete = true;

}





void SaveMenu::CancelDelete()
{

    confirmingDelete = false;

}





void SaveMenu::ConfirmDelete(
    SaveSystem& saveSystem
)
{

    SaveData data = GetCurrentSave();


    if(!data.filename.empty())
    {

        saveSystem.DeleteSave(
            data.filename
        );


        Refresh(
            saveSystem
        );

    }


    confirmingDelete = false;

}





bool SaveMenu::IsConfirmingDelete() const
{

    return confirmingDelete;

}