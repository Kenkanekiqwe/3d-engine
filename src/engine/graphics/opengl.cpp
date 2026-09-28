#include "opengl.hpp"

namespace cry::gl {

#define LOAD(name, symbol) do { name = reinterpret_cast<decltype(name)>(glfwGetProcAddress(symbol)); if(!name) return false; } while(false)

CreateShaderProc CreateShader{};
ShaderSourceProc ShaderSource{};
CompileShaderProc CompileShader{};
GetShaderivProc GetShaderiv{};
GetShaderInfoLogProc GetShaderInfoLog{};
DeleteShaderProc DeleteShader{};
CreateProgramProc CreateProgram{};
AttachShaderProc AttachShader{};
LinkProgramProc LinkProgram{};
GetProgramivProc GetProgramiv{};
GetProgramInfoLogProc GetProgramInfoLog{};
DeleteProgramProc DeleteProgram{};
UseProgramProc UseProgram{};
GetUniformLocationProc GetUniformLocation{};
Uniform1fProc Uniform1f{};
Uniform1iProc Uniform1i{};
Uniform3fProc Uniform3f{};
UniformMatrix4fvProc UniformMatrix4fv{};
GenVertexArraysProc GenVertexArrays{};
BindVertexArrayProc BindVertexArray{};
GenBuffersProc GenBuffers{};
BindBufferProc BindBuffer{};
BufferDataProc BufferData{};
VertexAttribPointerProc VertexAttribPointer{};
EnableVertexAttribArrayProc EnableVertexAttribArray{};

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