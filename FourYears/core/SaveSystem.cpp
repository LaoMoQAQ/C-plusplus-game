#include "SaveSystem.h"


#include <fstream>
#include <iostream>



SaveSystem::SaveSystem()
{


}






std::string SaveSystem::GetPath(
    int slot
)
{

    return 
    "save/slot"
    + std::to_string(slot)
    + ".dat";

}







bool SaveSystem::Save(

    int slot,

    const SaveData& data

)
{


    std::ofstream file(
        GetPath(slot)
    );



    if(!file.is_open())
    {

        return false;

    }



    file
    << data.chapter
    << "\n";



    file
    << data.scriptIndex
    << "\n";



    file
    << data.background
    << "\n";



    file
    << data.speaker
    << "\n";



    file
    << data.text
    << "\n";



    file
    << data.affection.size()
    << "\n";



    for(auto& item:data.affection)
    {


        file
        << item.first
        << " "
        << item.second
        << "\n";


    }



    file.close();



    return true;


}








bool SaveSystem::Load(

    int slot,

    SaveData& data

)
{


    std::ifstream file(
        GetPath(slot)
    );



    if(!file.is_open())
    {

        return false;

    }



    file
    >> data.chapter;



    file
    >> data.scriptIndex;



    file.ignore();



    std::getline(
        file,
        data.background
    );


    std::getline(
        file,
        data.speaker
    );


    std::getline(
        file,
        data.text
    );



    int count;



    file
    >> count;



    data.affection.clear();



    for(int i=0;i<count;i++)
    {

        std::string name;

        int value;


        file
        >> name
        >> value;



        data.affection[name]=value;


    }



    file.close();



    return true;


}







bool SaveSystem::Exists(
    int slot
)
{


    std::ifstream file(
        GetPath(slot)
    );


    return file.good();


}