#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H


#include <SDL_ttf.h>

#include <string>
#include <map>


class FontManager
{


public:


    FontManager();

    ~FontManager();



    // 加载主字体（默认 32 号）
    bool Load(
        const std::string& path
    );



    // 获取默认字体（32 号）
    TTF_Font* GetFont();



    // [新增] 按字号获取字体，
    // 使用 Load() 时传入的路径。
    TTF_Font* GetFont(
        int size
    );



    // [新增] 按路径 + 字号获取字体，
    // 内部缓存，重复调用不会重复加载。
    TTF_Font* GetFont(
        const std::string& path,
        int size
    );



private:


    // Load() 时记下的默认字体路径
    std::string defaultPath;



    // 默认 32 号字体（保持原接口）
    TTF_Font* font;



    // [新增] 缓存：(路径, 字号) -> TTF_Font*
    struct FontKey
    {
        std::string path;
        int size;

        bool operator<(
            const FontKey& other
        ) const
        {
            if(path != other.path)
            {
                return path < other.path;
            }
            return size < other.size;
        }
    };

    std::map<FontKey, TTF_Font*> cache;


};


#endif