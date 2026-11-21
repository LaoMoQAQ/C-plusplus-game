#ifndef GL_H
#define GL_H


// ==========================================================
// 极简 OpenGL 函数加载器
// ==========================================================
//
// 不依赖 GLEW / GLAD / glad2，也不需要链接 opengl32 / libGL：
// 所有 GL 入口点都在运行时用 SDL_GL_GetProcAddress() 取地址。
//
// 这样做的好处：
//   1. 构建依赖和改造前完全一样，只多了一个 .cpp；
//   2. Windows(MSYS2/MinGW) 的 <GL/gl.h> 只有 1.1，
//      自带加载器能让同一份代码在 Windows / Linux / macOS 都能跑；
//   3. 不会和系统 GL 头文件里的原型冲突（这里不包含它们）。
//
// 这里只声明项目真正用到的那部分 GL 3.3 子集。
// ----------------------------------------------------------


#include <cstddef>



// ---------------- 基础类型 ----------------

typedef unsigned int   GLenum;
typedef unsigned char  GLboolean;
typedef unsigned int   GLbitfield;
typedef signed char    GLbyte;
typedef short          GLshort;
typedef int            GLint;
typedef int            GLsizei;
typedef unsigned char  GLubyte;
typedef unsigned short GLushort;
typedef unsigned int   GLuint;
typedef float          GLfloat;
typedef float          GLclampf;
typedef double         GLdouble;
typedef char           GLchar;
typedef ptrdiff_t      GLintptr;
typedef ptrdiff_t      GLsizeiptr;



// ---------------- 常量 ----------------

#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_NO_ERROR                       0

#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005

#define GL_UNSIGNED_BYTE                  0x1401
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406

#define GL_BLEND                          0x0BE2
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301

#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_CLAMP_TO_EDGE                  0x812F

#define GL_RED                            0x1903
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_RGBA8                          0x8058
#define GL_BGRA                           0x80E1

#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_UNPACK_ROW_LENGTH              0x0CF2

#define GL_COLOR_BUFFER_BIT               0x00004000

#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03
#define GL_MAX_TEXTURE_SIZE               0x0D33

#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_STREAM_DRAW                    0x88E0

#define GL_FRAMEBUFFER                    0x8D40
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5

#define GL_SCISSOR_TEST                   0x0C11
#define GL_DEPTH_TEST                     0x0B71
#define GL_CULL_FACE                      0x0B44

#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506



// ---------------- 入口点 ----------------

#define GL_DECLARE(ret, name, params)  typedef ret (*PFN_##name) params; extern PFN_##name name;


// 状态 / 查询
GL_DECLARE(const GLubyte*, glGetString, (GLenum name) )
GL_DECLARE(GLenum,glGetError,         (void) )
GL_DECLARE(void,  glGetIntegerv,      (GLenum pname, GLint* data) )
GL_DECLARE(void,  glViewport,         (GLint x, GLint y, GLsizei w, GLsizei h) )
GL_DECLARE(void,  glScissor,          (GLint x, GLint y, GLsizei w, GLsizei h) )
GL_DECLARE(void,  glEnable,           (GLenum cap) )
GL_DECLARE(void,  glDisable,          (GLenum cap) )
GL_DECLARE(void,  glBlendFunc,        (GLenum sfactor, GLenum dfactor) )
GL_DECLARE(void,  glBlendFuncSeparate,(GLenum sr, GLenum dr, GLenum sa, GLenum da) )
GL_DECLARE(void,  glClearColor,       (GLfloat r, GLfloat g, GLfloat b, GLfloat a) )
GL_DECLARE(void,  glClear,            (GLbitfield mask) )
GL_DECLARE(void,  glPixelStorei,      (GLenum pname, GLint param) )
GL_DECLARE(void,  glFinish,           (void) )

// 纹理
GL_DECLARE(void,  glGenTextures,      (GLsizei n, GLuint* textures) )
GL_DECLARE(void,  glDeleteTextures,   (GLsizei n, const GLuint* textures) )
GL_DECLARE(void,  glBindTexture,      (GLenum target, GLuint texture) )
GL_DECLARE(void,  glTexImage2D,       (GLenum target, GLint level, GLint internalformat, GLsizei w, GLsizei h, GLint border, GLenum format, GLenum type, const void* pixels) )
GL_DECLARE(void,  glTexSubImage2D,    (GLenum target, GLint level, GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels) )
GL_DECLARE(void,  glTexParameteri,    (GLenum target, GLenum pname, GLint param) )
GL_DECLARE(void,  glActiveTexture,    (GLenum texture) )

// 缓冲 / 顶点数组
GL_DECLARE(void,  glGenBuffers,       (GLsizei n, GLuint* buffers) )
GL_DECLARE(void,  glDeleteBuffers,    (GLsizei n, const GLuint* buffers) )
GL_DECLARE(void,  glBindBuffer,       (GLenum target, GLuint buffer) )
GL_DECLARE(void,  glBufferData,       (GLenum target, GLsizeiptr size, const void* data, GLenum usage) )
GL_DECLARE(void,  glBufferSubData,    (GLenum target, GLintptr offset, GLsizeiptr size, const void* data) )
GL_DECLARE(void,  glGenVertexArrays,  (GLsizei n, GLuint* arrays) )
GL_DECLARE(void,  glDeleteVertexArrays,(GLsizei n, const GLuint* arrays) )
GL_DECLARE(void,  glBindVertexArray,  (GLuint array) )
GL_DECLARE(void,  glEnableVertexAttribArray, (GLuint index) )
GL_DECLARE(void,  glDisableVertexAttribArray,(GLuint index) )
GL_DECLARE(void,  glVertexAttribPointer,(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) )

// 着色器 / 程序
GL_DECLARE(GLuint,glCreateShader,     (GLenum type) )
GL_DECLARE(void,  glShaderSource,     (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) )
GL_DECLARE(void,  glCompileShader,    (GLuint shader) )
GL_DECLARE(void,  glGetShaderiv,      (GLuint shader, GLenum pname, GLint* params) )
GL_DECLARE(void,  glGetShaderInfoLog, (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog) )
GL_DECLARE(void,  glDeleteShader,     (GLuint shader) )
GL_DECLARE(GLuint,glCreateProgram,    (void) )
GL_DECLARE(void,  glAttachShader,     (GLuint program, GLuint shader) )
GL_DECLARE(void,  glBindAttribLocation,(GLuint program, GLuint index, const GLchar* name) )
GL_DECLARE(void,  glLinkProgram,      (GLuint program) )
GL_DECLARE(void,  glGetProgramiv,     (GLuint program, GLenum pname, GLint* params) )
GL_DECLARE(void,  glGetProgramInfoLog,(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog) )
GL_DECLARE(void,  glUseProgram,       (GLuint program) )
GL_DECLARE(void,  glDeleteProgram,    (GLuint program) )
GL_DECLARE(GLint, glGetUniformLocation,(GLuint program, const GLchar* name) )
GL_DECLARE(void,  glUniform1i,        (GLint location, GLint v0) )
GL_DECLARE(void,  glUniform1f,        (GLint location, GLfloat v0) )
GL_DECLARE(void,  glUniform2f,        (GLint location, GLfloat v0, GLfloat v1) )
GL_DECLARE(void,  glUniform3f,        (GLint location, GLfloat v0, GLfloat v1, GLfloat v2) )
GL_DECLARE(void,  glUniform4f,        (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) )

// 绘制
GL_DECLARE(void,  glDrawArrays,       (GLenum mode, GLint first, GLsizei count) )

// 离屏渲染（毛玻璃需要）
GL_DECLARE(void,  glGenFramebuffers,  (GLsizei n, GLuint* framebuffers) )
GL_DECLARE(void,  glDeleteFramebuffers,(GLsizei n, const GLuint* framebuffers) )
GL_DECLARE(void,  glBindFramebuffer,  (GLenum target, GLuint framebuffer) )
GL_DECLARE(void,  glFramebufferTexture2D,(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) )
GL_DECLARE(GLenum,glCheckFramebufferStatus,(GLenum target) )

// 截图用（开发期自检）
GL_DECLARE(void,  glReadPixels,       (GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, void* pixels) )


#undef GL_DECLARE



// ==========================================================
// 加载全部入口点。
//
// 必须在 SDL_GL_CreateContext() 成功之后调用
// （SDL_GL_GetProcAddress 需要已经加载好 GL 库）。
//
// 返回 false 表示某个必需入口点没取到，调用方应终止。
// ==========================================================

bool GL_LoadFunctions();



#endif
