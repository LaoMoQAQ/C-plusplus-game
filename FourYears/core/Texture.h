#ifndef TEXTURE_H
#define TEXTURE_H


#include "GL.h"


// ==========================================================
// GPU 纹理
// ==========================================================
//
// 改造前到处传 SDL_Texture*；现在统一换成这个轻量结构。
//
// 故意不写析构函数：纹理的释放时机很关键（必须在 GL 上下文
// 还活着的时候），所以由持有者显式调用
// ResourceManager::Destroy() / Renderer::DeleteTexture()，
// 而不是靠 C++ 对象析构，避免上下文销毁后误删。

struct Texture
{
    GLuint id = 0;
    int width = 0;
    int height = 0;

    bool Valid() const
    {
        return id != 0;
    }
};


#endif
