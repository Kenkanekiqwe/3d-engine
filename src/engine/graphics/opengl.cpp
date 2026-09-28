#include "opengl.hpp"
#include <iostream>

namespace cry::gl {

#define LOAD(name) do { name = reinterpret_cast<decltype(name)>(glfwGetProcAddress(#name)); if(!name) return false; } while(false)

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
    LOAD(CreateShader); LOAD(ShaderSource); LOAD(CompileShader);
    LOAD(GetShaderiv); LOAD(GetShaderInfoLog); LOAD(DeleteShader);
    LOAD(CreateProgram); LOAD(AttachShader); LOAD(LinkProgram);
    LOAD(GetProgramiv); LOAD(GetProgramInfoLog); LOAD(DeleteProgram);
    LOAD(UseProgram); LOAD(GetUniformLocation); LOAD(Uniform1f);
    LOAD(Uniform1i); LOAD(Uniform3f); LOAD(UniformMatrix4fv);
    LOAD(GenVertexArrays); LOAD(BindVertexArray); LOAD(GenBuffers);
    LOAD(BindBuffer); LOAD(BufferData); LOAD(VertexAttribPointer);
    LOAD(EnableVertexAttribArray);
    return true;
}

}