#include "SaveMenu.h"

#include <iostream>



SaveMenu::SaveMenu()
{
    choice = 1;
    page = SavePage::LOAD;

    saveDataList.resize(SAVE_SLOT_COUNT);
}




void SaveMenu::SetPage(SavePage value)
{
    page = value;
    choice = 1;

    confirmingDelete = false;
    renaming = false;
    creating = false;
    renameBuffer.clear();
    editingText.clear();
}




void SaveMenu::SetSaveSystem(SaveSystem* system)
{
    saveSystem = system;
}




void SaveMenu::Refresh(SaveSystem& saveSystem)
{
    saveDataList = saveSystem.GetSaveList();

    confirmingDelete = false;
}




// ==========================================================
// 布局
// ==========================================================

SDL_Rect SaveMenu::SlotRect(int slot) const
{
    SDL_Rect r = { 0, 0, 0, 0 };

    if(slot < 1 || slot > SAVE_SLOT_COUNT)
        return r;

    int idx = slot - 1;
    int col = idx / ROWS;       // 0 或 1
    int row = idx % ROWS;

    r.x = GRID_X + col * (CELL_W + CELL_GAP_X);
    r.y = GRID_Y + row * (CELL_H + CELL_GAP_Y);
    r.w = CELL_W;
    r.h = CELL_H;

    return r;
}




int SaveMenu::HitTest(int x, int y) const
{
    for(int i = 1; i <= SAVE_SLOT_COUNT; i++)
    {
        SDL_Rect r = SlotRect(i);

        if(x >= r.x && x < r.x + r.w &&
           y >= r.y && y < r.y + r.h)
        {
            return i;
        }
    }

    return 0;
}




// ==========================================================
// FitText
// ==========================================================
//
// 按像素宽度截断文字，超出部分用 "…" 收尾。
//
// 这里只是粗略估算：正文用 simhei 32 号，中文字符约 32px 宽，
// ASCII 约 16px。够用，且不依赖 TTF_SizeUTF8 的额外调用。

std::string SaveMenu::FitText(
    const std::string& text,
    int maxWidth
) const
{
    if(text.empty() || maxWidth <= 0)
        return text;


    int width = 0;
    size_t i = 0;

    for(; i < text.size();)
    {
        unsigned char c = (unsigned char)text[i];

        size_t len = 1;
        int adv = 16;

        if((c & 0x80) == 0)
        {
            len = 1;
            adv = 16;
        }
        else if((c & 0xE0) == 0xC0)
        {
            len = 2;
            adv = 32;
        }
        else if((c & 0xF0) == 0xE0)
        {
            len = 3;
            adv = 32;
        }
        else if((c & 0xF8) == 0xF0)
        {
            len = 4;
            adv = 32;
        }

        if(width + adv > maxWidth)
        {
            // 放不下了，截断并加省略号
            return text.substr(0, i) + "...";
        }

        width += adv;
        i += len;
    }

    return text;
}




void SaveMenu::Update(float delta)
{
    SDL_Rect r = SlotRect(choice);

    float tx = (float)r.x;
    float ty = (float)r.y;

    float dx = tx - displayX;
    float dy = ty - displayY;

    float f = SmoothFactor(hlSpeed, delta);

    if(dx > -0.5f && dx < 0.5f) displayX = tx;
    else                        displayX += dx * f;

    if(dy > -0.5f && dy < 0.5f) displayY = ty;
    else                        displayY += dy * f;
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

    if(!IsRenaming())
    {
        renderer.DrawFilledRoundRect(
            (int)displayX - 6,
            (int)displayY - 6,
            CELL_W + 12,
            CELL_H + 12,
            UILayout::HL_RADIUS,
            UILayout::HL_R,
            UILayout::HL_G,
            UILayout::HL_B,
            UILayout::HL_A
        );
    }


    // ---- 10 个槽位 ----

    for(int i = 1; i <= SAVE_SLOT_COUNT; i++)
    {
        SDL_Rect r = SlotRect(i);

        const SaveData& data = saveDataList[i - 1];

        bool selected = (i == choice);


        // 缩略图区：有图就画图，没图画个暗色占位框

        SDL_Texture* thumb = nullptr;

        if(data.used && saveSystem)
            thumb = saveSystem->GetThumbnail(renderer.GetSDLRenderer(), i);

        SDL_Rect thumbRect = {
            r.x + 12,
            r.y + (CELL_H - THUMB_H) / 2,
            THUMB_W,
            THUMB_H
        };

        if(thumb)
        {
            renderer.DrawTexture(
                thumb,
                thumbRect.x, thumbRect.y,
                thumbRect.w, thumbRect.h
            );
        }
        else
        {
            renderer.DrawFilledRoundRect(
                thumbRect.x, thumbRect.y,
                thumbRect.w, thumbRect.h,
                6,
                30, 30, 40, 180
            );

            renderer.DrawText(
                "空",
                thumbRect.x + 66,
                thumbRect.y + 30
            );
        }


        // 文字

        int tx = r.x + TEXT_DX;
        int ty = r.y + TEXT_DY;

        if(data.used)
        {
            std::string line1 = data.displayName;
            if(line1.empty()) line1 = "未命名";

            renderer.DrawText(
                FitText(line1, TEXT_MAX_W),
                tx, ty
            );

            std::string line2 =
                "[" + data.chapterName + "]  " + data.time;

            renderer.DrawText(
                FitText(line2, TEXT_MAX_W),
                tx, ty + TEXT_GAP
            );

            std::string line3 = "槽位 " + std::to_string(i);

            if(selected)
                line3 = "> " + line3;

            renderer.DrawText(line3, tx, ty + TEXT_GAP * 2);
        }
        else
        {
            std::string line1 = "空槽位 " + std::to_string(i);
            if(selected) line1 = "> " + line1;

            renderer.DrawText(line1, tx, ty + TEXT_GAP);
        }
    }


    // ---- 底部 ----

    if(IsRenaming())
    {
        std::string prefix =
            creating ? "新建存档: " : "重命名: ";

        std::string line = prefix + renameBuffer + "_";
        renderer.DrawText(line, 400, 830);

        if(!editingText.empty())
        {
            std::string pinyinLine = "拼音: " + editingText;
            renderer.DrawText(pinyinLine, 400, 862);
        }
        else
        {
            renderer.DrawText(
                "支持中文输入  Enter 确认 / ESC 取消",
                400, 862
            );
        }
    }
    else if(confirmingDelete)
    {
        SaveData data = GetCurrentSave();

        if(data.used)
        {
            std::string msg =
                "确认删除槽位 "
                + std::to_string(choice)
                + " \"" + data.displayName
                + "\" ？   Enter 确认 / ESC 取消";

            renderer.DrawText(msg, 380, 838);
        }
    }
    else
    {
        if(page == SavePage::LOAD)
        {
            renderer.DrawText(
                "Enter 读取  N 新建  C 复制  Delete 删除  R 重命名",
                280, 838
            );
        }
        else
        {
            renderer.DrawText(
                "Enter 保存  N 新建  C 复制  Delete 删除  R 重命名",
                280, 838
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

    hlSpeed = HL_ANIM_SPEED;


    int col = (choice - 1) / ROWS;
    int row = (choice - 1) % ROWS;


    // 1=上 2=下 3=左 4=右

    if(key == 1)
    {
        row--;
        if(row < 0) row = ROWS - 1;
    }
    else if(key == 2)
    {
        row++;
        if(row >= ROWS) row = 0;
    }
    else if(key == 3)
    {
        col--;
        if(col < 0) col = COLS - 1;
    }
    else if(key == 4)
    {
        col++;
        if(col >= COLS) col = 0;
    }


    choice = col * ROWS + row + 1;

    if(choice < 1) choice = 1;
    if(choice > SAVE_SLOT_COUNT) choice = SAVE_SLOT_COUNT;
}




int SaveMenu::GetChoice() const
{
    return choice;
}




void SaveMenu::Reset()
{
    choice = 1;

    confirmingDelete = false;
    renaming = false;
    creating = false;

    renameBuffer.clear();
    editingText.clear();

    pendingThumbnail.clear();
    pendingThumbW = 0;
    pendingThumbH = 0;
}




SaveData SaveMenu::GetCurrentSave()
{
    if(choice >= 1 && choice <= (int)saveDataList.size())
        return saveDataList[choice - 1];

    return SaveData();
}




void SaveMenu::Delete(SaveSystem& saveSystem)
{
    SaveData data = GetCurrentSave();

    if(!data.used) return;

    saveSystem.DeleteSave(choice);
    Refresh(saveSystem);
}




void SaveMenu::Copy(SaveSystem& saveSystem)
{
    SaveData data = GetCurrentSave();

    if(!data.used) return;

    // 复制到下一个槽位，循环
    int target = choice + 1;
    if(target > SAVE_SLOT_COUNT) target = 1;

    saveSystem.CopySave(choice, target);
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
    std::map<std::string, int>& outAffection,
    std::vector<AffectionRecord>& outRecords
)
{
    if(page != SavePage::LOAD)
        return false;

    SaveData data = GetCurrentSave();

    if(!data.used)
        return false;

    return saveSystem.LoadSave(
        choice,
        outScriptFile,
        outChapterName,
        outIndex,
        outAffection,
        outRecords
    );
}




void SaveMenu::HandleMouseMove(int x, int y)
{
    if(IsRenaming()) return;

    int slot = HitTest(x, y);

    if(slot > 0)
    {
        hlSpeed = HL_ANIM_SPEED_FAST;
        choice = slot;
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


    int slot = HitTest(x, y);

    if(slot <= 0)
        return MenuMouseResult::NONE;


    // 删除确认状态下，只有点当前选中槽位才算确认
    if(confirmingDelete)
    {
        if(slot == choice)
            return MenuMouseResult::ACTIVATE;

        return MenuMouseResult::NONE;
    }


    choice = slot;
    return MenuMouseResult::ACTIVATE;
}




void SaveMenu::BeginDelete()
{
    if(!GetCurrentSave().used) return;

    confirmingDelete = true;
}




void SaveMenu::CancelDelete()
{
    confirmingDelete = false;
}




void SaveMenu::ConfirmDelete(SaveSystem& saveSystem)
{
    if(GetCurrentSave().used)
    {
        saveSystem.DeleteSave(choice);
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

    if(!data.used) return;

    renaming = true;
    creating = false;

    renameBuffer = data.displayName;

    editingText.clear();
}




void SaveMenu::BeginCreate(
    const std::string& scriptFile,
    const std::string& chapterName,
    int index,
    const std::map<std::string, int>& affection,
    const std::vector<AffectionRecord>& records
)
{
    renaming = false;
    creating = true;

    renameBuffer.clear();
    editingText.clear();

    pendingScriptFile = scriptFile;
    pendingChapterName = chapterName;
    pendingIndex = index;
    pendingAffection = affection;
    pendingRecords = records;
}




void SaveMenu::SetPendingThumbnail(
    const std::vector<unsigned char>& rgba,
    int w, int h
)
{
    pendingThumbnail = rgba;
    pendingThumbW = w;
    pendingThumbH = h;
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
    editingText.clear();

    pendingThumbnail.clear();
    pendingThumbW = 0;
    pendingThumbH = 0;
}




void SaveMenu::ConfirmRename(SaveSystem& saveSystem)
{
    // ---- 新建 ----

    if(creating)
    {
        std::string name = renameBuffer;

        if(name.empty())
            name = "存档 " + std::to_string(choice);

        saveSystem.CreateSave(
            choice,
            name,
            pendingScriptFile,
            pendingChapterName,
            pendingIndex,
            pendingAffection,
            pendingRecords,
            pendingThumbnail.empty() ? nullptr : pendingThumbnail.data(),
            pendingThumbW,
            pendingThumbH
        );

        Refresh(saveSystem);
    }

    // ---- 重命名 ----

    else if(renaming && !renameBuffer.empty())
    {
        saveSystem.RenameSave(choice, renameBuffer);

        Refresh(saveSystem);
    }


    renaming = false;
    creating = false;

    renameBuffer.clear();
    editingText.clear();

    pendingThumbnail.clear();
    pendingThumbW = 0;
    pendingThumbH = 0;
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
