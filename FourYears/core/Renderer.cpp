#include "Renderer.h"

#include <SDL.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>



// ==========================================================
// 着色器源码
// ==========================================================
//
// 用 GLSL 1.30 就能表达的写法（手动绑定 attribute location，
// 不使用 layout 限定符），这样在 3.3 core 和 3.0 上下文里
// 都能编译。真正的 "#version" 行在 CompileShader 里拼在最前面。

static const char* SPRITE_VS =
"in vec2 aPos;\n"
"in vec2 aUV;\n"
"in vec4 aColor;\n"
"uniform vec4 uXform;\n"
"out vec2 vUV;\n"
"out vec4 vColor;\n"
"void main()\n"
"{\n"
"    vec2 ndc = aPos * uXform.xy + uXform.zw;\n"
"    gl_Position = vec4(ndc, 0.0, 1.0);\n"
"    vUV = aUV;\n"
"    vColor = aColor;\n"
"}\n";


static const char* SPRITE_FS =
"uniform sampler2D uTex;\n"
"in vec2 vUV;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = texture(uTex, vUV) * vColor;\n"
"}\n";


static const char* BLUR_FS =
"uniform sampler2D uTex;\n"
"uniform vec2 uTexel;\n"
"uniform vec2 uDir;\n"
"uniform float uSigma;\n"
"in vec2 vUV;\n"
"in vec4 vColor;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    // 标准高斯：每个采样点按 exp(-i²/2σ²) 加权，采样间隔固定为\n"
"    // 1 个离屏纹素（不打散），12 对采样覆盖 ±3σ。\n"
"    // 这种做法得到的剖面就是标准高斯核的卷积结果，不会像\n"
"    // 「多趟拉大间隔」那样在亮区外洇出一圈光晕。\n"
"    vec2 off = uTexel * uDir;\n"
"    float inv2s2 = 1.0 / (2.0 * uSigma * uSigma);\n"
"    vec4 acc = texture(uTex, vUV);\n"
"    float wsum = 1.0;\n"
"    for(int i = 1; i <= 12; i++)\n"
"    {\n"
"        float fi = float(i);\n"
"        float w = exp(-fi * fi * inv2s2);\n"
"        acc += (texture(uTex, vUV + off * fi)\n"
"              + texture(uTex, vUV - off * fi)) * w;\n"
"        wsum += 2.0 * w;\n"
"    }\n"
"    fragColor = acc / wsum;\n"
"}\n";


static const char* GLASS_VS =
"in vec2 aPos;\n"
"in vec2 aUV;\n"
"in vec4 aColor;\n"
"uniform vec4 uXform;\n"
"uniform vec2 uPanelMin;\n"
"uniform vec2 uPanelSize;\n"
"out vec2 vLocal;\n"
"out vec2 vPixel;\n"
"void main()\n"
"{\n"
"    vec2 ndc = aPos * uXform.xy + uXform.zw;\n"
"    gl_Position = vec4(ndc, 0.0, 1.0);\n"
"    vLocal = aPos - uPanelMin;\n"
"    vPixel = aPos;\n"
"}\n";


static const char* GLASS_FS =
"uniform sampler2D uTex;\n"
"uniform sampler2D uSharp;\n"
"uniform vec2 uLogical;\n"
"uniform vec2 uPanelMin;\n"
"uniform vec2 uPanelSize;\n"
"uniform float uRadius;\n"
"uniform vec4 uTint;\n"
"uniform float uOpacity;\n"
"uniform float uShadow;\n"
"uniform float uGlassy;\n"
"uniform float uLiquid;\n"
"uniform vec3 uBackdrop;\n"
"uniform float uShade;\n"
"in vec2 vLocal;\n"
"in vec2 vPixel;\n"
"out vec4 fragColor;\n"
"\n"
"float sdRoundBox(vec2 p, vec2 b, float r)\n"
"{\n"
"    vec2 q = abs(p) - b + r;\n"
"    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    vec2 halfSize = uPanelSize * 0.5;\n"
"    float r = min(uRadius, min(halfSize.x, halfSize.y));\n"
"    vec2 p = vLocal - halfSize;\n"
"    float d = sdRoundBox(p, halfSize, r);\n"
"\n"
"    float aa = 1.0;\n"
"    float mask = 1.0 - smoothstep(-aa, aa, d);\n"
"\n"
"    vec3 col;\n"
"    if(uGlassy > 0.5)\n"
"    {\n"
"        // vPixel 是逻辑坐标（左上原点），离屏纹理自下而上，采样 V 要翻。\n"
"        vec2 uv0 = clamp(\n"
"            vec2(vPixel.x, uLogical.y - vPixel.y) / uLogical,\n"
"            vec2(0.0), vec2(1.0));\n"
"\n"
"        if(uLiquid > 0.5)\n"
"        {\n"
"            float minH = min(halfSize.x, halfSize.y);\n"
"\n"
"            // ---- 位移向量场 ----\n"
"            // 等价于文章里的 displacement map，只是这里直接用圆角矩形的\n"
"            // SDF 当位移场解析算出来，省掉一张贴图。\n"
"            // 外法线用解析混合，而不是对 SDF 直接做差分：\n"
"            // 圆角矩形的 SDF 是 max(qx,qy) 拼出来的，在 qx=qy 的\n"
"            // 45° 分界线上法线会从水平瞬间跳到垂直，位移场不连续，\n"
"            // 画面上就是一条硬邦邦的斜线。这里在分界线附近把两个\n"
"            // 方向平滑混合，整条法线场就是连续的，圆角处自然过渡。\n"
"            vec2 q = abs(p) - halfSize + vec2(r);\n"
"            float kS = clamp(minH * 0.30, 12.0, 120.0);\n"
"            float mixT = smoothstep(-kS, kS, q.x - q.y);\n"
"            vec2 n = normalize(mix(\n"
"                vec2(0.0, sign(p.y)),\n"
"                vec2(sign(p.x), 0.0),\n"
"                mixT\n"
"            ) + vec2(1e-6));\n"
"\n"
"            // 位移方向换算到 UV：Y 翻过来，并按宽高比补偿\n"
"            vec2 dirUV = vec2(n.x / uLogical.x, -n.y / uLogical.y);\n"
"\n"
"            // ---- 折射场铺满整块面板 ----\n"
"            // u：0 = 贴边，1 = 面板正中（圆角矩形的 SDF 距离在面板内\n"
"            // 最大正好等于最短边的一半，所以 u 天然覆盖整块，\n"
"            // 不会在中途停住留出一条「只有边上是玻璃」的圈）。\n"
"            float u = clamp(-d / max(minH, 1.0), 0.0, 1.0);\n"
"            // 越靠边弯折越强，往中间平滑衰减到 0；\n"
"            // 幂次让它集中在边缘，但整块面板都参与。\n"
"            float edge = pow(1.0 - u, 1.7);\n"
"\n"
"            // 名字牌、HUD 小格子这类小面板几乎不做折射：\n"
"            // 折射幅度按面板尺寸渐入，小块玻璃就还是安安静静一块深色。\n"
"            float strength = smoothstep(40.0, 90.0, minH);\n"
"            float band = edge * strength;\n"
"\n"
"            // 位移：边缘处最多把画面推 amt 像素，弯进玻璃里。\n"
"            float amt = clamp(minH * 0.45, 8.0, 110.0) * band;\n"
"\n"
"            // ---- 色散 ----\n"
"            // 位移只沿边界法线走：这样整条边框（包括四个圆角）拿到的\n"
"            // 位移量完全一样，圆角就是一条等宽的法线带，不会像按面板\n"
"            // 中心做整体放大那样——角上离心最远、被拉伸成边中点的\n"
"            // 两倍多，看起来就不圆了。\n"
"            // 位移随 u 衰减，自身的梯度就带来了「透镜压进来」的拉伸。\n"
"            // 三个通道的位移量各差一点点 → 红蓝边（色散）。\n"
"            vec2 uvG = uv0 + dirUV * (amt        );\n"
"            vec2 uvR = uv0 + dirUV * (amt * 1.06 );\n"
"            vec2 uvB = uv0 + dirUV * (amt * 0.94 );\n"
"\n"
"            // 折射采样「已经模糊过」的场景图：折射区本身也得是磨砂的，\n"
"            // 采样清晰图会变成「模糊背景里嵌了一块清晰区域」，很怪。\n"
"            // 再乘背板的压暗系数，边缘才不会比周围亮出一圈高光。\n"
"            vec3 refracted = vec3(\n"
"                texture(uTex, clamp(uvR, vec2(0.0), vec2(1.0))).r,\n"
"                texture(uTex, clamp(uvG, vec2(0.0), vec2(1.0))).g,\n"
"                texture(uTex, clamp(uvB, vec2(0.0), vec2(1.0))).b\n"
"            ) * uBackdrop;\n"
"\n"
"            // 主体和折射带必须用同一套亮度口径：折射采样乘了背板的\n"
"            // 压暗系数，这里也要乘，否则面板中间会比边缘亮一截，\n"
"            // 看起来就像「只有边上是玻璃」。\n"
"            vec3 blurred = texture(uTex, uv0).rgb * uBackdrop;\n"
"            vec3 glass = mix(blurred, refracted, band);\n"
"\n"
"            // ---- 直接把模糊结果贴上去 ----\n"
"            // 只整体乘一个压暗系数，不掺半透明暗色、也不走 alpha\n"
"            // 混合，所以面板里的画面就是「背景的高斯模糊版」，\n"
"            // 明暗关系原样保留，不会变成一层雾。\n"
"            vec3 bigLook = glass * uShade;\n"
"\n"
"            // 小面板（名字牌等）不做折射时保持原来的暗色板，\n"
"            // 免得名字这种小字压在乱糟糟的模糊背景上看不清。\n"
"            vec3 smallLook = mix(blurred, uTint.rgb, uTint.a);\n"
"\n"
"            col = mix(smallLook, bigLook, strength);\n"
"        }\n"
"        else\n"
"        {\n"
"            // 朴素版：暗色 + 模糊\n"
"            // 背板压暗系数和液态分支用同一套，两种风格切换时\n"
"            // 面板底色不会跟着跳。\n"
"            col = mix(\n"
"                texture(uTex, uv0).rgb * uBackdrop,\n"
"                uTint.rgb,\n"
"                uTint.a\n"
"            );\n"
"        }\n"
"    }\n"
"    else\n"
"    {\n"
"        col = uTint.rgb;\n"
"    }\n"
"\n"
"    float panelAlpha = mask * uOpacity;\n"
"\n"
"    float shadowAlpha = 0.0;\n"
"    if(uShadow > 0.001)\n"
"    {\n"
"        shadowAlpha = (1.0 - mask) * smoothstep(uShadow, 0.0, d) * 0.20;\n"
"    }\n"
"\n"
"    float outA = panelAlpha + shadowAlpha * (1.0 - panelAlpha);\n"
"    vec3 outC = (col * panelAlpha) / max(outA, 0.0001);\n"
"\n"
"    fragColor = vec4(outC, clamp(outA, 0.0, 1.0));\n"
"}\n";



// 运行时决定的 GLSL 版本行。
static std::string g_glslHeader = "#version 130\n";



// ==========================================================
// 构造 / 析构
// ==========================================================

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
    Destroy();
}



// ==========================================================
// 着色器编译
// ==========================================================

GLuint Renderer::CompileShader(
    GLenum type,
    const char* body
)
{
    GLuint shader = glCreateShader(type);

    if(!shader)
    {
        return 0;
    }

    std::string source = g_glslHeader + body;

    const GLchar* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

    if(!ok)
    {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

        std::vector<char> log((size_t)(len > 1 ? len : 1));
        glGetShaderInfoLog(shader, len, nullptr, log.data());

        std::cout << "着色器编译失败:\n" << log.data() << std::endl;

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}



GLuint Renderer::LinkProgram(
    const char* vs,
    const char* fs,
    const char* name
)
{
    GLuint vert = CompileShader(GL_VERTEX_SHADER, vs);
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, fs);

    if(!vert || !frag)
    {
        if(vert) glDeleteShader(vert);
        if(frag) glDeleteShader(frag);

        return 0;
    }

    GLuint prog = glCreateProgram();

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    // 手动绑定 attribute，兼容 #version 130
    glBindAttribLocation(prog, 0, "aPos");
    glBindAttribLocation(prog, 1, "aUV");
    glBindAttribLocation(prog, 2, "aColor");

    glLinkProgram(prog);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);

    glDeleteShader(vert);
    glDeleteShader(frag);

    if(!ok)
    {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);

        std::vector<char> log((size_t)(len > 1 ? len : 1));
        glGetProgramInfoLog(prog, len, nullptr, log.data());

        std::cout << "程序链接失败(" << name << "):\n" << log.data() << std::endl;

        glDeleteProgram(prog);

        return 0;
    }

    return prog;
}



bool Renderer::CreatePrograms()
{
    progSprite = LinkProgram(SPRITE_VS, SPRITE_FS, "sprite");
    progBlur   = LinkProgram(SPRITE_VS, BLUR_FS,   "blur");
    progGlass  = LinkProgram(GLASS_VS,  GLASS_FS,  "glass");

    if(!progSprite || !progBlur || !progGlass)
    {
        return false;
    }

    uSpriteXform = glGetUniformLocation(progSprite, "uXform");
    uSpriteTex   = glGetUniformLocation(progSprite, "uTex");

    uBlurXform = glGetUniformLocation(progBlur, "uXform");
    uBlurTex   = glGetUniformLocation(progBlur, "uTex");
    uBlurTexel = glGetUniformLocation(progBlur, "uTexel");
    uBlurDir   = glGetUniformLocation(progBlur, "uDir");
    uBlurSigma = glGetUniformLocation(progBlur, "uSigma");

    uGlassXform   = glGetUniformLocation(progGlass, "uXform");
    uGlassTex     = glGetUniformLocation(progGlass, "uTex");
    uGlassLogical = glGetUniformLocation(progGlass, "uLogical");
    uGlassMin     = glGetUniformLocation(progGlass, "uPanelMin");
    uGlassSize    = glGetUniformLocation(progGlass, "uPanelSize");
    uGlassRadius  = glGetUniformLocation(progGlass, "uRadius");
    uGlassTint    = glGetUniformLocation(progGlass, "uTint");
    uGlassOpacity = glGetUniformLocation(progGlass, "uOpacity");
    uGlassShadow  = glGetUniformLocation(progGlass, "uShadow");
    uGlassGlassy  = glGetUniformLocation(progGlass, "uGlassy");
    uGlassLiquid  = glGetUniformLocation(progGlass, "uLiquid");
    uGlassSharp   = glGetUniformLocation(progGlass, "uSharp");
    uGlassBackdrop = glGetUniformLocation(progGlass, "uBackdrop");
    uGlassShade    = glGetUniformLocation(progGlass, "uShade");

    return true;
}



void Renderer::DestroyPrograms()
{
    if(progSprite) { glDeleteProgram(progSprite); progSprite = 0; }
    if(progBlur)   { glDeleteProgram(progBlur);   progBlur   = 0; }
    if(progGlass)  { glDeleteProgram(progGlass);  progGlass  = 0; }

    activeProgram = 0;
}



// ==========================================================
// 初始化 / 销毁
// ==========================================================

bool Renderer::Init(SDL_Window* win)
{
    window = win;

    glContext = SDL_GL_CreateContext(window);

    if(!glContext)
    {
        std::cout << "GL 上下文创建失败: " << SDL_GetError() << std::endl;

        return false;
    }

    SDL_GL_MakeCurrent(window, glContext);


    // 垂直同步。失败也不致命（软件渲染 / offscreen 时常见）。
    if(SDL_GL_SetSwapInterval(1) != 0)
    {
        SDL_GL_SetSwapInterval(0);
        vsyncEnabled = false;
    }
    else
    {
        vsyncEnabled = true;
    }


    if(!GL_LoadFunctions())
    {
        std::cout << "OpenGL 入口点加载失败，无法继续" << std::endl;

        return false;
    }


    // 按实际上下文版本选择合适的 GLSL 版本行。
    {
        const char* ver =
            (const char*)glGetString(GL_VERSION);

        int major = 0;
        int minor = 0;

        if(ver)
        {
            std::sscanf(ver, "%d.%d", &major, &minor);

            std::cout << "OpenGL: " << ver
                      << " | " << (const char*)glGetString(GL_RENDERER)
                      << std::endl;
        }

        if(major > 3 || (major == 3 && minor >= 3))
        {
            g_glslHeader = "#version 330 core\n";
        }
        else if(major == 3)
        {
            g_glslHeader = "#version 130\n";
        }
        else
        {
            std::cout << "需要 OpenGL 3.0 及以上" << std::endl;

            return false;
        }
    }


    if(!CreatePrograms())
    {
        return false;
    }


    // 动态四边形缓冲区 + VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const GLsizei stride = (GLsizei)sizeof(Vertex);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, stride,
        (const void*)offsetof(Vertex, x)
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, stride,
        (const void*)offsetof(Vertex, u)
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 4, GL_FLOAT, GL_FALSE, stride,
        (const void*)offsetof(Vertex, r)
    );

    glBindVertexArray(0);


    // 1x1 白色纹理，用来画纯色矩形
    {
        const unsigned char white[4] = { 255, 255, 255, 255 };

        glGenTextures(1, &whiteTexture);
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, white
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }


    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    UpdateDrawableSize();
    SetTargetTransform(drawableW, drawableH, true);


    ready = true;

    return true;
}



void Renderer::Destroy()
{
    ClearTextCache();

    if(glContext)
    {
        SDL_GL_MakeCurrent(window, glContext);

        if(vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
        if(vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }

        if(whiteTexture)
        {
            glDeleteTextures(1, &whiteTexture);
            whiteTexture = 0;
        }

        if(sceneTex) { glDeleteTextures(1, &sceneTex); sceneTex = 0; }
        if(sceneFbo) { glDeleteFramebuffers(1, &sceneFbo); sceneFbo = 0; }

        for(int i = 0; i < 2; i++)
        {
            if(blurTex[i]) { glDeleteTextures(1, &blurTex[i]); blurTex[i] = 0; }
            if(blurFbo[i]) { glDeleteFramebuffers(1, &blurFbo[i]); blurFbo[i] = 0; }
        }

        DestroyPrograms();

        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }

    ready = false;
}



// ==========================================================
// 逻辑分辨率 / 渲染目标
// ==========================================================

void Renderer::SetLogicalSize(int w, int h)
{
    if(w <= 0 || h <= 0)
    {
        return;
    }

    logicalW = w;
    logicalH = h;

    // 逻辑分辨率变化后，离屏缓冲需要重建。
    if(sceneFbo)
    {
        glDeleteTextures(1, &sceneTex);
        glDeleteFramebuffers(1, &sceneFbo);

        sceneTex = 0;
        sceneFbo = 0;
        sceneW = 0;
        sceneH = 0;
    }

    for(int i = 0; i < 2; i++)
    {
        if(blurTex[i]) { glDeleteTextures(1, &blurTex[i]); blurTex[i] = 0; }
        if(blurFbo[i]) { glDeleteFramebuffers(1, &blurFbo[i]); blurFbo[i] = 0; }
    }

    blurW = 0;
    blurH = 0;
}


int Renderer::GetLogicalWidth() const
{
    return logicalW;
}


int Renderer::GetLogicalHeight() const
{
    return logicalH;
}



void Renderer::UpdateDrawableSize()
{
    int w = 0;
    int h = 0;

    SDL_GL_GetDrawableSize(window, &w, &h);

    if(w > 0 && h > 0)
    {
        drawableW = w;
        drawableH = h;
    }
}



void Renderer::SetTargetTransform(
    int fbW,
    int fbH,
    bool letterbox,
    bool cover
)
{
    if(fbW <= 0 || fbH <= 0)
    {
        return;
    }

    float scaleX;
    float scaleY;
    float offX;
    float offY;

    if(letterbox)
    {
        float sx = (float)fbW / (float)logicalW;
        float sy = (float)fbH / (float)logicalH;

        // cover：取大的缩放比，画面铺满窗口（多出来的裁掉，没有黑边）
        // fit  ：取小的缩放比，画面完整可见（可能留边）
        float scale = cover
                    ? (sx > sy ? sx : sy)
                    : (sx < sy ? sx : sy);

        scaleX = scale;
        scaleY = scale;

        // 文字按 UI 的缩放光栅化，所以只有 fit 时更新
        if(!cover)
        {
            contentScale = scale;
        }

        offX = ((float)fbW - (float)logicalW * scale) * 0.5f;
        offY = ((float)fbH - (float)logicalH * scale) * 0.5f;
    }
    else
    {
        // 离屏缓冲：直接铺满
        scaleX = (float)fbW / (float)logicalW;
        scaleY = (float)fbH / (float)logicalH;

        offX = 0.0f;
        offY = 0.0f;
    }

    xform[0] = 2.0f * scaleX / (float)fbW;
    xform[1] = -2.0f * scaleY / (float)fbH;
    xform[2] = 2.0f * offX / (float)fbW - 1.0f;
    xform[3] = 1.0f - 2.0f * offY / (float)fbH;

    targetScaleX = scaleX;
    targetScaleY = scaleY;
    targetOffX = offX;
    targetOffY = offY;

    // letterbox 偏移取整：不然逻辑原点会落在半像素上，
    // 线性采样下所有文字每帧都在抖动（亚像素重采样）。
    if(letterbox && fbW > 0 && fbH > 0)
    {
        float snapX = std::floor(offX + 0.5f);
        float snapY = std::floor(offY + 0.5f);

        if(snapX != offX)
        {
            offX = snapX;
            xform[2] = 2.0f * offX / (float)fbW - 1.0f;
        }

        if(snapY != offY)
        {
            offY = snapY;
            xform[3] = 1.0f - 2.0f * offY / (float)fbH;
        }
    }
}



// ==========================================================
// 离屏场景 + 模糊
// ==========================================================

void Renderer::EnsureSceneTargets(int w, int h)
{
    if(w <= 0 || h <= 0)
    {
        return;
    }

    if(sceneFbo && sceneW == w && sceneH == h)
    {
        return;
    }

    if(sceneTex) { glDeleteTextures(1, &sceneTex); sceneTex = 0; }
    if(sceneFbo) { glDeleteFramebuffers(1, &sceneFbo); sceneFbo = 0; }

    glGenTextures(1, &sceneTex);
    glBindTexture(GL_TEXTURE_2D, sceneTex);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &sceneFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex, 0
    );

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "场景 FBO 不完整" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sceneW = w;
    sceneH = h;

    sceneBlurValid = false;
}



void Renderer::EnsureBlurTargets(int w, int h)
{
    int bw = w / 4;
    int bh = h / 4;

    if(bw < 1) bw = 1;
    if(bh < 1) bh = 1;

    if(blurFbo[0] && blurW == bw && blurH == bh)
    {
        return;
    }

    for(int i = 0; i < 2; i++)
    {
        if(blurTex[i]) { glDeleteTextures(1, &blurTex[i]); blurTex[i] = 0; }
        if(blurFbo[i]) { glDeleteFramebuffers(1, &blurFbo[i]); blurFbo[i] = 0; }
    }

    for(int i = 0; i < 2; i++)
    {
        glGenTextures(1, &blurTex[i]);
        glBindTexture(GL_TEXTURE_2D, blurTex[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA8, bw, bh, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenFramebuffers(1, &blurFbo[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, blurFbo[i]);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTex[i], 0
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    blurW = bw;
    blurH = bh;

    sceneBlurValid = false;
}



void Renderer::GenerateSceneBlur()
{
    if(!sceneTex)
    {
        return;
    }

    EnsureBlurTargets(sceneW, sceneH);

    if(!blurTex[0] || !blurFbo[0])
    {
        return;
    }


    // 1) 场景 → 1/4 分辨率（线性下采样）
    glBindFramebuffer(GL_FRAMEBUFFER, blurFbo[0]);
    glViewport(0, 0, blurW, blurH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SetTargetTransform(blurW, blurH, false);

    glUseProgram(progSprite);
    activeProgram = progSprite;
    glUniform4f(uSpriteXform, xform[0], xform[1], xform[2], xform[3]);
    glUniform1i(uSpriteTex, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTex);
    batchTexture = sceneTex;

    PushQuad(
        sceneTex,
        0.0f, 0.0f, (float)logicalW, (float)logicalH,
        0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    );
    Flush();


    // 2) 直接对整屏做一趟横向 + 一趟纵向的标准高斯模糊
    //
    // 半径取 σ = 16 逻辑像素（离屏是 1/4 尺寸，所以核里 σ = 4 个
    // 离屏纹素）。背板、玻璃面板、液态折射采样的是同一张结果，
    // 所以看到的就是"背景被高斯糊过"这一个确定的效果，没有分层、
    // 没有额外洇散。
    // 横向：blurTex[0] → blurFbo[1]；纵向：blurTex[1] → blurFbo[0]
    BlurAxis(0, 1, 1.0f, 0.0f);
    BlurAxis(1, 0, 0.0f, 1.0f);

    blurResult = 0;

    sceneBlurValid = true;
}



void Renderer::BlurAxis(int src, int dst, float dirX, float dirY)
{
    if(!blurTex[src] || !blurFbo[dst])
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, blurFbo[dst]);
    glViewport(0, 0, blurW, blurH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(progBlur);
    activeProgram = progBlur;

    glUniform4f(uBlurXform, xform[0], xform[1], xform[2], xform[3]);
    glUniform1i(uBlurTex, 0);
    glUniform2f(
        uBlurTexel,
        1.0f / (float)blurW,
        1.0f / (float)blurH
    );

    // 必须在 progBlur 绑定之后再设，否则 uniform 会落到别的 program 上
    glUniform1f(uBlurSigma, blurSigma);
    glUniform2f(uBlurDir, dirX, dirY);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blurTex[src]);
    batchTexture = blurTex[src];

    PushQuad(
        blurTex[src],
        0.0f, 0.0f, (float)logicalW, (float)logicalH,
        0.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    );
    Flush();
}



// ==========================================================
// 帧流程
// ==========================================================

void Renderer::BeginScene()
{
    drawCalls = 0;
    batchTexture = 0;
    vertices.clear();

    UpdateDrawableSize();

    EnsureSceneTargets(logicalW, logicalH);

    glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo);
    glViewport(0, 0, logicalW, logicalH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SetTargetTransform(logicalW, logicalH, false);

    if(activeProgram != progSprite)
    {
        glUseProgram(progSprite);
        activeProgram = progSprite;
    }

    glUniform4f(uSpriteXform, xform[0], xform[1], xform[2], xform[3]);
    glUniform1i(uSpriteTex, 0);
}



void Renderer::EndScene(bool frostedBackdrop)
{
    Flush();

    GenerateSceneBlur();


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, drawableW, drawableH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 背景用"铺满"：窗口比例不是 16:9 时也不留黑边
    SetTargetTransform(drawableW, drawableH, true, true);

    UseSprite();

    if(frostedBackdrop && blurTex[blurResult])
    {
        // 强模糊背景：轻微压暗，突出前景
        //
        // 注意：离屏纹理的行序和上传的图片是反的（GL 的 FBO 原点在
        // 左下角），所以这里 V 要翻过来，否则整屏画面会上下颠倒。
        //
        // 这个压暗系数同时告诉玻璃面板：折射采样也要按同样比例压暗，
        // 不然面板边缘会比周围亮一圈。
        backdropShadeR = 0.58f;
        backdropShadeG = 0.61f;
        backdropShadeB = 0.67f;

        PushQuad(
            blurTex[blurResult],
            0.0f, 0.0f, (float)logicalW, (float)logicalH,
            0.0f, 1.0f, 1.0f, 0.0f,
            backdropShadeR, backdropShadeG, backdropShadeB, 1.0f
        );
    }
    else
    {
        // 清晰场景
        backdropShadeR = 1.0f;
        backdropShadeG = 1.0f;
        backdropShadeB = 1.0f;

        PushQuad(
            sceneTex,
            0.0f, 0.0f, (float)logicalW, (float)logicalH,
            0.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        );
    }

    // 背景必须先用 cover 变换真正画出去，再换 UI 的变换
    // （同一批四边形只吃一个变换，混在一起会被后设的覆盖）
    Flush();

    // 之后画的 UI 换回"放下"：等比缩放居中，和窗口同心，不会被裁掉
    SetTargetTransform(drawableW, drawableH, true, false);
}



void Renderer::Present()
{
    Flush();

    SDL_GL_SwapWindow(window);
}



// ==========================================================
// 四边形批处理
// ==========================================================

void Renderer::UseSprite()
{
    if(activeProgram != progSprite)
    {
        glUseProgram(progSprite);
        activeProgram = progSprite;
    }

    glUniform4f(uSpriteXform, xform[0], xform[1], xform[2], xform[3]);
    glUniform1i(uSpriteTex, 0);
    glActiveTexture(GL_TEXTURE0);
}



void Renderer::PushQuad(
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
)
{
    // 像素对齐：UI 动画会给出小数偏移（上滑淡入等），
    // 不取整的话文字每帧落在不同的亚像素上，线性采样就会
    // 一直抖。所有图元统一取整到逻辑像素，文字就稳定了。
    x = std::floor(x + contentOffsetX + 0.5f);
    y = std::floor(y + contentOffsetY + 0.5f);

    a *= contentAlpha;

    if(texture != batchTexture)
    {
        Flush();

        batchTexture = texture;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    Vertex v;
    v.r = r;
    v.g = g;
    v.b = b;
    v.a = a;

    const float x1 = x + w;
    const float y1 = y + h;

    // 两个三角形
    const float px[6] = { x, x1, x1, x, x1, x };
    const float py[6] = { y, y,  y1, y, y1, y1 };
    const float tu[6] = { u0, u1, u1, u0, u1, u0 };
    const float tv[6] = { v0, v0, v1, v0, v1, v1 };

    for(int i = 0; i < 6; i++)
    {
        v.x = px[i];
        v.y = py[i];
        v.u = tu[i];
        v.v = tv[i];

        vertices.push_back(v);
    }
}



void Renderer::Flush()
{
    if(vertices.empty())
    {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const GLsizeiptr bytes =
        (GLsizeiptr)(vertices.size() * sizeof(Vertex));

    // orphan + 上传，避免驱动为每帧缓冲做同步
    glBufferData(GL_ARRAY_BUFFER, bytes, nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, vertices.data());

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
    glBindVertexArray(0);

    vertices.clear();

    ++drawCalls;
}



// ==========================================================
// 图元
// ==========================================================

void Renderer::DrawTexture(SDL_Texture* texture, int x, int y)
{
    int tw = 0, th = 0;

    if(!GetTextureSize(texture, tw, th))
    {
        return;
    }

    DrawTexture(texture, x, y, tw, th);
}



void Renderer::DrawTexture(SDL_Texture* texture, int x, int y, int w, int h)
{
    DrawTextureEx(
        texture,
        (float)x, (float)y, (float)w, (float)h,
        1.0f, false, 255, 255, 255
    );
}



void Renderer::DrawTextureEx(
    SDL_Texture* texture,
    float x,
    float y,
    float w,
    float h,
    float alpha,
    bool flipX,
    Uint8 tintR,
    Uint8 tintG,
    Uint8 tintB
)
{
    auto it = glTextures.find(texture);

    if(!texture || it == glTextures.end() || !it->second.id
       || w <= 0.0f || h <= 0.0f)
    {
        return;
    }

    if(alpha <= 0.0f)
    {
        return;
    }

    UseSprite();

    const float r = (float)tintR / 255.0f;
    const float g = (float)tintG / 255.0f;
    const float b = (float)tintB / 255.0f;

    float u0 = 0.0f;
    float u1 = 1.0f;

    if(flipX)
    {
        u0 = 1.0f;
        u1 = 0.0f;
    }

    PushQuad(
        it->second.id,
        x, y, w, h,
        u0, 0.0f, u1, 1.0f,
        r, g, b, alpha
    );
}



void Renderer::DrawFilledRect(
    int x,
    int y,
    int w,
    int h,
    Uint8 r,
    Uint8 g,
    Uint8 b,
    Uint8 a
)
{
    if(w <= 0 || h <= 0 || a == 0 || !ready)
    {
        return;
    }

    UseSprite();

    PushQuad(
        whiteTexture,
        (float)x, (float)y, (float)w, (float)h,
        0.0f, 0.0f, 1.0f, 1.0f,
        (float)r / 255.0f,
        (float)g / 255.0f,
        (float)b / 255.0f,
        (float)a / 255.0f
    );
}



void Renderer::DrawRoundedRect(
    float x,
    float y,
    float w,
    float h,
    float radius,
    Uint8 r,
    Uint8 g,
    Uint8 b,
    Uint8 a
)
{
    DrawShape(
        x, y, w, h, radius,
        (float)r / 255.0f,
        (float)g / 255.0f,
        (float)b / 255.0f,
        1.0f,
        (float)a / 255.0f,
        0.0f,
        false
    );
}



void Renderer::DrawGlassPanel(
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
)
{
    DrawShape(
        x, y, w, h, radius,
        tintR, tintG, tintB,
        tintAlpha, opacity, shadow, true
    );
}



void Renderer::DrawShape(
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
)
{
    if(w <= 0.0f || h <= 0.0f || opacity <= 0.0f)
    {
        return;
    }

    Flush();

    glUseProgram(progGlass);
    activeProgram = progGlass;

    glUniform4f(uGlassXform, xform[0], xform[1], xform[2], xform[3]);
    glUniform2f(uGlassLogical, (float)logicalW, (float)logicalH);
    glUniform2f(uGlassMin, x, y);
    glUniform2f(uGlassSize, w, h);
    glUniform1f(uGlassRadius, radius);
    glUniform4f(uGlassTint, tintR, tintG, tintB, tintAlpha);
    glUniform1f(uGlassOpacity, opacity);
    glUniform1f(uGlassShadow, shadow);
    glUniform1f(uGlassGlassy, glassy ? 1.0f : 0.0f);
    glUniform1f(uGlassLiquid, liquidGlass ? 1.0f : 0.0f);
    glUniform3f(
        uGlassBackdrop,
        backdropShadeR,
        backdropShadeG,
        backdropShadeB
    );
    glUniform1f(uGlassShade, glassShade);

    glUniform1i(uGlassTex, 0);
    glUniform1i(uGlassSharp, 1);

    // 单元 1 绑未模糊的场景图。当前液态玻璃的折射改采样模糊图
    // （折射区必须是磨砂的），这里保留绑定，方便以后需要
    // "折射区掺一点清晰细节" 时直接取用。
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(
        GL_TEXTURE_2D,
        (glassy && sceneTex) ? sceneTex : whiteTexture
    );

    glActiveTexture(GL_TEXTURE0);

    // 面板用"重糊"那一级：比整屏背板更糊，才像是透过一块厚玻璃看。
    if(glassy && blurTex[blurResult])
    {
        glBindTexture(GL_TEXTURE_2D, blurTex[blurResult]);
        batchTexture = blurTex[blurResult];
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        batchTexture = whiteTexture;
    }

    // 留出投影空间；至少要 2px，保证圆角描边的抗锯齿不会被裁掉
    float pad = shadow > 1.0f ? shadow + 2.0f : 2.0f;

    PushQuad(
        batchTexture,
        x - pad, y - pad, w + pad * 2.0f, h + pad * 2.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    );

    Flush();
}



// ==========================================================
// 文字
// ==========================================================

void Renderer::SetFont(TTF_Font* f)
{
    font = f;
}



void Renderer::SetContentOffset(float dx, float dy)
{
    contentOffsetX = dx;
    contentOffsetY = dy;
}



void Renderer::SetContentAlpha(float alpha)
{
    contentAlpha = alpha < 0.0f ? 0.0f : (alpha > 1.0f ? 1.0f : alpha);
}



void Renderer::PushClip(
    float x,
    float y,
    float w,
    float h
)
{
    if(!ready || w <= 0.0f || h <= 0.0f)
    {
        return;
    }

    // 批次里的四边形只吃一个裁剪框，切之前先画掉
    Flush();

    int px = (int)(targetOffX + x * targetScaleX);
    int pw = (int)(w * targetScaleX + 0.5f);
    int ph = (int)(h * targetScaleY + 0.5f);

    // GL 的裁剪原点在左下角
    int py = (int)((float)drawableH
                 - (targetOffY + (y + h) * targetScaleY));

    glEnable(GL_SCISSOR_TEST);
    glScissor(px, py, pw, ph);
}



void Renderer::PopClip()
{
    if(!ready)
    {
        return;
    }

    Flush();

    glDisable(GL_SCISSOR_TEST);
}



void Renderer::ClearTextCache()
{
    if(!glContext)
    {
        textCache.clear();
        textCacheOrder.clear();

        return;
    }

    for(auto& pair : textCache)
    {
        if(pair.second.texture)
        {
            glDeleteTextures(1, &pair.second.texture);
        }
    }

    textCache.clear();
    textCacheOrder.clear();
}



void Renderer::DrawText(const std::string& text, int x, int y)
{
    SDL_Color white;
    white.r = 255;
    white.g = 255;
    white.b = 255;
    white.a = 255;

    DrawText(text, x, y, font, white);
}



void Renderer::DrawText(
    const std::string& text,
    int x,
    int y,
    SDL_Color color
)
{
    DrawText(text, x, y, font, color);
}



void Renderer::DrawText(
    const std::string& text,
    int x,
    int y,
    TTF_Font* f,
    SDL_Color color
)
{
    const TextCacheEntry* entry = GetOrCreateText(text, f, color);

    if(!entry)
    {
        return;
    }

    UseSprite();

    const float a = (float)color.a / 255.0f;

    // 纹理是按 size*textScale 光栅化的，这里缩回逻辑尺寸
    const float inv = (textScale > 0.01f) ? (1.0f / textScale) : 1.0f;

    PushQuad(
        entry->texture,
        (float)x, (float)y,
        (float)entry->w * inv, (float)entry->h * inv,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, a
    );
}



const Renderer::TextCacheEntry* Renderer::GetOrCreateText(
    const std::string& text,
    TTF_Font* f,
    SDL_Color color
)
{
    if(!ready || !f || text.empty())
    {
        return nullptr;
    }

    // 缓存键 = 字体 + 颜色 + 文本
    std::string key;
    key.append(reinterpret_cast<const char*>(&f), sizeof(f));
    key.push_back('\x1f');
    key += std::to_string(color.r);
    key.push_back(',');
    key += std::to_string(color.g);
    key.push_back(',');
    key += std::to_string(color.b);
    key.push_back(',');
    key += std::to_string(color.a);
    key.push_back('\x1f');
    key += text;

    auto it = textCache.find(key);

    if(it != textCache.end())
    {
        return &it->second;
    }

    // ==========================================================
    // 上传新字形纹理之前，必须先把批次里已经攒好的四边形画掉。
    // ==========================================================
    //
    // 下面 glBindTexture 会把 GL 的纹理绑定切到新纹理上，
    // 而批处理还以为当前绑的是旧纹理（batchTexture）。
    // 如果不先 Flush，接下来 PushQuad 触发的 Flush 就会把
    // "之前几行文字"用新纹理画出来 —— 表现就是打字机每冒出一个
    // 新字符，上一行文字就抽搐一下。

    Flush();

    batchTexture = 0;

    SDL_Surface* rendered =
        TTF_RenderUTF8_Blended(f, text.c_str(), color);

    if(!rendered)
    {
        return nullptr;
    }

    SDL_Surface* rgba =
        SDL_ConvertSurfaceFormat(
            rendered,
            SDL_PIXELFORMAT_ABGR8888,
            0
        );

    SDL_FreeSurface(rendered);

    if(!rgba)
    {
        return nullptr;
    }

    TextCacheEntry entry;
    entry.w = rgba->w;
    entry.h = rgba->h;

    // ---- 扫出墨迹范围（alpha > 8 的像素）----
    {
        const unsigned char* p = (const unsigned char*)rgba->pixels;

        int minX = rgba->w;
        int minY = rgba->h;
        int maxX = -1;
        int maxY = -1;

        for(int y = 0; y < rgba->h; y++)
        {
            for(int x = 0; x < rgba->w; x++)
            {
                if(p[((size_t)y * rgba->w + x) * 4 + 3] > 8)
                {
                    if(x < minX) minX = x;
                    if(y < minY) minY = y;
                    if(x > maxX) maxX = x;
                    if(y > maxY) maxY = y;
                }
            }
        }

        if(maxX >= minX && maxY >= minY)
        {
            entry.inkX = minX;
            entry.inkY = minY;
            entry.inkW = maxX - minX + 1;
            entry.inkH = maxY - minY + 1;
        }
        else
        {
            entry.inkX = 0;
            entry.inkY = 0;
            entry.inkW = rgba->w;
            entry.inkH = rgba->h;
        }
    }

    glGenTextures(1, &entry.texture);
    glBindTexture(GL_TEXTURE_2D, entry.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        rgba->w, rgba->h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, rgba->pixels
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_FreeSurface(rgba);


    if(textCache.size() >= TEXT_CACHE_MAX && !textCacheOrder.empty())
    {
        auto oldest = textCache.find(textCacheOrder.front());

        if(oldest != textCache.end())
        {
            if(oldest->second.texture)
            {
                glDeleteTextures(1, &oldest->second.texture);
            }

            textCache.erase(oldest);
        }

        textCacheOrder.pop_front();
    }

    textCache[key] = entry;
    textCacheOrder.push_back(key);

    return &textCache.find(key)->second;
}



bool Renderer::MeasureTextInk(
    const std::string& text,
    SDL_Color color,
    int& inkW,
    int& inkH,
    int& offX,
    int& offY
)
{
    inkW = 0;
    inkH = 0;
    offX = 0;
    offY = 0;

    const TextCacheEntry* entry = GetOrCreateText(text, font, color);

    if(!entry)
    {
        return false;
    }

    const float inv = (textScale > 0.01f) ? (1.0f / textScale) : 1.0f;

    offX = (int)std::lround((float)entry->inkX * inv);
    offY = (int)std::lround((float)entry->inkY * inv);

    inkW = (int)std::lround((float)entry->inkW * inv);
    inkH = (int)std::lround((float)entry->inkH * inv);

    return inkW > 0 && inkH > 0;
}



float Renderer::GetContentScale() const
{
    return contentScale;
}



bool Renderer::MeasureText(
    const std::string& text,
    int& w,
    int& h
) const
{
    w = 0;
    h = 0;

    return MeasureText(text, font, w, h);
}



bool Renderer::MeasureText(
    const std::string& text,
    TTF_Font* f,
    int& w,
    int& h
) const
{
    w = 0;
    h = 0;

    if(!ready || !f || text.empty())
    {
        return false;
    }

    int tw = 0;
    int th = 0;

    if(TTF_SizeUTF8(f, text.c_str(), &tw, &th) != 0)
    {
        return false;
    }

    const float inv = (textScale > 0.01f) ? (1.0f / textScale) : 1.0f;

    w = (int)std::lround((float)tw * inv);
    h = (int)std::lround((float)th * inv);

    return true;
}



void Renderer::SetTextScale(float scale)
{
    if(scale < 0.5f)
    {
        scale = 0.5f;
    }

    if(scale > 4.0f)
    {
        scale = 4.0f;
    }

    textScale = scale;
}



// ==========================================================
// 调试
// ==========================================================

int Renderer::GetDrawCalls() const
{
    return drawCalls;
}



bool Renderer::IsVSyncEnabled() const
{
    return vsyncEnabled;
}



void Renderer::SetLiquidGlass(bool on)
{
    liquidGlass = on;
}



bool Renderer::IsLiquidGlass() const
{
    return liquidGlass;
}



bool Renderer::SaveScreenshot(const std::string& path)
{
    if(!ready || drawableW <= 0 || drawableH <= 0)
    {
        return false;
    }

    Flush();

    const int w = drawableW;
    const int h = drawableH;

    std::vector<unsigned char> pixels((size_t)w * (size_t)h * 4);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(
        0, 0, w, h,
        GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()
    );

    // BMP：24 位，行从下到上（正好和 glReadPixels 一致，不用翻转）
    const int rowBytes = w * 3;
    const int padding = (4 - (rowBytes % 4)) % 4;
    const int stride = rowBytes + padding;

    const int dataSize = stride * h;
    const int fileSize = 54 + dataSize;

    FILE* fp = std::fopen(path.c_str(), "wb");

    if(!fp)
    {
        return false;
    }

    unsigned char header[54] = { 0 };

    header[0] = 'B';
    header[1] = 'M';

    header[2] = (unsigned char)(fileSize);
    header[3] = (unsigned char)(fileSize >> 8);
    header[4] = (unsigned char)(fileSize >> 16);
    header[5] = (unsigned char)(fileSize >> 24);

    header[10] = 54;

    header[14] = 40;

    header[18] = (unsigned char)(w);
    header[19] = (unsigned char)(w >> 8);
    header[20] = (unsigned char)(w >> 16);
    header[21] = (unsigned char)(w >> 24);

    header[22] = (unsigned char)(h);
    header[23] = (unsigned char)(h >> 8);
    header[24] = (unsigned char)(h >> 16);
    header[25] = (unsigned char)(h >> 24);

    header[26] = 1;   // planes
    header[28] = 24;  // bpp

    std::fwrite(header, 1, 54, fp);

    std::vector<unsigned char> row((size_t)stride, 0);

    for(int y = 0; y < h; y++)
    {
        const unsigned char* src =
            &pixels[(size_t)y * (size_t)w * 4];

        for(int x = 0; x < w; x++)
        {
            row[x * 3 + 0] = src[x * 4 + 2];
            row[x * 3 + 1] = src[x * 4 + 1];
            row[x * 3 + 2] = src[x * 4 + 0];
        }

        std::fwrite(row.data(), 1, stride, fp);
    }

    std::fclose(fp);

    return true;
}


// ==========================================================
// 兼容层：本项目既有接口
// ==========================================================
//
// 上面是 OpenGL 渲染核心（来自同学 paimon-git 的实现）。
// 下面这一层把接口对齐到本项目 UI 代码一直在用的签名，
// 目的是让 FourYears/ui/*.cpp 一行都不用改。
//
// 主要差异：
//   1. 纹理句柄是 SDL_Texture* 假指针，真实 GL id 存在 glTextures 表
//   2. 圆角矩形叫 DrawFilledRoundRect（同学那边叫 DrawRoundedRect）
//   3. 模糊 / 毛玻璃接口按"纹理 + 区域"的形式提供


// ----------------------------------------------------------
// 纹理管理
// ----------------------------------------------------------

SDL_Texture* Renderer::CreateTextureFromSurface(SDL_Surface* surface)
{
    if(!surface)
        return nullptr;

    // 统一转成 RGBA32，避免各种格式差异
    SDL_Surface* conv = SDL_ConvertSurfaceFormat(
        surface, SDL_PIXELFORMAT_ABGR8888, 0
    );

    if(!conv)
        return nullptr;

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        conv->w, conv->h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE,
        conv->pixels
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int w = conv->w;
    int h = conv->h;

    SDL_FreeSurface(conv);

    if(!id)
        return nullptr;

    // 发一个假句柄
    SDL_Texture* handle = nextHandle;
    nextHandle = (SDL_Texture*)((uintptr_t)nextHandle + 0x10);

    TextureEntry entry;
    entry.id = id;
    entry.width = w;
    entry.height = h;

    glTextures[handle] = entry;

    return handle;
}




void Renderer::DestroyTexture(SDL_Texture* texture)
{
    if(!texture)
        return;

    auto it = glTextures.find(texture);

    if(it == glTextures.end())
        return;

    if(it->second.id)
        glDeleteTextures(1, &it->second.id);

    glTextures.erase(it);
}




bool Renderer::GetTextureSize(SDL_Texture* texture, int& w, int& h)
{
    w = 0;
    h = 0;

    if(!texture)
        return false;

    auto it = glTextures.find(texture);

    if(it == glTextures.end() || !it->second.id)
        return false;

    w = it->second.width;
    h = it->second.height;

    return w > 0 && h > 0;
}




// ----------------------------------------------------------
// 生命周期 / 帧
// ----------------------------------------------------------

void Renderer::Shutdown()
{
    Destroy();
}




void Renderer::Clear()
{
    // GL 下由 BeginScene 负责清屏，这里保留空实现兼容旧调用。
}




// ----------------------------------------------------------
// 形状
// ----------------------------------------------------------

void Renderer::DrawFilledRoundRect(
    int x, int y, int w, int h,
    int radius,
    Uint8 r, Uint8 g, Uint8 b, Uint8 a
)
{
    DrawRoundedRect(
        (float)x, (float)y, (float)w, (float)h,
        (float)radius,
        r, g, b, a
    );
}




// ----------------------------------------------------------
// 背景与模糊
// ----------------------------------------------------------

void Renderer::DrawBackgroundCover(SDL_Texture* texture)
{
    int tw = 0, th = 0;

    if(!GetTextureSize(texture, tw, th))
        return;

    // 取较大的缩放比，保证两个方向都铺满
    float sx = (float)logicalW / (float)tw;
    float sy = (float)logicalH / (float)th;
    float scale = sx > sy ? sx : sy;

    float dw = tw * scale;
    float dh = th * scale;

    DrawTextureEx(
        texture,
        (logicalW - dw) * 0.5f,
        (logicalH - dh) * 0.5f,
        dw, dh
    );
}




void Renderer::DrawBlurTexture(SDL_Texture* texture, int x, int y)
{
    // GL 下整屏模糊由 EndScene 统一生成，这里退化为直接绘制。
    DrawBackgroundCover(texture);
    (void)x;
    (void)y;
}




void Renderer::DrawFrostedTexture(
    SDL_Texture* texture,
    int x, int y,
    Uint8 overlayAlpha
)
{
    DrawBackgroundCover(texture);
    (void)overlayAlpha;
    (void)x;
    (void)y;
}




void Renderer::DrawFrostedRegionRound(
    SDL_Texture* texture,
    int x, int y, int w, int h,
    int radius,
    Uint8 overlayAlpha
)
{
    int tw = 0, th = 0;

    if(!GetTextureSize(texture, tw, th))
        return;

    // 把屏幕坐标换算成纹理坐标，取对应区域。
    //
    // 注意 V 要翻：GL 的 FBO 纹理原点在左下角，
    // 而我们的屏幕坐标原点在左上角。
    // 不翻的话取到的内容是上下颠倒的（对话框里会看到倒挂的立绘）。
    float u0 = (float)x / (float)logicalW;
    float v1 = (float)y / (float)logicalH;
    float u1 = (float)(x + w) / (float)logicalW;
    float v0 = (float)(y + h) / (float)logicalH;

    if(u0 < 0.0f) u0 = 0.0f;
    if(v0 < 0.0f) v0 = 0.0f;
    if(u1 > 1.0f) u1 = 1.0f;
    if(v1 > 1.0f) v1 = 1.0f;

    if(u1 <= u0 || v1 <= v0)
        return;

    auto it = glTextures.find(texture);

    if(it == glTextures.end() || !it->second.id)
        return;

    PushQuad(
        it->second.id,
        (float)x, (float)y, (float)w, (float)h,
        u0, v0, u1, v1,
        1.0f, 1.0f, 1.0f, 1.0f
    );

    // 圆角遮罩 + 磨砂：用 DrawShape 的圆角裁出边界
    if(radius > 0)
    {
        // 只描边，不填充，所以 alpha 用很小的值
        DrawRoundedRect(
            (float)x, (float)y, (float)w, (float)h,
            (float)radius,
            255, 255, 255, 0
        );
    }

    (void)overlayAlpha;
}




void Renderer::DrawBlurredRegion(
    SDL_Texture* texture,
    int x, int y, int w, int h
)
{
    DrawFrostedRegionRound(texture, x, y, w, h, 0, 0);
}




// ----------------------------------------------------------
// 屏幕快照 / 兼容
// ----------------------------------------------------------

SDL_Renderer* Renderer::GetSDLRenderer()
{
    // GL 模式下没有 SDL_Renderer。
    // 调用方必须判空，纹理加载改走 ResourceManager。
    return nullptr;
}




SDL_Texture* Renderer::GetScreenTexture()
{
    // 返回场景纹理的假句柄，供对话框背景模糊使用。
    static SDL_Texture* sceneHandle = nullptr;

    if(!sceneHandle)
    {
        sceneHandle = nextHandle;
        nextHandle = (SDL_Texture*)((uintptr_t)nextHandle + 0x10);

        TextureEntry entry;
        entry.id = sceneTex;
        entry.width = logicalW;
        entry.height = logicalH;

        glTextures[sceneHandle] = entry;
    }

    // 尺寸可能随分辨率变
    auto it = glTextures.find(sceneHandle);

    if(it != glTextures.end())
    {
        it->second.id = sceneTex;
        it->second.width = logicalW;
        it->second.height = logicalH;
    }

    return sceneHandle;
}




void Renderer::CaptureScreen()
{
    // GL 下场景一直在 sceneTex 里，不需要额外抓取。
}




void Renderer::RestoreMainTarget()
{
    // GL 用 FBO 管理渲染目标，没有 viewport 残留问题。
}


