#include "SaveSystem.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>


namespace fs = std::filesystem;



SaveSystem::SaveSystem()
{

    savePath = "save/";

}




void SaveSystem::Init()
{

    if(!fs::exists(savePath))
    {
        fs::create_directory(savePath);
    }

}







std::string SaveSystem::GetCurrentTime()
{

    auto now =
    std::chrono::system_clock::now();


    std::time_t t =
    std::chrono::system_clock::to_time_t(
        now
    );


    std::tm tm;


#ifdef _WIN32

    localtime_s(
        &tm,
        &t
    );

#else

    localtime_r(
        &t,
        &tm
    );

#endif



    std::stringstream ss;


    ss
    << std::put_time(
        &tm,
        "%Y-%m-%d %H:%M:%S"
    );


    return ss.str();

}









std::vector<SaveData>
SaveSystem::GetSaveList()
{

    std::vector<SaveData> list;



    if(!fs::exists(savePath))
    {
        return list;
    }




    for(
        auto& file :
        fs::directory_iterator(savePath)
    )
    {


        if(
            file.path().extension()
            !=
            ".dat"
        )
        {
            continue;
        }



        SaveData data;



        data.filename =
            file.path().filename().string();



        std::ifstream in(
            file.path()
        );



        if(in)
        {

            std::getline(
                in,
                data.displayName
            );


            in
            >>
            data.chapter;


            in
            >>
            data.index;


            in.ignore();


            std::getline(
                in,
                data.time
            );

        }


        else
        {

            data.displayName="未知存档";

            data.chapter=0;

            data.index=0;

            data.time="";

        }



        list.push_back(
            data
        );


    }




    return list;

}









bool SaveSystem::CreateSave(
    const std::string& name,
    int chapter,
    int index
)
{


    std::string filename;


    filename =
    savePath
    +
    "save_"
    +
    std::to_string(
        std::time(nullptr)
    )
    +
    ".dat";




    std::ofstream out(
        filename
    );


    if(!out)
    {
        return false;
    }



    out
    << name
    << "\n";



    out
    << chapter
    << "\n";



    out
    << index
    << "\n";



    out
    << GetCurrentTime()
    << "\n";



    out.close();



    return true;

}









bool SaveSystem::LoadSave(
    const std::string& filename,
    int& chapter,
    int& index
)
{


    std::ifstream in(
        savePath+filename
    );


    if(!in)
    {
        return false;
    }




    std::string name;

    std::string time;



    std::getline(
        in,
        name
    );


    in
    >>
    chapter;


    in
    >>
    index;



    return true;

}









bool SaveSystem::DeleteSave(
    const std::string& filename
)
{


    fs::path path =
        savePath+filename;



    if(!fs::exists(path))
    {
        return false;
    }



    return fs::remove(path);

}









bool SaveSystem::RenameSave(
    const std::string& filename,
    const std::string& newName
)
{


    fs::path oldPath =
        savePath+filename;



    if(!fs::exists(oldPath))
    {
        return false;
    }



    fs::path newPath =
        savePath+newName;



    if(newPath.extension()!=".dat")
    {
        newPath += ".dat";
    }



    fs::rename(
        oldPath,
        newPath
    );


    return true;

}









bool SaveSystem::CopySave(
    const std::string& filename
)
{


    fs::path oldPath =
        savePath+filename;



    if(!fs::exists(oldPath))
    {
        return false;
    }





    std::string newFile =
    savePath
    +
    "copy_"
    +
    std::to_string(
        std::time(nullptr)
    )
    +
    ".dat";





    fs::copy_file(
        oldPath,
        newFile,
        fs::copy_options::overwrite_existing
    );



    return true;

}









bool SaveSystem::Empty()
{

    return GetSaveList().empty();

}