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
    auto now = std::chrono::system_clock::now();

    std::time_t t =
        std::chrono::system_clock::to_time_t(now);

    std::tm tm;

#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::stringstream ss;

    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return ss.str();
}





// ==========================================================
// 好感度序列化 / 反序列化
// ==========================================================
//
// 格式：李君浩:10,张瀚宇:15
// 空 map -> 空字符串
// 单条解析失败时跳过，不报错。

static std::string SerializeAffection(
    const std::map<std::string, int>& affection
)
{
    std::string result;

    for(auto& pair : affection)
    {
        if(!result.empty())
            result += ',';

        result += pair.first;
        result += ':';
        result += std::to_string(pair.second);
    }

    return result;
}





static void DeserializeAffection(
    const std::string& line,
    std::map<std::string, int>& out
)
{
    out.clear();

    if(line.empty())
        return;

    std::stringstream ss(line);
    std::string item;

    while(std::getline(ss, item, ','))
    {
        size_t colon = item.find(':');

        if(colon == std::string::npos)
            continue;

        std::string name = item.substr(0, colon);
        std::string val  = item.substr(colon + 1);

        if(name.empty() || val.empty())
            continue;

        try
        {
            out[name] = std::stoi(val);
        }
        catch(...)
        {
            // 数字无效，跳过
        }
    }
}





// ==========================================================
// ReadSaveFile
// ==========================================================
//
// 读一个存档文件并解析 6 行格式。
// 打开失败返回 false。

bool SaveSystem::ReadSaveFile(
    const std::string& path,
    SaveData& out
)
{
    std::ifstream in(path);

    if(!in)
        return false;

    std::getline(in, out.displayName);
    std::getline(in, out.scriptFile);
    std::getline(in, out.chapterName);

    in >> out.index;
    in.ignore();

    std::getline(in, out.time);

    std::string affLine;
    std::getline(in, affLine);

    DeserializeAffection(affLine, out.affection);

    return true;
}





std::vector<SaveData>
SaveSystem::GetSaveList()
{
    std::vector<SaveData> list;

    if(!fs::exists(savePath))
        return list;


    // 排序用的临时结构：SaveData + 文件写入时间
    struct Entry
    {
        SaveData data;
        fs::file_time_type writeTime;
    };

    std::vector<Entry> entries;


    for(auto& file : fs::directory_iterator(savePath))
    {
        if(file.path().extension() != ".dat")
            continue;

        // 跳过 0 字节文件
        {
            std::error_code ec;
            auto size = fs::file_size(file.path(), ec);

            if(ec || size == 0)
                continue;
        }


        Entry entry;

        entry.data.filename =
            file.path().filename().string();


        // 记录文件写入时间，供排序使用
        {
            std::error_code ec;

            entry.writeTime =
                fs::last_write_time(file.path(), ec);

            if(ec)
                entry.writeTime = fs::file_time_type::min();
        }


        if(!ReadSaveFile(file.path().string(), entry.data))
        {
            entry.data.displayName = "未知存档";
            entry.data.scriptFile  = "";
            entry.data.chapterName = "";
            entry.data.index       = 0;
            entry.data.time        = "";
            entry.data.affection.clear();
        }


        entries.push_back(entry);
    }


    // 按写入时间倒序（最新在上）
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
        list.push_back(e.data);


    return list;
}





bool SaveSystem::CreateSave(
    const std::string& name,
    const std::string& scriptFile,
    const std::string& chapterName,
    int index,
    const std::map<std::string, int>& affection
)
{
    // 毫秒级时间戳，避免同一秒内多次按 N 冲突
    auto now = std::chrono::system_clock::now();

    auto ms = std::chrono::duration_cast<
        std::chrono::milliseconds
    >(now.time_since_epoch()).count();


    std::string filename =
        savePath + "save_" + std::to_string(ms) + ".dat";


    std::ofstream out(filename);

    if(!out)
        return false;


    out << name        << "\n";
    out << scriptFile  << "\n";
    out << chapterName << "\n";
    out << index       << "\n";
    out << GetCurrentTime() << "\n";
    out << SerializeAffection(affection) << "\n";

    out.close();

    return true;
}





bool SaveSystem::LoadSave(
    const std::string& filename,
    std::string& outScriptFile,
    std::string& outChapterName,
    int& index,
    std::map<std::string, int>& outAffection
)
{
    SaveData data;

    if(!ReadSaveFile(savePath + filename, data))
        return false;

    outScriptFile  = data.scriptFile;
    outChapterName = data.chapterName;
    index          = data.index;
    outAffection   = data.affection;

    return true;
}





bool SaveSystem::DeleteSave(const std::string& filename)
{
    fs::path path = savePath + filename;

    if(!fs::exists(path))
        return false;

    return fs::remove(path);
}





bool SaveSystem::RenameSave(
    const std::string& filename,
    const std::string& newDisplayName
)
{
    fs::path path = savePath + filename;

    if(!fs::exists(path))
        return false;


    // 读旧内容
    SaveData data;

    if(!ReadSaveFile(path.string(), data))
        return false;


    // 覆盖写回，只改第一行
    std::ofstream out(path);

    if(!out)
        return false;

    out << newDisplayName   << "\n";
    out << data.scriptFile  << "\n";
    out << data.chapterName << "\n";
    out << data.index       << "\n";
    out << data.time        << "\n";
    out << SerializeAffection(data.affection) << "\n";

    out.close();

    return true;
}





bool SaveSystem::CopySave(const std::string& filename)
{
    fs::path oldPath = savePath + filename;

    if(!fs::exists(oldPath))
        return false;


    auto now = std::chrono::system_clock::now();

    auto ms = std::chrono::duration_cast<
        std::chrono::milliseconds
    >(now.time_since_epoch()).count();


    std::string newFile =
        savePath + "copy_" + std::to_string(ms) + ".dat";


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