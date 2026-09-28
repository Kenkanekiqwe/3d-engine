#include "opengl.hpp"

namespace cry::gl {

#define LOAD(name, symbol) do { name = reinterpret_cast<decltype(name)>(glfwGetProcAddress(symbol)); if(!name) return false; } while(false)

PFNGLCREATESHADERPROC CreateShader{};
PFNGLSHADERSOURCEPROC ShaderSource{};
PFNGLCOMPILESHADERPROC CompileShader{};
PFNGLGETSHADERIVPROC GetShaderiv{};
PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog{};
PFNGLDELETESHADERPROC DeleteShader{};
PFNGLCREATEPROGRAMPROC CreateProgram{};
PFNGLATTACHSHADERPROC AttachShader{};
PFNGLLINKPROGRAMPROC LinkProgram{};
PFNGLGETPROGRAMIVPROC GetProgramiv{};
PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog{};
PFNGLDELETEPROGRAMPROC DeleteProgram{};
PFNGLUSEPROGRAMPROC UseProgram{};
PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation{};
PFNGLUNIFORM1FPROC Uniform1f{};
PFNGLUNIFORM1IPROC Uniform1i{};
PFNGLUNIFORM3FPROC Uniform3f{};
PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv{};
PFNGLGENVERTEXARRAYSPROC GenVertexArrays{};
PFNGLBINDVERTEXARRAYPROC BindVertexArray{};
PFNGLGENBUFFERSPROC GenBuffers{};
PFNGLBINDBUFFERPROC BindBuffer{};
PFNGLBUFFERDATAPROC BufferData{};
PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer{};
PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray{};

bool load() {
    LOAD(CreateShader, "glCreateShader");
    LOAD(ShaderSource, "glShaderSource");
    LOAD(CompileShader, "glCompileShader");
    LOAD(GetShaderiv, "glGetShaderiv");
    LOAD(GetShaderInfoLog, "glGetShaderInfoLog");
    LOAD(DeleteShader, "glDeleteShader");
    LOAD(CreateProgram, "glCreateProgram");
    LOAD(AttachShader, "glAttachShader");
    LOAD(LinkProgram, "glLinkProgram");
    LOAD(GetProgramiv, "glGetProgramiv");
    LOAD(GetProgramInfoLog, "glGetProgramInfoLog");
    LOAD(DeleteProgram, "glDeleteProgram");
    LOAD(UseProgram, "glUseProgram");
    LOAD(GetUniformLocation, "glGetUniformLocation");
    LOAD(Uniform1f, "glUniform1f");
    LOAD(Uniform1i, "glUniform1i");
    LOAD(Uniform3f, "glUniform3f");
    LOAD(UniformMatrix4fv, "glUniformMatrix4fv");
    LOAD(GenVertexArrays, "glGenVertexArrays");
    LOAD(BindVertexArray, "glBindVertexArray");
    LOAD(GenBuffers, "glGenBuffers");
    LOAD(BindBuffer, "glBindBuffer");
    LOAD(BufferData, "glBufferData");
    LOAD(VertexAttribPointer, "glVertexAttribPointer");
    LOAD(EnableVertexAttribArray, "glEnableVertexAttribArray");
    return true;
}

}