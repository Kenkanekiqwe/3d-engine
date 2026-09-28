#pragma once
#include <vector>
#include "opengl.hpp"
#include "../math/math.hpp"
namespace cry {
struct Mesh {
    unsigned vao{},vbo{}; int count{};
    Mesh()=default; explicit Mesh(const std::vector<float>&);
    void draw()const;
};
Mesh makeCube(); Mesh makeGround(int,float); Mesh makePine(); Mesh makeRock();
}