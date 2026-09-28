#pragma once
#include "../math/math.hpp"
#include <string>

namespace cry {

class Shader {
    unsigned int id_{};
    int loc(const char* name) const;

public:
    ~Shader();
    bool create(const std::string& vertexPath, const std::string& fragmentPath);
    void bind() const;
    void setFloat(const char* name, float value) const;
    void setInt(const char* name, int value) const;
    void setVec3(const char* name, Vec3 value) const;
    void setMat4(const char* name, const Mat4& value) const;
};

}