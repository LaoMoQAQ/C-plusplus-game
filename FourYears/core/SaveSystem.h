#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H


#include <string>
#include <vector>


struct SaveData
{
    std::string filename;

    std::string displayName;

    int chapter;

    int index;

    std::string time;
};



class SaveSystem
{

public:


    SaveSystem();



    // 初始化存档目录

    void Init();



    // 获取所有存档

    std::vector<SaveData>
    GetSaveList();



    // 创建存档

    bool CreateSave(
        const std::string& name,
        int chapter,
        int index
    );



    // 读取存档

    bool LoadSave(
        const std::string& filename,
        int& chapter,
        int& index
    );



    // 删除存档

    bool DeleteSave(
        const std::string& filename
    );



    // 重命名

    bool RenameSave(
        const std::string& filename,
        const std::string& newName
    );



    // 复制存档

    bool CopySave(
        const std::string& filename
    );



    // 判断是否为空

    bool Empty();



private:


    std::string savePath;



    std::string GetCurrentTime();


};



#endif