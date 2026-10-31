#include "SaveSystem.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include <SDL_image.h>


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

    MigrateLegacySaves();
}




// ==========================================================
// MigrateLegacySaves
// ==========================================================
//
// 旧版本用 save_<毫秒时间戳>.dat 无限列表，新版改成 slot1~10。
//
// 升级后如果直接不认旧文件，玩家的存档就全没了。
// 这里在启动时把旧文件按写入时间倒序，依次填进空槽位。
// 迁移完把旧文件改名成 .migrated，不删除——万一出问题还能捞回来。
//
// 只跑一次：槽位填满后剩下的旧文件保持 .migrated 不动。

void SaveSystem::MigrateLegacySaves()
{
    if(!fs::exists(savePath))
        return;


    struct Legacy
    {
        fs::path path;
        fs::file_time_type time;
    };

    std::vector<Legacy> legacy;

    std::error_code ec;

    for(auto& entry : fs::directory_iterator(savePath, ec))
    {
        if(ec)
            break;

        if(entry.path().extension() != ".dat")
            continue;

        std::string name = entry.path().filename().string();

        // 只处理旧命名，槽位文件不动
        if(name.rfind("save_", 0) != 0)
            continue;

        Legacy item;
        item.path = entry.path();

        std::error_code tec;
        item.time = fs::last_write_time(entry.path(), tec);

        if(tec)
            item.time = fs::file_time_type::min();

        legacy.push_back(item);
    }

    if(legacy.empty())
        return;


    // 最新的排前面
    std::sort(
        legacy.begin(), legacy.end(),
        [](const Legacy& a, const Legacy& b)
        {
            return a.time > b.time;
        }
    );


    // 找空槽位
    int next = 1;

    for(auto& item : legacy)
    {
        while(next <= SAVE_SLOT_COUNT)
        {
            std::string dst = SlotPath(next);

            std::error_code sec;

            bool occupied =
                fs::exists(dst, sec) &&
                !sec &&
                fs::file_size(dst, sec) > 0 &&
                !sec;

            if(!occupied)
                break;

            next++;
        }

        if(next > SAVE_SLOT_COUNT)
            break;      // 槽位满了，剩下的保持原样

        std::error_code cec;

        fs::rename(item.path, SlotPath(next), cec);

        if(!cec)
        {
            std::cout
                << "迁移旧存档: "
                << item.path.filename().string()
                << " -> slot"
                << next
                << ".dat"
                << std::endl;

            next++;
        }
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




std::string SaveSystem::SlotPath(int slot) const
{
    if(slot < 1 || slot > SAVE_SLOT_COUNT)
        return "";

    return savePath + "slot" + std::to_string(slot) + ".dat";
}




std::string SaveSystem::ThumbPath(int slot) const
{
    if(slot < 1 || slot > SAVE_SLOT_COUNT)
        return "";

    return savePath + "slot" + std::to_string(slot) + ".png";
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
// 好感度历史序列化 / 反序列化
// ==========================================================
//
// 格式：李君浩|10|好，麻烦你了,张瀚宇|-5|随便聊聊
// 字段之间用 | 分隔，记录之间用 , 分隔。
//
// 来源文本里如果有 , 或 | 会破坏格式，写的时候替换成空格。

static std::string SanitizeField(const std::string& s)
{
    std::string out = s;

    for(char& c : out)
    {
        if(c == ',' || c == '|' || c == '\n' || c == '\r')
            c = ' ';
    }

    return out;
}




static std::string SerializeRecords(
    const std::vector<AffectionRecord>& records
)
{
    std::string result;

    for(auto& rec : records)
    {
        if(!result.empty())
            result += ',';

        result += SanitizeField(rec.character);
        result += '|';
        result += std::to_string(rec.delta);
        result += '|';
        result += SanitizeField(rec.source);
    }

    return result;
}




static void DeserializeRecords(
    const std::string& line,
    std::vector<AffectionRecord>& out
)
{
    out.clear();

    if(line.empty())
        return;

    std::stringstream ss(line);
    std::string item;

    while(std::getline(ss, item, ','))
    {
        if(item.empty())
            continue;

        // 拆成 角色|变化量|来源
        size_t p1 = item.find('|');

        if(p1 == std::string::npos)
            continue;

        size_t p2 = item.find('|', p1 + 1);

        AffectionRecord rec;
        rec.character = item.substr(0, p1);

        std::string deltaStr;

        if(p2 == std::string::npos)
        {
            deltaStr = item.substr(p1 + 1);
        }
        else
        {
            deltaStr = item.substr(p1 + 1, p2 - p1 - 1);
            rec.source = item.substr(p2 + 1);
        }

        if(rec.character.empty() || deltaStr.empty())
            continue;

        try
        {
            rec.delta = std::stoi(deltaStr);
        }
        catch(...)
        {
            continue;
        }

        out.push_back(rec);
    }
}




// ==========================================================
// ReadSaveFile
// ==========================================================
//
// 读一个存档文件并解析格式。
// 打开失败返回 false。
//
// 第 7 行（历史）是后加的，旧存档没有这一行，
// getline 会失败，此时 out.records 保持空。

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


    // 第 7 行：历史（旧存档没有）
    std::string recLine;

    if(std::getline(in, recLine))
        DeserializeRecords(recLine, out.records);

    return true;
}




bool SaveSystem::ReadSlot(int slot, SaveData& out)
{
    out = SaveData();
    out.slot = slot;

    std::string path = SlotPath(slot);

    if(path.empty() || !fs::exists(path))
        return false;

    // 跳过 0 字节文件
    {
        std::error_code ec;
        auto size = fs::file_size(path, ec);

        if(ec || size == 0)
            return false;
    }

    out.filename = "slot" + std::to_string(slot) + ".dat";

    if(!ReadSaveFile(path, out))
        return false;

    out.slot = slot;
    out.used = true;

    std::string thumb = ThumbPath(slot);

    if(fs::exists(thumb))
        out.thumbnail = "slot" + std::to_string(slot) + ".png";

    return true;
}




std::vector<SaveData> SaveSystem::GetSaveList()
{
    std::vector<SaveData> list;
    list.reserve(SAVE_SLOT_COUNT);

    for(int i = 1; i <= SAVE_SLOT_COUNT; i++)
    {
        SaveData data;
        data.slot = i;

        ReadSlot(i, data);

        list.push_back(data);
    }

    return list;
}




// ==========================================================
// WriteThumbnail
// ==========================================================
//
// 把 RGBA32 像素写成 PNG。
// 缩略图不是关键数据，失败就静默跳过，不影响存档本身。

bool SaveSystem::WriteThumbnail(
    int slot,
    const void* pixels,
    int w,
    int h
)
{
    if(!pixels || w <= 0 || h <= 0)
        return false;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        const_cast<void*>(pixels),
        w, h,
        32,              // 每像素 32 位
        w * 4,           // 每行字节数
        SDL_PIXELFORMAT_RGBA32
    );

    if(!surface)
        return false;

    std::string path = ThumbPath(slot);

    int rc = IMG_SavePNG(surface, path.c_str());

    SDL_FreeSurface(surface);

    return rc == 0;
}




// ==========================================================
// CreateSave
// ==========================================================

bool SaveSystem::CreateSave(
    int slot,
    const std::string& name,
    const std::string& scriptFile,
    const std::string& chapterName,
    int index,
    const std::map<std::string, int>& affection,
    const std::vector<AffectionRecord>& records,
    const void* thumbnailPixels,
    int thumbnailW,
    int thumbnailH
)
{
    std::string path = SlotPath(slot);

    if(path.empty())
        return false;

    std::ofstream out(path);

    if(!out)
        return false;


    out << name        << "\n";
    out << scriptFile  << "\n";
    out << chapterName << "\n";
    out << index       << "\n";
    out << GetCurrentTime() << "\n";
    out << SerializeAffection(affection) << "\n";
    out << SerializeRecords(records) << "\n";

    out.close();


    // 缩略图（可选）
    if(thumbnailPixels)
        WriteThumbnail(slot, thumbnailPixels, thumbnailW, thumbnailH);


    InvalidateThumbnail(slot);

    return true;
}




bool SaveSystem::LoadSave(
    int slot,
    std::string& outScriptFile,
    std::string& outChapterName,
    int& index,
    std::map<std::string, int>& outAffection,
    std::vector<AffectionRecord>& outRecords
)
{
    SaveData data;

    if(!ReadSlot(slot, data))
        return false;

    outScriptFile  = data.scriptFile;
    outChapterName = data.chapterName;
    index          = data.index;
    outAffection   = data.affection;
    outRecords     = data.records;

    return true;
}




bool SaveSystem::DeleteSave(int slot)
{
    bool removed = false;

    std::string dat = SlotPath(slot);

    if(!dat.empty() && fs::exists(dat))
        removed = fs::remove(dat);

    std::string png = ThumbPath(slot);

    if(!png.empty() && fs::exists(png))
        fs::remove(png);

    InvalidateThumbnail(slot);

    return removed;
}




bool SaveSystem::RenameSave(
    int slot,
    const std::string& newDisplayName
)
{
    SaveData data;

    if(!ReadSlot(slot, data))
        return false;


    std::string path = SlotPath(slot);

    std::ofstream out(path);

    if(!out)
        return false;

    out << newDisplayName   << "\n";
    out << data.scriptFile  << "\n";
    out << data.chapterName << "\n";
    out << data.index       << "\n";
    out << data.time        << "\n";
    out << SerializeAffection(data.affection) << "\n";
    out << SerializeRecords(data.records) << "\n";

    out.close();

    return true;
}




// ==========================================================
// CopySave
// ==========================================================
//
// 直接按字节复制 dat，再复制缩略图。
// 目标槽位原有内容会被覆盖。

bool SaveSystem::CopySave(int fromSlot, int toSlot)
{
    std::string src = SlotPath(fromSlot);
    std::string dst = SlotPath(toSlot);

    if(src.empty() || dst.empty() || fromSlot == toSlot)
        return false;

    if(!fs::exists(src))
        return false;


    std::error_code ec;

    fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);

    if(ec)
        return false;


    // 缩略图
    std::string srcPng = ThumbPath(fromSlot);
    std::string dstPng = ThumbPath(toSlot);

    if(fs::exists(srcPng))
    {
        fs::copy_file(
            srcPng, dstPng,
            fs::copy_options::overwrite_existing, ec
        );
    }
    else if(fs::exists(dstPng))
    {
        // 源没有图，目标不该留着旧图
        fs::remove(dstPng);
    }


    InvalidateThumbnail(toSlot);

    return true;
}




bool SaveSystem::Empty()
{
    for(int i = 1; i <= SAVE_SLOT_COUNT; i++)
    {
        std::string path = SlotPath(i);

        if(!path.empty() && fs::exists(path))
        {
            std::error_code ec;
            auto size = fs::file_size(path, ec);

            if(!ec && size > 0)
                return false;
        }
    }

    return true;
}




// ==========================================================
// 缩略图纹理
// ==========================================================

SDL_Texture* SaveSystem::GetThumbnail(
    SDL_Renderer* renderer,
    int slot
)
{
    if(!renderer)
        return nullptr;

    auto it = thumbCache.find(slot);

    if(it != thumbCache.end() && it->second.tried)
        return it->second.texture;


    ThumbEntry entry;
    entry.tried = true;

    std::string path = ThumbPath(slot);

    if(!path.empty() && fs::exists(path))
    {
        SDL_Surface* surface = IMG_Load(path.c_str());

        if(surface)
        {
            entry.texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
    }

    thumbCache[slot] = entry;

    return entry.texture;
}




void SaveSystem::InvalidateThumbnail(int slot)
{
    auto it = thumbCache.find(slot);

    if(it == thumbCache.end())
        return;

    if(it->second.texture)
        SDL_DestroyTexture(it->second.texture);

    thumbCache.erase(it);
}




void SaveSystem::ClearThumbnails()
{
    for(auto& pair : thumbCache)
    {
        if(pair.second.texture)
            SDL_DestroyTexture(pair.second.texture);
    }

    thumbCache.clear();
}
