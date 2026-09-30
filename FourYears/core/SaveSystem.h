#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H


#include <string>
#include <vector>
#include <map>


struct SaveData
{
    std::string filename;

    std::string displayName;

    std::string scriptFile;

    std::string chapterName;

    int index;

    std::string time;


    // ==========================================================
    // [新增] 好感度快照
    // ==========================================================
    //
    // 角色名 -> 好感度数值。
    // 读档时用它恢复 RouteManager。

    std::map<std::string, int> affection;

    // ==========================================================
};



class SaveSystem
{

public:


    SaveSystem();



    void Init();



    std::vector<SaveData>
    GetSaveList();



    // [修改] 加 affection
    bool CreateSave(
        const std::string& name,
        const std::string& scriptFile,
        const std::string& chapterName,
        int index,
        const std::map<std::string, int>& affection
    );



    // [修改] 带出 affection
    bool LoadSave(
        const std::string& filename,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& index,
        std::map<std::string, int>& outAffection
    );



    bool DeleteSave(
        const std::string& filename
    );



    bool RenameSave(
        const std::string& filename,
        const std::string& newDisplayName
    );



    bool CopySave(
        const std::string& filename
    );



    bool Empty();



private:


    std::string savePath;



    std::string GetCurrentTime();

};



#endif