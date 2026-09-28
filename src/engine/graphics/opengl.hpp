#pragma once
#include <GLFW/glfw3.h>
#include <cstddef>

#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW 0x88E4
#endif
#ifndef GL_FLOAT
#define GL_FLOAT 0x1406
#endif
#ifndef GL_FALSE
#define GL_FALSE 0
#endif
#ifndef GL_TRUE
#define GL_TRUE 1
#endif
#ifndef GL_TRIANGLES
#define GL_TRIANGLES 0x0004
#endif
#ifndef GL_COLOR_BUFFER_BIT
#define GL_COLOR_BUFFER_BIT 0x00004000
#endif
#ifndef GL_DEPTH_BUFFER_BIT
#define GL_DEPTH_BUFFER_BIT 0x00000100
#endif
#ifndef GL_DEPTH_TEST
#define GL_DEPTH_TEST 0x0B71
#endif
#ifndef GL_CULL_FACE
#define GL_CULL_FACE 0x0B44
#endif
#ifndef GL_BACK
#define GL_BACK 0x0405
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS 0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS 0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

namespace cry::gl {

using CreateShaderProc = unsigned int (*)(unsigned int);
using ShaderSourceProc = void (*)(unsigned int, int, const char* const*, const int*);
using CompileShaderProc = void (*)(unsigned int);
using GetShaderivProc = void (*)(unsigned int, unsigned int, int*);
using GetShaderInfoLogProc = void (*)(unsigned int, int, int*, char*);
using DeleteShaderProc = void (*)(unsigned int);
using CreateProgramProc = unsigned int (*)();
using AttachShaderProc = void (*)(unsigned int, unsigned int);
using LinkProgramProc = void (*)(unsigned int);
using GetProgramivProc = void (*)(unsigned int, unsigned int, int*);
using GetProgramInfoLogProc = void (*)(unsigned int, int, int*, char*);
using DeleteProgramProc = void (*)(unsigned int);
using UseProgramProc = void (*)(unsigned int);
using GetUniformLocationProc = int (*)(unsigned int, const char*);
using Uniform1fProc = void (*)(int, float);
using Uniform1iProc = void (*)(int, int);
using Uniform3fProc = void (*)(int, float, float, float);
using UniformMatrix4fvProc = void (*)(int, int, unsigned char, const float*);
using GenVertexArraysProc = void (*)(int, unsigned int*);
using BindVertexArrayProc = void (*)(unsigned int);
using GenBuffersProc = void (*)(int, unsigned int*);
using BindBufferProc = void (*)(unsigned int, unsigned int);
using BufferDataProc = void (*)(unsigned int, std::ptrdiff_t, const void*, unsigned int);
using VertexAttribPointerProc = void (*)(unsigned int, int, unsigned int, unsigned char, int, const void*);
using EnableVertexAttribArrayProc = void (*)(unsigned int);

extern CreateShaderProc CreateShader;
extern ShaderSourceProc ShaderSource;
extern CompileShaderProc CompileShader;
extern GetShaderivProc GetShaderiv;
extern GetShaderInfoLogProc GetShaderInfoLog;
extern DeleteShaderProc DeleteShader;
extern CreateProgramProc CreateProgram;
extern AttachShaderProc AttachShader;
extern LinkProgramProc LinkProgram;
extern GetProgramivProc GetProgramiv;
extern GetProgramInfoLogProc GetProgramInfoLog;
extern DeleteProgramProc DeleteProgram;
extern UseProgramProc UseProgram;
extern GetUniformLocationProc GetUniformLocation;
extern Uniform1fProc Uniform1f;
extern Uniform1iProc Uniform1i;
extern Uniform3fProc Uniform3f;
extern UniformMatrix4fvProc UniformMatrix4fv;
extern GenVertexArraysProc GenVertexArrays;
extern BindVertexArrayProc BindVertexArray;
extern GenBuffersProc GenBuffers;
extern BindBufferProc BindBuffer;
extern BufferDataProc BufferData;
extern VertexAttribPointerProc VertexAttribPointer;
extern EnableVertexAttribArrayProc EnableVertexAttribArray;

bool load();

}