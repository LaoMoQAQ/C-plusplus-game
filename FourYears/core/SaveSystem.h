#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H


#include <string>
#include <vector>


struct SaveData
{
    std::string filename;

    std::string displayName;

    // 章节脚本路径，如 "script/chapter01.txt"
    std::string scriptFile;

    // [修改] 从 int 改成 string。
    // 直接存显示名："第1章" / "结局" / "李君浩线" 等，
    // 不再靠数字硬编码。
    std::string chapterName;

    int index;

    std::string time;
};



class SaveSystem
{

public:


    SaveSystem();



    void Init();



    std::vector<SaveData>
    GetSaveList();



    // [修改] chapter -> chapterName
    bool CreateSave(
        const std::string& name,
        const std::string& scriptFile,
        const std::string& chapterName,
        int index
    );



    // [修改] 通过引用带出 chapterName
    bool LoadSave(
        const std::string& filename,
        std::string& outScriptFile,
        std::string& outChapterName,
        int& index
    );



    bool DeleteSave(
        const std::string& filename
    );



    // [修改] 现在改的是"显示名"（文件第一行），
    // 不再改文件名。
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