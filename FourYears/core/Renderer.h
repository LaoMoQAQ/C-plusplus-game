#ifndef RENDERER_H
#define RENDERER_H


#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include <deque>
#include <vector>
#include <unordered_map>


#include "GL.h"
#include "Texture.h"


// ==========================================================
// OpenGL 渲染器
// ==========================================================
//
// 改造前用 SDL_Renderer / SDL_Texture；现在整个渲染后端改成
// 直接调用 OpenGL 3.3 Core（用 SDL_GL_GetProcAddress 加载入口点，
// 因此构建依赖不变，仍然只需要 SDL2 系列库）。
//
// 渲染流程（每帧）：
//
//   BeginScene()   → 把背景 / 立绘画进离屏场景缓冲
//   EndScene()     → 生成毛玻璃模糊图，再把场景合成到窗口
//   ...UI 绘制...  → 文字、毛玻璃面板、高亮等直接画到窗口
//   Present()      → SDL_GL_SwapWindow
//
// 坐标一律用"逻辑分辨率"（默认 1600x900，左上角原点）。
// 全屏时按 letterbox 等比缩放居中，和其它分辨率保持一致。

class Renderer
{

public:

    Renderer();

    ~Renderer();


    // 必须早于窗口销毁调用；可重复调用。
    void Destroy();


    bool Init(SDL_Window* window);


    // ---------------- 逻辑分辨率 ----------------

    void SetLogicalSize(int w, int h);

    int GetLogicalWidth() const;

    int GetLogicalHeight() const;


    // 当前窗口相对逻辑分辨率的缩放（1.0 = 正好 1600x900）
    float GetContentScale() const;


    // 文字纹理是按 size*scale 光栅化的，绘制时要按 1/scale 缩回去
    void SetTextScale(float scale);


    // 玻璃风格：false = 朴素暗色 + 模糊，true = 液态玻璃。
    //
    // 液态玻璃 = 用圆角矩形的 SDF 梯度当位移向量场，对外圈一条
    // 折射带做「位移 + 放大」，R/G/B 三个通道的位移量各差一点点
    // 形成色散；折射带采样的仍是模糊图，所以折射区是磨砂的。
    // 面板中间保持原来的暗色 + 模糊，文字清晰度不受影响。
    void SetLiquidGlass(bool on);

    bool IsLiquidGlass() const;


    // ---------------- 帧流程 ----------------

    void BeginScene();

    // frostedBackdrop = true 时，整屏铺一层强模糊背景（菜单用）。
    void EndScene(bool frostedBackdrop);

    void Present();


    // ---------------- 图元 ----------------

    void DrawTexture(SDL_Texture* texture, int x, int y);

    void DrawTexture(SDL_Texture* texture, int x, int y, int w, int h);

    // 支持透明度 / 水平翻转 / 颜色染色。
    void DrawTextureEx(
        SDL_Texture* texture,
        float x,
        float y,
        float w,
        float h,
        float alpha = 1.0f,
        bool flipX = false,
        Uint8 tintR = 255,
        Uint8 tintG = 255,
        Uint8 tintB = 255
    );


    void DrawFilledRect(
        int x,
        int y,
        int w,
        int h,
        Uint8 r,
        Uint8 g,
        Uint8 b,
        Uint8 a
    );


    // 圆角矩形（选项高亮条等）。
    void DrawRoundedRect(
        float x,
        float y,
        float w,
        float h,
        float radius,
        Uint8 r,
        Uint8 g,
        Uint8 b,
        Uint8 a
    );


    // ==========================================================
    // 毛玻璃面板
    // ==========================================================
    //
    // 采样 EndScene() 生成的场景模糊图，叠加：
    //   - 圆角 + 抗锯齿
    //   - 冷色调玻璃染色
    //   - 内侧高光描边
    //   - 顶部渐变光泽
    //   - 细微噪点（磨砂质感）
    //   - 柔和投影
    //
    // tintAlpha 越低越透；opacity 控制整体不透明度。
    void DrawGlassPanel(
        float x,
        float y,
        float w,
        float h,
        float radius,
        float tintR,
        float tintG,
        float tintB,
        float tintAlpha,
        float opacity,
        float shadow
    );


    // ---------------- 文字 ----------------

    void SetFont(TTF_Font* font);

    void ClearTextCache();


    // 内容整体偏移 / 透明度（菜单弹出时用来做整体上滑 + 淡入）。
    // 只影响设置之后绘制的图元，绘制完成后请还原为 (0,0,1)。
    void SetContentOffset(float dx, float dy);

    void SetContentAlpha(float alpha);


    // 逻辑坐标裁剪（存档名太长要滚动，滚出格子的部分要切掉）
    void PushClip(float x, float y, float w, float h);

    void PopClip();


    void DrawText(
        const std::string& text,
        int x,
        int y
    );


    // 用默认字体 + 指定颜色
    void DrawText(
        const std::string& text,
        int x,
        int y,
        SDL_Color color
    );


    void DrawText(
        const std::string& text,
        int x,
        int y,
        TTF_Font* font,
        SDL_Color color
    );


    // 量一段文字在默认字体下的逻辑尺寸（已经折算过 textScale，
    // 可以直接拿来做名字牌这类背景框）。
    bool MeasureText(
        const std::string& text,
        int& w,
        int& h
    ) const;


    // 指定字体的版本（量标题用大字号）
    bool MeasureText(
        const std::string& text,
        TTF_Font* font,
        int& w,
        int& h
    ) const;


    // 量"墨迹"：字形真正占用的范围，以及它在绘制矩形里的偏移。
    // 要做一个四边留白一样大的框，必须用这个而不是 MeasureText
    // —— 后者带上了字体的升部 / 降部空白，左边距和上边距会对不齐。
    bool MeasureTextInk(
        const std::string& text,
        SDL_Color color,
        int& inkW,
        int& inkH,
        int& offX,
        int& offY
    );


    // ---------------- 调试 / 自检 ----------------

    // 把当前窗口内容存成 BMP（开发期截图用，不依赖任何图片库）。
    bool SaveScreenshot(const std::string& path);


    int GetDrawCalls() const;

    // 垂直同步是否生效。没有 vsync 时调用方需要自己做帧率限制。
    bool IsVSyncEnabled() const;



    // ==========================================================
    // 兼容接口（本项目既有 UI 代码使用）
    // ==========================================================
    //
    // 下面这些是为了让 FourYears/ui/*.cpp 一行都不用改。
    // 内部全部转调上面的 GL 接口。
    //
    // 纹理句柄用 SDL_Texture* 假指针，真实 GL id 存在 glTextures 表。
    // 加载图片请走 CreateTextureFromSurface / ResourceManager。

    void Shutdown();

    void Clear();

    // 圆角矩形（GL 原生叫 DrawRoundedRect）
    void DrawFilledRoundRect(
        int x, int y, int w, int h,
        int radius,
        Uint8 r, Uint8 g, Uint8 b, Uint8 a
    );

    // 背景按 cover 铺满
    void DrawBackgroundCover(SDL_Texture* texture);

    // 整屏模糊（GL 下由 EndScene 统一生成）
    void DrawBlurTexture(SDL_Texture* texture, int x, int y);

    // 毛玻璃 = 模糊 + 白色磨砂
    void DrawFrostedTexture(
        SDL_Texture* texture,
        int x, int y,
        Uint8 overlayAlpha
    );

    // 圆角区域毛玻璃（对话框背景）
    void DrawFrostedRegionRound(
        SDL_Texture* texture,
        int x, int y, int w, int h,
        int radius,
        Uint8 overlayAlpha
    );

    void DrawBlurredRegion(
        SDL_Texture* texture,
        int x, int y, int w, int h
    );

    // 纹理管理
    SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);
    void DestroyTexture(SDL_Texture* texture);
    bool GetTextureSize(SDL_Texture* texture, int& w, int& h);

    // GL 下没有 SDL_Renderer，返回 nullptr。调用方必须判空。
    SDL_Renderer* GetSDLRenderer();

    // 场景纹理假句柄，供对话框背景模糊使用
    SDL_Texture* GetScreenTexture();

    void CaptureScreen();
    void RestoreMainTarget();


private:

    // 纹理表：SDL_Texture* 假句柄 -> GL 纹理
    struct TextureEntry
    {
        GLuint id = 0;
        int width = 0;
        int height = 0;
    };

    std::unordered_map<SDL_Texture*, TextureEntry> glTextures;

    SDL_Texture* nextHandle = (SDL_Texture*)(uintptr_t)0x1000;

private:

    struct Vertex
    {
        float x;
        float y;
        float u;
        float v;
        float r;
        float g;
        float b;
        float a;
    };


    struct TextCacheEntry
    {
        GLuint texture = 0;
        int w = 0;
        int h = 0;

        // 墨迹在纹理里的位置（像素）
        int inkX = 0;
        int inkY = 0;
        int inkW = 0;
        int inkH = 0;
    };


    // ---- 内部工具 ----

    GLuint CompileShader(GLenum type, const char* source);

    GLuint LinkProgram(const char* vs, const char* fs, const char* name);

    bool CreatePrograms();

    void DestroyPrograms();

    void UpdateDrawableSize();

    // cover = true 时用"铺满"（放大到填满，超出部分裁掉，不留黑边）；
    // cover = false 时用"放下"（等比缩放到完整可见，可能留边）。
    void SetTargetTransform(
        int fbW,
        int fbH,
        bool letterbox,
        bool cover = false
    );

    void PushQuad(
        GLuint texture,
        float x,
        float y,
        float w,
        float h,
        float u0,
        float v0,
        float u1,
        float v1,
        float r,
        float g,
        float b,
        float a
    );

    void Flush();

    void UseSprite();

    void EnsureSceneTargets(int w, int h);

    void EnsureBlurTargets(int w, int h);

    // 一趟可分离高斯（横向或纵向）。src / dst 是 blurTex / blurFbo 的下标。
    void BlurAxis(int src, int dst, float dirX, float dirY);

    void GenerateSceneBlur();

    const TextCacheEntry* GetOrCreateText(
        const std::string& text,
        TTF_Font* font,
        SDL_Color color
    );

    void DrawShape(
        float x,
        float y,
        float w,
        float h,
        float radius,
        float tintR,
        float tintG,
        float tintB,
        float tintAlpha,
        float opacity,
        float shadow,
        bool glassy
    );


private:

    SDL_Window* window = nullptr;

    SDL_GLContext glContext = nullptr;

    bool ready = false;


    int logicalW = 1600;
    int logicalH = 900;

    int drawableW = 1600;
    int drawableH = 900;

    bool vsyncEnabled = false;

    bool liquidGlass = false;

    float contentScale = 1.0f;

    float textScale = 1.0f;


    // ---- 着色器 ----

    GLuint progSprite = 0;
    GLuint progBlur   = 0;
    GLuint progGlass  = 0;

    GLuint activeProgram = 0;


    GLint uSpriteXform = -1;
    GLint uSpriteTex   = -1;

    GLint uBlurXform   = -1;
    GLint uBlurTex     = -1;
    GLint uBlurTexel   = -1;
    GLint uBlurDir     = -1;
    GLint uBlurSigma   = -1;

    GLint uGlassXform   = -1;
    GLint uGlassTex     = -1;
    GLint uGlassLogical = -1;
    GLint uGlassMin     = -1;
    GLint uGlassSize    = -1;
    GLint uGlassRadius  = -1;
    GLint uGlassTint    = -1;
    GLint uGlassOpacity = -1;
    GLint uGlassShadow  = -1;
    GLint uGlassGlassy  = -1;
    GLint uGlassLiquid  = -1;
    GLint uGlassSharp   = -1;
    GLint uGlassBackdrop = -1;
    GLint uGlassShade    = -1;


    // 液态玻璃的压暗系数：面板里显示的就是「背景高斯模糊 × 这个值」，
    // 纯乘法，不掺半透明暗色。1.0 = 与背板同亮，越小越暗。
    float glassShade = 0.65f;


    // EndScene() 铺背板时用的压暗系数。
    // 液态玻璃的折射要跟着一起压暗，否则边缘会比周围亮出一圈高光。
    float backdropShadeR = 1.0f;
    float backdropShadeG = 1.0f;
    float backdropShadeB = 1.0f;


    // ---- 动态四边形 ----

    GLuint vao = 0;
    GLuint vbo = 0;

    std::vector<Vertex> vertices;

    GLuint batchTexture = 0;

    int drawCalls = 0;


    // 当前渲染目标的坐标变换 (scaleX, scaleY, offsetX, offsetY)
    float xform[4] = { 2.0f, -2.0f, -1.0f, 1.0f };

    float contentOffsetX = 0.0f;
    float contentOffsetY = 0.0f;
    float contentAlpha = 1.0f;

    // 当前目标的缩放 / 偏移（PushClip 换算窗口坐标用）
    float targetScaleX = 1.0f;
    float targetScaleY = 1.0f;
    float targetOffX = 0.0f;
    float targetOffY = 0.0f;


    // ---- 1x1 白纹理（画纯色块用） ----

    GLuint whiteTexture = 0;


    // ---- 离屏场景 / 模糊 ----

    GLuint sceneFbo = 0;
    GLuint sceneTex = 0;

    GLuint blurFbo[2] = { 0, 0 };
    GLuint blurTex[2] = { 0, 0 };

    int sceneW = 0;
    int sceneH = 0;

    int blurW = 0;
    int blurH = 0;

    int blurResult = 0;

    // 高斯 sigma，单位 = 离屏（1/4 分辨率）纹素。4.0 → 16 逻辑像素。
    float blurSigma = 4.0f;

    bool sceneBlurValid = false;


    // ---- 文字 ----

    TTF_Font* font = nullptr;

    static constexpr size_t TEXT_CACHE_MAX = 768;

    std::unordered_map<std::string, TextCacheEntry> textCache;

    std::deque<std::string> textCacheOrder;

};


#endif
