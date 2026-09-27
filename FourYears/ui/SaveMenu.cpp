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

    confirmingDelete=false;

    renaming=false;

    renameBuffer.clear();

    renameTargetFile.clear();

}





void SaveMenu::Refresh(
    SaveSystem& saveSystem
)
{

    saves.clear();

    saveDataList.clear();



    saveDataList =
        saveSystem.GetSaveList();



    for(auto& data : saveDataList)
    {

        std::string line = data.displayName;

        line += "  [";
        line += data.chapterName;
        line += "]  ";
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




    // ==========================================================
    // 底部提示区域
    // ==========================================================

    if(renaming)
    {

        // 输入模式：显示输入框
        std::string line =
            "重命名: " + renameBuffer + "_";

        renderer.DrawText(
            line,
            400,
            780
        );

        renderer.DrawText(
            "Enter 确认 / ESC 取消",
            400,
            830
        );

    }
    else if(confirmingDelete && !saveDataList.empty())
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

    // 重命名模式或删除确认时不响应上下
    if(renaming)
    {
        return;
    }


    if(
        saves.empty()
    )
    {
        return;
    }




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

    renaming=false;

    renameBuffer.clear();

    renameTargetFile.clear();

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
    const std::string& scriptFile,
    const std::string& chapterName,
    int index
)
{

    saveSystem.CreateSave(
        name,
        scriptFile,
        chapterName,
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





SavePage SaveMenu::GetPage() const
{

    return page;

}





bool SaveMenu::Confirm(
    SaveSystem& saveSystem,
    std::string& outScriptFile,
    std::string& outChapterName,
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
            outScriptFile,
            outChapterName,
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

    if(renaming)
    {
        return;
    }


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

    if(renaming)
    {
        return MenuMouseResult::NONE;
    }


    if(
        x >= UILayout::BACK_X &&
        x <  UILayout::BACK_X + UILayout::BACK_W &&
        y >= UILayout::BACK_Y &&
        y <  UILayout::BACK_Y + UILayout::BACK_H
    )
    {
        if(confirmingDelete)
        {
            confirmingDelete = false;
            return MenuMouseResult::NONE;
        }

        return MenuMouseResult::BACK;
    }


    if(confirmingDelete)
    {
        return MenuMouseResult::ACTIVATE;
    }


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





void SaveMenu::BeginDelete()
{

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





// ==========================================================
// [新增] 重命名输入模式
// ==========================================================

void SaveMenu::BeginRename()
{

    SaveData data = GetCurrentSave();


    if(data.filename.empty())
    {
        return;
    }


    renaming = true;

    renameTargetFile = data.filename;

    // 用当前名字做初始内容，方便用户改
    renameBuffer = data.displayName;

}





void SaveMenu::HandleRenameKey(
    SDL_Keycode sym,
    SaveSystem& saveSystem
)
{

    if(!renaming)
    {
        return;
    }


    // Enter 确认
    if(sym == SDLK_RETURN)
    {

        ConfirmRename(saveSystem);

        return;

    }


    // ESC 取消
    if(sym == SDLK_ESCAPE)
    {

        CancelRename();

        return;

    }


    // [修改] 退格：按 UTF-8 字符边界删除。
    //
    // renameBuffer 里存的是 UTF-8 字符串，
    // 一个中文字符占 3 字节。
    // 之前的 pop_back() 只删 1 字节，
    // 剩下半个字符渲染成方框。
    //
    // 正确做法：
    //   从末尾往前扫，
    //   跳过所有 continuation 字节（10xxxxxx），
    //   删到第一个非 continuation 字节为止。
    if(sym == SDLK_BACKSPACE)
    {

        if(!renameBuffer.empty())
        {

            int i =
                (int)renameBuffer.size() - 1;


            while(
                i > 0
                &&
                (
                    (unsigned char)renameBuffer[i]
                    & 0xC0
                )
                ==
                0x80
            )
            {
                i--;
            }


            renameBuffer.erase(
                i
            );

        }

        return;

    }


    // 空格
    if(sym == SDLK_SPACE)
    {

        renameBuffer += ' ';

        return;

    }


    // 字母 a-z
    if(sym >= SDLK_a && sym <= SDLK_z)
    {

        renameBuffer +=
            (char)('a' + (sym - SDLK_a));

        return;

    }


    // 数字 0-9
    if(sym >= SDLK_0 && sym <= SDLK_9)
    {

        renameBuffer +=
            (char)('0' + (sym - SDLK_0));

        return;

    }

}





void SaveMenu::CancelRename()
{

    renaming = false;

    renameBuffer.clear();

    renameTargetFile.clear();

}





void SaveMenu::ConfirmRename(
    SaveSystem& saveSystem
)
{

    if(
        renaming
        &&
        !renameTargetFile.empty()
        &&
        !renameBuffer.empty()
    )
    {

        saveSystem.RenameSave(
            renameTargetFile,
            renameBuffer
        );


        Refresh(
            saveSystem
        );

    }


    renaming = false;

    renameBuffer.clear();

    renameTargetFile.clear();

}





bool SaveMenu::IsRenaming() const
{

    return renaming;

}