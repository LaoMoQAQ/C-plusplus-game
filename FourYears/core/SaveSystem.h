#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H


#include <string>
#include <map>



struct SaveData
{

    int chapter;


    int scriptIndex;


    std::string background;


    std::string speaker;


    std::string text;



    std::map<std::string,int> affection;


};




class SaveSystem
{

public:


    SaveSystem();



    bool Save(
        int slot,
        const SaveData& data
    );



    bool Load(
        int slot,
        SaveData& data
    );



    bool Exists(
        int slot
    );



private:


    std::string GetPath(
        int slot
    );


};



#endif