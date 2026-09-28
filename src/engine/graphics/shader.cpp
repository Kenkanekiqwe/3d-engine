#include "shader.hpp"
#include "opengl.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace cry {

static std::string readText(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if(!file) return {};
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static unsigned compile(unsigned type, const std::string& source) {
    const unsigned id = gl::CreateShader(type);
    const char* src = source.c_str();
    gl::ShaderSource(id, 1, &src, nullptr);
    gl::CompileShader(id);

    int ok=0;
    gl::GetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        int len=0;
        gl::GetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(static_cast<size_t>(len)+1);
        gl::GetShaderInfoLog(id, len, nullptr, log.data());
        std::cerr << "Shader compile error:\n" << log.data() << '\n';
        gl::DeleteShader(id);
        return 0;
    }
    return id;
}

Shader::~Shader() {
    if(id_) gl::DeleteProgram(id_);
}

bool Shader::create(const std::string& vertexPath, const std::string& fragmentPath) {
    const auto vs=readText(vertexPath);
    const auto fs=readText(fragmentPath);
    if(vs.empty() || fs.empty()) {
        std::cerr << "Unable to read shader files.\n";
        return false;
    }

    const unsigned v=compile(GL_VERTEX_SHADER,vs);
    const unsigned f=compile(GL_FRAGMENT_SHADER,fs);
    if(!v || !f) return false;

    id_=gl::CreateProgram();
    gl::AttachShader(id_,v);
    gl::AttachShader(id_,f);
    gl::LinkProgram(id_);

    int ok=0;
    gl::GetProgramiv(id_,GL_LINK_STATUS,&ok);
    gl::DeleteShader(v);
    gl::DeleteShader(f);

    if(!ok) {
        int len=0;
        gl::GetProgramiv(id_,GL_INFO_LOG_LENGTH,&len);
        std::vector<char> log(static_cast<size_t>(len)+1);
        gl::GetProgramInfoLog(id_,len,nullptr,log.data());
        std::cerr << "Program link error:\n" << log.data() << '\n';
        gl::DeleteProgram(id_);
        id_=0;
        return false;
    }
    return true;
}

void Shader::bind() const { gl::UseProgram(id_); }

int Shader::loc(const char* name) const {
    return gl::GetUniformLocation(id_,name);
}

void Shader::setFloat(const char* name,float value) const { gl::Uniform1f(loc(name),value); }
void Shader::setInt(const char* name,int value) const { gl::Uniform1i(loc(name),value); }
void Shader::setVec3(const char* name,Vec3 v) const { gl::Uniform3f(loc(name),v.x,v.y,v.z); }
void Shader::setMat4(const char* name,const Mat4& v) const { gl::UniformMatrix4fv(loc(name),1,GL_FALSE,v.m); }

}