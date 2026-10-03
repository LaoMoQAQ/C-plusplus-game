#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H


#include <SDL_ttf.h>

#include <string>
#include <map>



// ==========================================================
// FontManager
// ==========================================================
//
// 字体缓存。按 (路径, 字号) 去重。
//
// 用法：
//   Load(path)                    加载默认字体（32 号）
//   GetFont()                     取默认字体
//   GetFont(size)                 用默认路径加载指定字号
//   GetFont(path, size)           指定路径 + 字号
//
// 标题、副标题、正文用不同字号，靠缓存避免重复加载。

class FontManager
{

public:

    FontManager();
    ~FontManager();



    // 加载主字体（默认 32 号）并记住路径。
    // 之后 GetFont(size) 会用这个路径。
    bool Load(const std::string& path);



    // 获取默认字体（32 号）。
    TTF_Font* GetFont();



    // 用 Load 时传入的路径，加载指定字号。
    TTF_Font* GetFont(int size);



    // 指定路径 + 字号。内部缓存，重复调用不重复加载。
    TTF_Font* GetFont(
        const std::string& path,
        int size
    );



private:

    std::string defaultPath;   // Load 时记下
    TTF_Font*   font;          // 默认 32 号字体



    // 缓存 key：(路径, 字号)
    struct FontKey
    {
        std::string path;
        int size;

        bool operator<(const FontKey& other) const
        {
            if(path != other.path)
                return path < other.path;
            return size < other.size;
        }
    };

    std::map<FontKey, TTF_Font*> cache;

};


#endif