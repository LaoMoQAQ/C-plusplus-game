#include "GL.h"

#include <SDL.h>

#include <iostream>



// ==========================================================
// 定义所有入口点指针
// ==========================================================

#define GL_DEFINE(ret, name, params)  PFN_##name name = nullptr;

GL_DEFINE(void,  glGetString,        (GLenum name) )
GL_DEFINE(GLenum,glGetError,         (void) )
GL_DEFINE(void,  glGetIntegerv,      (GLenum pname, GLint* data) )
GL_DEFINE(void,  glViewport,         (GLint x, GLint y, GLsizei w, GLsizei h) )
GL_DEFINE(void,  glScissor,          (GLint x, GLint y, GLsizei w, GLsizei h) )
GL_DEFINE(void,  glEnable,           (GLenum cap) )
GL_DEFINE(void,  glDisable,          (GLenum cap) )
GL_DEFINE(void,  glBlendFunc,        (GLenum sfactor, GLenum dfactor) )
GL_DEFINE(void,  glBlendFuncSeparate,(GLenum sr, GLenum dr, GLenum sa, GLenum da) )
GL_DEFINE(void,  glClearColor,       (GLfloat r, GLfloat g, GLfloat b, GLfloat a) )
GL_DEFINE(void,  glClear,            (GLbitfield mask) )
GL_DEFINE(void,  glPixelStorei,      (GLenum pname, GLint param) )
GL_DEFINE(void,  glFinish,           (void) )

GL_DEFINE(void,  glGenTextures,      (GLsizei n, GLuint* textures) )
GL_DEFINE(void,  glDeleteTextures,   (GLsizei n, const GLuint* textures) )
GL_DEFINE(void,  glBindTexture,      (GLenum target, GLuint texture) )
GL_DEFINE(void,  glTexImage2D,       (GLenum target, GLint level, GLint internalformat, GLsizei w, GLsizei h, GLint border, GLenum format, GLenum type, const void* pixels) )
GL_DEFINE(void,  glTexSubImage2D,    (GLenum target, GLint level, GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, const void* pixels) )
GL_DEFINE(void,  glTexParameteri,    (GLenum target, GLenum pname, GLint param) )
GL_DEFINE(void,  glActiveTexture,    (GLenum texture) )

GL_DEFINE(void,  glGenBuffers,       (GLsizei n, GLuint* buffers) )
GL_DEFINE(void,  glDeleteBuffers,    (GLsizei n, const GLuint* buffers) )
GL_DEFINE(void,  glBindBuffer,       (GLenum target, GLuint buffer) )
GL_DEFINE(void,  glBufferData,       (GLenum target, GLsizeiptr size, const void* data, GLenum usage) )
GL_DEFINE(void,  glBufferSubData,    (GLenum target, GLintptr offset, GLsizeiptr size, const void* data) )
GL_DEFINE(void,  glGenVertexArrays,  (GLsizei n, GLuint* arrays) )
GL_DEFINE(void,  glDeleteVertexArrays,(GLsizei n, const GLuint* arrays) )
GL_DEFINE(void,  glBindVertexArray,  (GLuint array) )
GL_DEFINE(void,  glEnableVertexAttribArray, (GLuint index) )
GL_DEFINE(void,  glDisableVertexAttribArray,(GLuint index) )
GL_DEFINE(void,  glVertexAttribPointer,(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) )

GL_DEFINE(GLuint,glCreateShader,     (GLenum type) )
GL_DEFINE(void,  glShaderSource,     (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) )
GL_DEFINE(void,  glCompileShader,    (GLuint shader) )
GL_DEFINE(void,  glGetShaderiv,      (GLuint shader, GLenum pname, GLint* params) )
GL_DEFINE(void,  glGetShaderInfoLog, (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog) )
GL_DEFINE(void,  glDeleteShader,     (GLuint shader) )
GL_DEFINE(GLuint,glCreateProgram,    (void) )
GL_DEFINE(void,  glAttachShader,     (GLuint program, GLuint shader) )
GL_DEFINE(void,  glBindAttribLocation,(GLuint program, GLuint index, const GLchar* name) )
GL_DEFINE(void,  glLinkProgram,      (GLuint program) )
GL_DEFINE(void,  glGetProgramiv,     (GLuint program, GLenum pname, GLint* params) )
GL_DEFINE(void,  glGetProgramInfoLog,(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog) )
GL_DEFINE(void,  glUseProgram,       (GLuint program) )
GL_DEFINE(void,  glDeleteProgram,    (GLuint program) )
GL_DEFINE(GLint, glGetUniformLocation,(GLuint program, const GLchar* name) )
GL_DEFINE(void,  glUniform1i,        (GLint location, GLint v0) )
GL_DEFINE(void,  glUniform1f,        (GLint location, GLfloat v0) )
GL_DEFINE(void,  glUniform2f,        (GLint location, GLfloat v0, GLfloat v1) )
GL_DEFINE(void,  glUniform3f,        (GLint location, GLfloat v0, GLfloat v1, GLfloat v2) )
GL_DEFINE(void,  glUniform4f,        (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) )

GL_DEFINE(void,  glDrawArrays,       (GLenum mode, GLint first, GLsizei count) )

GL_DEFINE(void,  glGenFramebuffers,  (GLsizei n, GLuint* framebuffers) )
GL_DEFINE(void,  glDeleteFramebuffers,(GLsizei n, const GLuint* framebuffers) )
GL_DEFINE(void,  glBindFramebuffer,  (GLenum target, GLuint framebuffer) )
GL_DEFINE(void,  glFramebufferTexture2D,(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) )
GL_DEFINE(GLenum,glCheckFramebufferStatus,(GLenum target) )

GL_DEFINE(void,  glReadPixels,       (GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, void* pixels) )

#undef GL_DEFINE



namespace
{
    int g_missingCount = 0;
}



static void* GL_Proc(const char* name)
{
    return SDL_GL_GetProcAddress(name);
}



// 取不到就记一笔。核心函数缺失说明驱动太老或不是 GL 3.x 上下文。
#define GL_LOAD(name)                                                  \
    do                                                                 \
    {                                                                  \
        name = (PFN_##name)GL_Proc(#name);                             \
        if(!name)                                                      \
        {                                                              \
            std::cout << "OpenGL 入口点缺失: " #name << std::endl;     \
            ++g_missingCount;                                          \
        }                                                              \
    } while(0)



bool GL_LoadFunctions()
{
    g_missingCount = 0;

    GL_LOAD(glGetString);
    GL_LOAD(glGetError);
    GL_LOAD(glGetIntegerv);
    GL_LOAD(glViewport);
    GL_LOAD(glScissor);
    GL_LOAD(glEnable);
    GL_LOAD(glDisable);
    GL_LOAD(glBlendFunc);
    GL_LOAD(glBlendFuncSeparate);
    GL_LOAD(glClearColor);
    GL_LOAD(glClear);
    GL_LOAD(glPixelStorei);
    GL_LOAD(glFinish);

    GL_LOAD(glGenTextures);
    GL_LOAD(glDeleteTextures);
    GL_LOAD(glBindTexture);
    GL_LOAD(glTexImage2D);
    GL_LOAD(glTexSubImage2D);
    GL_LOAD(glTexParameteri);
    GL_LOAD(glActiveTexture);

    GL_LOAD(glGenBuffers);
    GL_LOAD(glDeleteBuffers);
    GL_LOAD(glBindBuffer);
    GL_LOAD(glBufferData);
    GL_LOAD(glBufferSubData);
    GL_LOAD(glGenVertexArrays);
    GL_LOAD(glDeleteVertexArrays);
    GL_LOAD(glBindVertexArray);
    GL_LOAD(glEnableVertexAttribArray);
    GL_LOAD(glDisableVertexAttribArray);
    GL_LOAD(glVertexAttribPointer);

    GL_LOAD(glCreateShader);
    GL_LOAD(glShaderSource);
    GL_LOAD(glCompileShader);
    GL_LOAD(glGetShaderiv);
    GL_LOAD(glGetShaderInfoLog);
    GL_LOAD(glDeleteShader);
    GL_LOAD(glCreateProgram);
    GL_LOAD(glAttachShader);
    GL_LOAD(glBindAttribLocation);
    GL_LOAD(glLinkProgram);
    GL_LOAD(glGetProgramiv);
    GL_LOAD(glGetProgramInfoLog);
    GL_LOAD(glUseProgram);
    GL_LOAD(glDeleteProgram);
    GL_LOAD(glGetUniformLocation);
    GL_LOAD(glUniform1i);
    GL_LOAD(glUniform1f);
    GL_LOAD(glUniform2f);
    GL_LOAD(glUniform3f);
    GL_LOAD(glUniform4f);

    GL_LOAD(glDrawArrays);

    GL_LOAD(glGenFramebuffers);
    GL_LOAD(glDeleteFramebuffers);
    GL_LOAD(glBindFramebuffer);
    GL_LOAD(glFramebufferTexture2D);
    GL_LOAD(glCheckFramebufferStatus);

    GL_LOAD(glReadPixels);

    return g_missingCount == 0;
}

#undef GL_LOAD
