#ifndef CONFIG_H
#define CONFIG_H


#include <string>



// ==========================================================
// Config
// ==========================================================
//
// config.ini 的读写。
//
// 文件结构：
//   [window]    width / height / title / fullscreen
//   [game]      bgm / se / textSpeed / autoPlay
//   [resource]  bg / character / font
//
// 说明：
//   - [window] 和 [game] 段游戏内会读会写
//   - [resource] 段目前只读不写，所有路径实际硬编码在 Game::Init()

class Config
{

public:

    Config();



    bool Load(const std::string& path);
    bool Save(const std::string& path);



    // ---- 窗口 ----

    int         GetWidth()  const;
    int         GetHeight() const;
    std::string GetTitle()  const;

    void SetWidth(int value);
    void SetHeight(int value);
    void SetTitle(const std::string& value);



    // ---- 游戏 ----

    int  GetBGMVolume() const;
    int  GetSEVolume()  const;
    int  GetTextSpeed() const;
    bool IsFullscreen() const;
    bool IsAutoPlay()   const;

    void SetBGMVolume(int value);
    void SetSEVolume(int value);
    void SetTextSpeed(int value);
    void SetFullscreen(bool value);
    void SetAutoPlay(bool value);



    // ---- 资源路径 ----

    std::string GetResourceBg()        const;
    std::string GetResourceCharacter() const;
    std::string GetResourceFont()      const;

    void SetResourceBg(const std::string& value);
    void SetResourceCharacter(const std::string& value);
    void SetResourceFont(const std::string& value);



private:

    // 窗口
    int         width;
    int         height;
    std::string title;

    // 游戏
    int  bgmVolume;
    int  seVolume;
    int  textSpeed;
    bool fullscreen;
    bool autoPlay;

    // 资源路径
    std::string resourceBg;
    std::string resourceCharacter;
    std::string resourceFont;

};


#endif