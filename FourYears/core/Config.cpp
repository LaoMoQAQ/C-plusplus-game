#include "Config.h"

#include <fstream>
#include <sstream>


// ==========================================================
// 构造
// ==========================================================
//
// 默认值与 config.ini 保持一致（1600x900）。
// 如果用户删了 config.ini，游戏按这个尺寸启动。

Config::Config()
{
    width  = 1600;
    height = 900;
    title  = "Four Years";

    bgmVolume = 64;
    seVolume  = 64;
    textSpeed = 20;

    fullscreen = false;
    autoPlay   = false;

    resourceBg        = "resource/bg/";
    resourceCharacter = "resource/character/";
    resourceFont      = "resource/font/simhei.ttf";
}



// ==========================================================
// Load / Save
// ==========================================================

bool Config::Load(const std::string& path)
{
    std::ifstream file(path);

    if(!file.is_open())
        return false;

    std::string line;

    while(std::getline(file, line))
    {
        if(line.empty())
            continue;

        if(line[0] == '[')
            continue;

        size_t pos = line.find('=');

        if(pos == std::string::npos)
            continue;

        std::string key   = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if(key == "width")          width  = std::stoi(value);
        else if(key == "height")    height = std::stoi(value);
        else if(key == "title")     title  = value;
        else if(key == "bgm")       bgmVolume = std::stoi(value);
        else if(key == "se")        seVolume  = std::stoi(value);
        else if(key == "textSpeed") textSpeed = std::stoi(value);
        else if(key == "fullscreen") fullscreen = (value == "1" || value == "true");
        else if(key == "autoPlay")   autoPlay   = (value == "1" || value == "true");
        else if(key == "bg")        resourceBg        = value;
        else if(key == "character") resourceCharacter = value;
        else if(key == "font")      resourceFont      = value;
    }

    file.close();

    return true;
}



bool Config::Save(const std::string& path)
{
    std::ofstream file(path);

    if(!file.is_open())
        return false;

    file << "[window]\n";
    file << "width="  << width  << "\n";
    file << "height=" << height << "\n";
    file << "title="  << title  << "\n";
    file << "fullscreen=" << (fullscreen ? 1 : 0) << "\n\n";

    file << "[game]\n";
    file << "bgm="       << bgmVolume << "\n";
    file << "se="        << seVolume  << "\n";
    file << "textSpeed=" << textSpeed << "\n";
    file << "autoPlay="  << (autoPlay ? 1 : 0) << "\n\n";

    file << "[resource]\n";
    file << "bg="        << resourceBg        << "\n";
    file << "character=" << resourceCharacter << "\n";
    file << "font="      << resourceFont      << "\n";

    file.close();

    return true;
}



// ==========================================================
// 窗口
// ==========================================================

int         Config::GetWidth()  const { return width;  }
int         Config::GetHeight() const { return height; }
std::string Config::GetTitle()  const { return title;  }

void Config::SetWidth(int value)  { width  = value; }
void Config::SetHeight(int value) { height = value; }

void Config::SetTitle(const std::string& value)
{
    title = value;
}



// ==========================================================
// 游戏
// ==========================================================

int  Config::GetBGMVolume() const { return bgmVolume; }
int  Config::GetSEVolume()  const { return seVolume;  }
int  Config::GetTextSpeed() const { return textSpeed; }

bool Config::IsFullscreen() const { return fullscreen; }
bool Config::IsAutoPlay()   const { return autoPlay;   }

void Config::SetBGMVolume(int value)   { bgmVolume = value; }
void Config::SetSEVolume(int value)    { seVolume  = value; }
void Config::SetTextSpeed(int value)   { textSpeed = value; }
void Config::SetFullscreen(bool value) { fullscreen = value; }
void Config::SetAutoPlay(bool value)   { autoPlay   = value; }



// ==========================================================
// 资源路径
// ==========================================================

std::string Config::GetResourceBg()        const { return resourceBg;        }
std::string Config::GetResourceCharacter() const { return resourceCharacter; }
std::string Config::GetResourceFont()      const { return resourceFont;      }

void Config::SetResourceBg(const std::string& value)        { resourceBg = value;        }
void Config::SetResourceCharacter(const std::string& value) { resourceCharacter = value; }
void Config::SetResourceFont(const std::string& value)      { resourceFont = value;      }