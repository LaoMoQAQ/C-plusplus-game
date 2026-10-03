#include "SaveMenu.h"

#include <iostream>



SaveMenu::SaveMenu()
{
    choice = 0;
    page = SavePage::LOAD;

    displayHighlightY = (float)LIST_Y;
}





void SaveMenu::SetPage(SavePage value)
{
    page = value;
    choice = 0;

    confirmingDelete = false;
    renaming = false;
    creating = false;
    renameBuffer.clear();
    renameTargetFile.clear();
    editingText.clear();
}





void SaveMenu::Refresh(SaveSystem& saveSystem)
{
    saves.clear();
    saveDataList.clear();

    saveDataList = saveSystem.GetSaveList();

    for(auto& data : saveDataList)
    {
        std::string line = data.displayName;
        line += "  [";
        line += data.chapterName;
        line += "]  ";
        line += data.time;

        saves.push_back(line);
    }

    if(saves.empty())
    {
        saves.push_back("没有存档");
    }

    choice = 0;
    confirmingDelete = false;
}





void SaveMenu::Update()
{
    float target = (float)(LIST_Y + choice * LIST_GAP);

    float diff = target - displayHighlightY;

    if(diff > -0.5f && diff < 0.5f)
        displayHighlightY = target;
    else
        displayHighlightY += diff * hlSpeed;
}





void SaveMenu::Render(Renderer& renderer)
{
    // ---- 全屏遮罩 ----

    renderer.DrawFilledRect(
        0, 0,
        UILayout::SCREEN_W,
        UILayout::SCREEN_H,
        UILayout::OVERLAY_R,
        UILayout::OVERLAY_G,
        UILayout::OVERLAY_B,
        UILayout::OVERLAY_A
    );


    // ---- 标题 ----

    std::string title =
        (page == SavePage::LOAD) ? "读取存档" : "存档管理";

    renderer.DrawText(title, 700, 100);

    renderer.DrawFilledRect(
        650, 150, 300, 2,
        UILayout::LINE_R,
        UILayout::LINE_G,
        UILayout::LINE_B,
        UILayout::LINE_A
    );


    // ---- 高亮块 ----

    if(!saves.empty() && !IsRenaming())
    {
        renderer.DrawFilledRoundRect(
            LIST_X - 20,
            (int)displayHighlightY - 6,
            LIST_W + 40,
            LIST_H - 4,
            UILayout::HL_RADIUS,
            UILayout::HL_R,
            UILayout::HL_G,
            UILayout::HL_B,
            UILayout::HL_A
        );
    }


    // ---- 存档列表 ----

    for(int i = 0; i < (int)saves.size(); i++)
    {
        int iy = LIST_Y + i * LIST_GAP;

        std::string text;

        if(i == choice && !IsRenaming())
            text = "> " + saves[i];
        else if(i == choice)
            text = "  " + saves[i];
        else
            text = "  " + saves[i];

        renderer.DrawText(text, LIST_X, iy);
    }


    // ---- 底部 ----

    if(IsRenaming())
    {
        // 输入模式：区分新建 / 重命名
        std::string prefix =
            creating ? "新建存档: " : "重命名: ";

        std::string line = prefix + renameBuffer + "_";
        renderer.DrawText(line, 400, 780);

        if(!editingText.empty())
        {
            std::string pinyinLine = "拼音: " + editingText;
            renderer.DrawText(pinyinLine, 400, 815);
        }
        else
        {
            renderer.DrawText(
                "支持中文输入  Enter 确认 / ESC 取消",
                400, 815
            );
        }
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

            renderer.DrawText(msg, 380, 800);
        }
    }
    else
    {
        if(page == SavePage::LOAD)
        {
            renderer.DrawText(
                "Enter 读取存档  N 新建  C 复制  Delete 删除  R 重命名",
                300, 750
            );
        }
        else
        {
            renderer.DrawText(
                "N 新建  C 复制  Delete 删除  R 重命名",
                350, 750
            );
        }
    }


    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );
}





void SaveMenu::HandleInput(int key)
{
    if(IsRenaming()) return;
    if(saves.empty()) return;

    hlSpeed = HL_ANIM_SPEED;

    if(key == 1)
    {
        choice--;
        if(choice < 0)
            choice = saves.size() - 1;
    }
    else if(key == 2)
    {
        choice++;
        if(choice >= (int)saves.size())
            choice = 0;
    }
}





int SaveMenu::GetChoice() const
{
    return choice;
}





void SaveMenu::Reset()
{
    choice = 0;
    confirmingDelete = false;
    renaming = false;
    creating = false;
    renameBuffer.clear();
    renameTargetFile.clear();
    editingText.clear();
}





SaveData SaveMenu::GetCurrentSave()
{
    if(choice >= 0 && choice < (int)saveDataList.size())
        return saveDataList[choice];

    return SaveData();
}





void SaveMenu::Create(
    SaveSystem& saveSystem,
    const std::string& name,
    const std::string& scriptFile,
    const std::string& chapterName,
    int index,
    const std::map<std::string, int>& affection
)
{
    saveSystem.CreateSave(
        name,
        scriptFile,
        chapterName,
        index,
        affection
    );

    Refresh(saveSystem);
}





void SaveMenu::Delete(SaveSystem& saveSystem)
{
    SaveData data = GetCurrentSave();

    if(data.filename.empty()) return;

    saveSystem.DeleteSave(data.filename);
    Refresh(saveSystem);
}





void SaveMenu::Copy(SaveSystem& saveSystem)
{
    SaveData data = GetCurrentSave();

    if(data.filename.empty()) return;

    saveSystem.CopySave(data.filename);
    Refresh(saveSystem);
}





SavePage SaveMenu::GetPage() const
{
    return page;
}





bool SaveMenu::Confirm(
    SaveSystem& saveSystem,
    std::string& outScriptFile,
    std::string& outChapterName,
    int& outIndex,
    std::map<std::string, int>& outAffection
)
{
    if(saves.empty()) return false;

    if(page == SavePage::LOAD)
    {
        SaveData data = GetCurrentSave();

        if(data.filename.empty()) return false;

        return saveSystem.LoadSave(
            data.filename,
            outScriptFile,
            outChapterName,
            outIndex,
            outAffection
        );
    }

    return false;
}





void SaveMenu::HandleMouseMove(int x, int y)
{
    if(IsRenaming()) return;

    for(int i = 0; i < (int)saves.size(); i++)
    {
        int ix = LIST_X;
        int iy = LIST_Y + i * LIST_GAP;

        if(x >= ix && x < ix + LIST_W &&
           y >= iy && y < iy + LIST_H)
        {
            hlSpeed = HL_ANIM_SPEED_FAST;
            choice = i;
            return;
        }
    }
}





MenuMouseResult SaveMenu::HandleMouseClick(int x, int y)
{
    if(IsRenaming())
        return MenuMouseResult::NONE;


    if(x >= UILayout::BACK_X &&
       x <  UILayout::BACK_X + UILayout::BACK_W &&
       y >= UILayout::BACK_Y &&
       y <  UILayout::BACK_Y + UILayout::BACK_H)
    {
        if(confirmingDelete)
        {
            confirmingDelete = false;
            return MenuMouseResult::NONE;
        }

        return MenuMouseResult::BACK;
    }


    if(confirmingDelete)
        return MenuMouseResult::ACTIVATE;


    for(int i = 0; i < (int)saves.size(); i++)
    {
        int ix = LIST_X;
        int iy = LIST_Y + i * LIST_GAP;

        if(x >= ix && x < ix + LIST_W &&
           y >= iy && y < iy + LIST_H)
        {
            choice = i;
            return MenuMouseResult::ACTIVATE;
        }
    }

    return MenuMouseResult::NONE;
}





void SaveMenu::BeginDelete()
{
    if(GetCurrentSave().filename.empty()) return;

    confirmingDelete = true;
}





void SaveMenu::CancelDelete()
{
    confirmingDelete = false;
}





void SaveMenu::ConfirmDelete(SaveSystem& saveSystem)
{
    SaveData data = GetCurrentSave();

    if(!data.filename.empty())
    {
        saveSystem.DeleteSave(data.filename);
        Refresh(saveSystem);
    }

    confirmingDelete = false;
}





bool SaveMenu::IsConfirmingDelete() const
{
    return confirmingDelete;
}





// ==========================================================
// 输入模式
// ==========================================================

void SaveMenu::BeginRename()
{
    SaveData data = GetCurrentSave();

    if(data.filename.empty()) return;

    renaming = true;
    creating = false;

    renameTargetFile = data.filename;
    renameBuffer = data.displayName;

    editingText.clear();
}





void SaveMenu::BeginCreate(
    const std::string& scriptFile,
    const std::string& chapterName,
    int index,
    const std::map<std::string, int>& affection
)
{
    renaming = false;
    creating = true;

    renameTargetFile.clear();
    renameBuffer.clear();
    editingText.clear();

    pendingScriptFile = scriptFile;
    pendingChapterName = chapterName;
    pendingIndex = index;
    pendingAffection = affection;
}





void SaveMenu::HandleRenameKey(
    SDL_Keycode sym,
    SaveSystem& saveSystem
)
{
    if(!IsRenaming()) return;


    if(sym == SDLK_RETURN)
    {
        ConfirmRename(saveSystem);
        return;
    }

    if(sym == SDLK_ESCAPE)
    {
        CancelRename();
        return;
    }


    // 退格：按 UTF-8 字符边界删除
    if(sym == SDLK_BACKSPACE)
    {
        if(!renameBuffer.empty())
        {
            int i = (int)renameBuffer.size() - 1;

            while(
                i > 0 &&
                ((unsigned char)renameBuffer[i] & 0xC0) == 0x80
            )
            {
                i--;
            }

            renameBuffer.erase(i);
        }

        return;
    }
}





void SaveMenu::AppendRenameText(const std::string& utf8)
{
    if(!IsRenaming()) return;

    // 限长 30 个 UTF-8 字节
    if(renameBuffer.size() + utf8.size() > 30)
        return;

    renameBuffer += utf8;

    // 上屏后清空预编辑显示
    editingText.clear();
}





void SaveMenu::CancelRename()
{
    renaming = false;
    creating = false;

    renameBuffer.clear();
    renameTargetFile.clear();
    editingText.clear();
}





void SaveMenu::ConfirmRename(SaveSystem& saveSystem)
{
    // ---- 新建 ----

    if(creating)
    {
        if(!renameBuffer.empty())
        {
            saveSystem.CreateSave(
                renameBuffer,
                pendingScriptFile,
                pendingChapterName,
                pendingIndex,
                pendingAffection
            );

            Refresh(saveSystem);
        }
    }

    // ---- 重命名 ----

    else if(renaming &&
            !renameTargetFile.empty() &&
            !renameBuffer.empty())
    {
        saveSystem.RenameSave(
            renameTargetFile,
            renameBuffer
        );

        Refresh(saveSystem);
    }


    renaming = false;
    creating = false;

    renameBuffer.clear();
    renameTargetFile.clear();
    editingText.clear();
}





bool SaveMenu::IsRenaming() const
{
    return renaming || creating;
}





void SaveMenu::SetEditingText(const std::string& utf8)
{
    if(!IsRenaming()) return;

    editingText = utf8;
}