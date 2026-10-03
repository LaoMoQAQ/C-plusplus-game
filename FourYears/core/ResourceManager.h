#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H


#include <SDL.h>
#include <SDL_image.h>

#include <string>
#include <unordered_map>



// ==========================================================
// ResourceManager
// ==========================================================
//
// 纹理缓存。按路径去重，同一张图只加载一次。
//
// 主要用法：
//   LoadTexture(renderer, path)  加载（已缓存直接返回）
//   GetTexture(path)             查询（没缓存返回 nullptr）
//
// 生命周期：整个游戏期间常驻。Destroy 在析构时自动调用。

class ResourceManager
{

public:

    ResourceManager();
    ~ResourceManager();



    // 初始化 SDL_image（PNG 支持）。
    bool Init();



    // 加载纹理。已缓存则直接返回缓存。
    // 加载失败返回 nullptr。
    SDL_Texture* LoadTexture(
        SDL_Renderer* renderer,
        const std::string& path
    );



    // 只查询，不加载。
    // 找不到返回 nullptr。
    SDL_Texture* GetTexture(
        const std::string& path
    );



    // 释放所有纹理。
    void Destroy();



private:

    // 路径 -> 纹理
    std::unordered_map<
        std::string,
        SDL_Texture*
    > textures;

};


#endif