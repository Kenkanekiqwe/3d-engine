#pragma once
#include <string>
#include <vector>
#include "opengl.hpp"
#include "../math/math.hpp"
namespace cry {
struct MeshPart {
 unsigned vao{},vbo{},albedo{},normal{},roughness{};
 int count{};
 bool transparent{};
};
struct Mesh {
 std::vector<MeshPart> parts;
 bool valid() const { return !parts.empty(); }
 bool textured() const { for(const auto&p:parts) if(p.albedo) return true; return false; }
 void draw() const;
};
Mesh loadObj(const std::string& path);
Mesh loadModel(const std::string& path);
Mesh makeCube();
Mesh makeGround(int,float);
Mesh makePine();
Mesh makeRock();
}