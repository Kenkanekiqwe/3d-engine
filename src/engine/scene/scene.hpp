#pragma once
#include <vector>
#include "../math/math.hpp"
#include "../graphics/mesh.hpp"
#include "../graphics/shader.hpp"
namespace cry {
struct Light{Vec3 position,color;float radius,intensity;};
class Scene{
 Mesh cube_,ground_,pine_,rock_,realHouse_;
 std::vector<Mesh> realTrees_;
 bool hasRealHouse_{false};
 std::vector<Light> lights_;
 void draw(Shader&,const Mesh&,const Mat4&,const Vec3&,const Mat4&,const Vec3&,const Vec3&,const Vec3&,bool,float,int);
public:
 bool create();
 void render(Shader&,const Mat4&,const Vec3&,const Vec3&,const Vec3&,bool,float);
};
}