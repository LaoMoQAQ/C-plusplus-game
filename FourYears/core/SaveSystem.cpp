#include "SaveSystem.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>


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




    struct Entry
    {
        SaveData data;
        fs::file_time_type writeTime;
    };


    std::vector<Entry> entries;




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





        {
            std::error_code ec;

            auto size =
                fs::file_size(
                    file.path(),
                    ec
                );


            if(ec || size == 0)
            {
                continue;
            }
        }





        Entry entry;



        entry.data.filename =
            file.path().filename().string();




        {
            std::error_code ec;

            entry.writeTime =
                fs::last_write_time(
                    file.path(),
                    ec
                );


            if(ec)
            {
                entry.writeTime =
                    fs::file_time_type::min();
            }
        }




        std::ifstream in(
            file.path()
        );



        if(in)
        {

            // 存档格式（5 行）：
            //   1. displayName
            //   2. scriptFile
            //   3. chapterName
            //   4. index
            //   5. time
            std::getline(
                in,
                entry.data.displayName
            );

            std::getline(
                in,
                entry.data.scriptFile
            );

            std::getline(
                in,
                entry.data.chapterName
            );


            in
            >>
            entry.data.index;


            in.ignore();


            std::getline(
                in,
                entry.data.time
            );

        }


        else
        {

            entry.data.displayName="未知存档";

            entry.data.scriptFile="";

            entry.data.chapterName="";

            entry.data.index=0;

            entry.data.time="";

        }



        entries.push_back(
            entry
        );

    }




    std::sort(
        entries.begin(),
        entries.end(),
        [](const Entry& a, const Entry& b)
        {
            return a.writeTime > b.writeTime;
        }
    );




    list.reserve(entries.size());

    for(auto& e : entries)
    {
        list.push_back(e.data);
    }




    return list;

}








bool SaveSystem::CreateSave(
    const std::string& name,
    const std::string& scriptFile,
    const std::string& chapterName,
    int index
)
{

    auto now =
        std::chrono::system_clock::now();

    auto ms =
        std::chrono::duration_cast<
            std::chrono::milliseconds
        >(
            now.time_since_epoch()
        ).count();


    std::string filename =
        savePath
        + "save_"
        + std::to_string(ms)
        + ".dat";


    std::ofstream out(filename);

    if(!out)
    {
        return false;
    }


    out << name        << "\n";
    out << scriptFile  << "\n";
    out << chapterName << "\n";
    out << index       << "\n";
    out << GetCurrentTime() << "\n";

    out.close();


    return true;

}









bool SaveSystem::LoadSave(
    const std::string& filename,
    std::string& outScriptFile,
    std::string& outChapterName,
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




    std::string displayName;

    std::string scriptFile;

    std::string chapterName;

    std::string time;



    std::getline(in, displayName);
    std::getline(in, scriptFile);
    std::getline(in, chapterName);


    in >> index;



    outScriptFile = scriptFile;
    outChapterName = chapterName;


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









// [修改] 现在改的是"显示名"（文件第一行），
// 不再改文件名。
bool SaveSystem::RenameSave(
    const std::string& filename,
    const std::string& newDisplayName
)
{

    fs::path path =
        savePath+filename;



    if(!fs::exists(path))
    {
        return false;
    }



    // 读出原有内容
    std::ifstream in(path);

    if(!in)
    {
        return false;
    }


    std::string displayName;
    std::string scriptFile;
    std::string chapterName;
    int index = 0;
    std::string time;


    std::getline(in, displayName);
    std::getline(in, scriptFile);
    std::getline(in, chapterName);
    in >> index;
    in.ignore();
    std::getline(in, time);

    in.close();



    // 覆盖写回，只改第一行
    std::ofstream out(path);

    if(!out)
    {
        return false;
    }


    out << newDisplayName << "\n";
    out << scriptFile     << "\n";
    out << chapterName    << "\n";
    out << index          << "\n";
    out << time           << "\n";

    out.close();


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




    auto now =
        std::chrono::system_clock::now();

    auto ms =
        std::chrono::duration_cast<
            std::chrono::milliseconds
        >(
            now.time_since_epoch()
        ).count();



    std::string newFile =
        savePath
        + "copy_"
        + std::to_string(ms)
        + ".dat";





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