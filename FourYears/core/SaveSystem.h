#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H


#include <string>
#include <vector>
#include <map>



// ==========================================================
// SaveData
// ==========================================================
//
// 一份存档的完整快照。
// 存档文件格式（6 行，UTF-8）：
//   1. displayName     显示名
//   2. scriptFile      脚本路径
//   3. chapterName     章节显示名
//   4. index           事件下标
//   5. time            保存时间
//   6. affection       好感度（李君浩:10,张瀚宇:15）

struct SaveData
{
    std::string filename;
    std::string displayName;
    std::string scriptFile;
    std::string chapterName;
    int index;
    std::string time;
    std::map<std::string, int> affection;
};



// ==========================================================
// SaveSystem
// ==========================================================
//
// 存档读写。管理 save/ 目录下的 .dat 文件。
//
// 文件名格式：save_<毫秒时间戳>.dat
// 列表按文件写入时间倒序。
// 空文件（0 字节）视为无效，跳过。

class SaveSystem
{

public:

    SaveSystem();

    void Init();

    std::vector<SaveData> GetSaveList();

    bool CreateSave(
        const std::string& name,
        const std::string& scriptFile,
        const std::string& chapterName,
        int index,
        const std::map<std::string, int>& affection
    );

    bool LoadSave(
        const std::string& filename,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& index,
        std::map<std::string, int>& outAffection
    );

    bool DeleteSave(const std::string& filename);

    bool RenameSave(
        const std::string& filename,
        const std::string& newDisplayName
    );

    bool CopySave(const std::string& filename);

    bool Empty();



private:

    std::string savePath;

    std::string GetCurrentTime();



    // ==========================================================
    // [新增] 读一个存档文件
    // ==========================================================
    //
    // 从磁盘读取并解析 6 行格式。
    // 读失败（文件打不开）返回 false，out 保持默认值。
    //
    // GetSaveList 和 LoadSave 共用，避免解析逻辑重复。

    bool ReadSaveFile(
        const std::string& path,
        SaveData& out
    );

};


#endif