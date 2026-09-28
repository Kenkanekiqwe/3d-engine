#include "opengl.hpp"
namespace cry::gl {
CreateShaderProc CreateShader=nullptr; ShaderSourceProc ShaderSource=nullptr; CompileShaderProc CompileShader=nullptr; GetShaderivProc GetShaderiv=nullptr; GetShaderInfoLogProc GetShaderInfoLog=nullptr; DeleteShaderProc DeleteShader=nullptr;
CreateProgramProc CreateProgram=nullptr; AttachShaderProc AttachShader=nullptr; LinkProgramProc LinkProgram=nullptr; GetProgramivProc GetProgramiv=nullptr; GetProgramInfoLogProc GetProgramInfoLog=nullptr; DeleteProgramProc DeleteProgram=nullptr;
UseProgramProc UseProgram=nullptr; GetUniformLocationProc GetUniformLocation=nullptr; Uniform1fProc Uniform1f=nullptr; Uniform1iProc Uniform1i=nullptr; Uniform3fProc Uniform3f=nullptr; UniformMatrix4fvProc UniformMatrix4fv=nullptr;
GenVertexArraysProc GenVertexArrays=nullptr; BindVertexArrayProc BindVertexArray=nullptr; GenBuffersProc GenBuffers=nullptr; BindBufferProc BindBuffer=nullptr; BufferDataProc BufferData=nullptr; VertexAttribPointerProc VertexAttribPointer=nullptr; EnableVertexAttribArrayProc EnableVertexAttribArray=nullptr;
GenTexturesProc GenTextures=nullptr; BindTextureProc BindTexture=nullptr; TexParameteriProc TexParameteri=nullptr; TexImage2DProc TexImage2D=nullptr; GenerateMipmapProc GenerateMipmap=nullptr; ActiveTextureProc ActiveTexture=nullptr;
bool load(){
#define LOAD(n,s) do{n=reinterpret_cast<decltype(n)>(glfwGetProcAddress(s));if(!n)return false;}while(false)
LOAD(CreateShader,"glCreateShader");LOAD(ShaderSource,"glShaderSource");LOAD(CompileShader,"glCompileShader");LOAD(GetShaderiv,"glGetShaderiv");LOAD(GetShaderInfoLog,"glGetShaderInfoLog");LOAD(DeleteShader,"glDeleteShader");
LOAD(CreateProgram,"glCreateProgram");LOAD(AttachShader,"glAttachShader");LOAD(LinkProgram,"glLinkProgram");LOAD(GetProgramiv,"glGetProgramiv");LOAD(GetProgramInfoLog,"glGetProgramInfoLog");LOAD(DeleteProgram,"glDeleteProgram");
LOAD(UseProgram,"glUseProgram");LOAD(GetUniformLocation,"glGetUniformLocation");LOAD(Uniform1f,"glUniform1f");LOAD(Uniform1i,"glUniform1i");LOAD(Uniform3f,"glUniform3f");LOAD(UniformMatrix4fv,"glUniformMatrix4fv");
LOAD(GenVertexArrays,"glGenVertexArrays");LOAD(BindVertexArray,"glBindVertexArray");LOAD(GenBuffers,"glGenBuffers");LOAD(BindBuffer,"glBindBuffer");LOAD(BufferData,"glBufferData");LOAD(VertexAttribPointer,"glVertexAttribPointer");LOAD(EnableVertexAttribArray,"glEnableVertexAttribArray");
LOAD(GenTextures,"glGenTextures");LOAD(BindTexture,"glBindTexture");LOAD(TexParameteri,"glTexParameteri");LOAD(TexImage2D,"glTexImage2D");LOAD(GenerateMipmap,"glGenerateMipmap");LOAD(ActiveTexture,"glActiveTexture");
#undef LOAD
return true;}
}