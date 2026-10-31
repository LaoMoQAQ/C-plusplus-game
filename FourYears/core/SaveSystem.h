#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H


#include <string>
#include <vector>
#include <map>

#include <SDL.h>


#include "../story/RouteManager.h"



// ==========================================================
// 存档槽位
// ==========================================================
//
// 固定 10 格，文件名 slot1.dat ~ slot10.dat。
// 空槽位没有对应文件。

constexpr int SAVE_SLOT_COUNT = 10;



// ==========================================================
// SaveData
// ==========================================================
//
// 一份存档的完整快照。
//
// 存档文件格式（7 行纯文本，UTF-8）：
//   1. displayName     显示名
//   2. scriptFile      脚本路径
//   3. chapterName     章节显示名
//   4. index           事件下标
//   5. time            保存时间
//   6. affection       好感度（李君浩:10,张瀚宇:15）
//   7. records         好感度变化历史（可选）
//
// 第 7 行是后加的。旧存档只有 6 行，读的时候 getline 失败就当作
// 空历史，不影响读档。
//
// 缩略图不写进这 7 行，而是单独存成 slotN.png 放在同一目录。

struct SaveData
{
    std::string filename;       // slotN.dat
    std::string displayName;
    std::string scriptFile;
    std::string chapterName;

    // 必须初始化：ReadSaveFile 里 in >> index 失败时该值是不确定的，
    // 读档会跳到一个随机位置。
    int index = 0;

    std::string time;
    std::map<std::string, int> affection;

    // 好感度变化历史。旧存档没有，为空。
    std::vector<AffectionRecord> records;

    // 槽位号 1~10。0 表示无效/未使用。
    int slot = 0;

    // 缩略图路径（相对 save/），空表示没有
    std::string thumbnail;

    // 槽位是否已被占用
    bool used = false;
};



// ==========================================================
// SaveSystem
// ==========================================================
//
// 存档读写。管理 save/ 目录下固定 10 个槽位。
//
//   save/slot1.dat   ~ save/slot10.dat    存档正文
//   save/slot1.png   ~ save/slot10.png    缩略图（可选）
//
// 空槽位没有 dat 文件。0 字节的 dat 视为无效。

class SaveSystem
{

public:

    SaveSystem();

    void Init();


    // 返回固定 SAVE_SLOT_COUNT 条记录，下标 0 对应 slot1。
    // 未使用的槽位 used == false。
    std::vector<SaveData> GetSaveList();

    // 读单个槽位。空槽位返回 false。
    bool ReadSlot(int slot, SaveData& out);

    // 覆盖写入指定槽位。slot 越界返回 false。
    // thumbnail 为 RGBA32 像素数据，w*h*4 字节；传 nullptr 表示不存图。
    bool CreateSave(
        int slot,
        const std::string& name,
        const std::string& scriptFile,
        const std::string& chapterName,
        int index,
        const std::map<std::string, int>& affection,
        const std::vector<AffectionRecord>& records,
        const void* thumbnailPixels = nullptr,
        int thumbnailW = 0,
        int thumbnailH = 0
    );

    bool LoadSave(
        int slot,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& index,
        std::map<std::string, int>& outAffection,
        std::vector<AffectionRecord>& outRecords
    );

    // 清空槽位（删 dat 和 png）。
    bool DeleteSave(int slot);

    bool RenameSave(int slot, const std::string& newDisplayName);

    // 把一个槽位复制到另一个槽位（含缩略图）。
    bool CopySave(int fromSlot, int toSlot);

    bool Empty();


    // 槽位 -> 文件路径
    std::string SlotPath(int slot)      const;
    std::string ThumbPath(int slot)     const;

    // 槽位 -> 缩略图纹理（懒加载 + 缓存）。
    // 没有缩略图返回 nullptr。
    SDL_Texture* GetThumbnail(
        SDL_Renderer* renderer,
        int slot
    );

    // 丢弃缩略图纹理缓存（槽位内容变化后调用）
    void InvalidateThumbnail(int slot);
    void ClearThumbnails();



private:

    std::string savePath;

    std::string GetCurrentTime();

    bool ReadSaveFile(
        const std::string& path,
        SaveData& out
    );

    // 把旧版 save_<时间戳>.dat 迁移进空槽位（启动时跑一次）
    void MigrateLegacySaves();

    // 保存缩略图 PNG
    bool WriteThumbnail(
        int slot,
        const void* pixels,
        int w,
        int h
    );



    // 槽位 -> 已加载的缩略图纹理
    struct ThumbEntry
    {
        SDL_Texture* texture = nullptr;
        bool tried = false;     // 已经尝试过加载（失败也记，避免反复 IO）
    };

    std::map<int, ThumbEntry> thumbCache;

};


#endif
