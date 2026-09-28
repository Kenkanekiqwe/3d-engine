#pragma once
#include <string>
#include <vector>
#include "opengl.hpp"
#include "../math/math.hpp"

namespace cry {

struct Mesh {
    unsigned vao{}, vbo{};
    int count{};
    Mesh() = default;
    explicit Mesh(const std::vector<float>&);
    void draw() const;
    bool valid() const { return vao != 0 && count > 0; }
};

Mesh loadObj(const std::string& path);
Mesh makeCube();
Mesh makeGround(int,float);
Mesh makePine();
Mesh makeRock();

}
