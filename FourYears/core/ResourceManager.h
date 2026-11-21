#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H


#include <SDL.h>
#include <SDL_image.h>

#include <string>
#include <unordered_map>


class Renderer;



// ==========================================================
// ResourceManager
// ==========================================================
//
// 纹理缓存。按路径去重，同一张图只加载一次。
//
// GL 移植后不再依赖 SDL_Renderer：
//   图片用 IMG_Load 读成 Surface，再交给 Renderer 上传到 GL。
//   LoadTexture 的第一个参数保留是为了兼容旧调用点，
//   现在忽略它，真正用的是 SetRenderer 绑定的那个。

class ResourceManager
{

public:

    ResourceManager();
    ~ResourceManager();



    // 初始化 SDL_image（PNG 支持）。
    bool Init();



    // 绑定渲染器。必须在上传纹理之前调用。
    void SetRenderer(Renderer* r) { targetRenderer = r; }



    // 加载纹理。已缓存则直接返回缓存。
    // 加载失败返回 nullptr。
    //
    // renderer 参数已废弃，保留只为兼容既有调用点。
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

    // 上传纹理用。由 Game 在初始化时绑定。
    Renderer* targetRenderer = nullptr;

    // IMG_Init 是否成功过。Destroy 用它防止重复 IMG_Quit。
    bool imageInited = false;

};


#endif
