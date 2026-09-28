#include "scene.hpp"
namespace cry {
bool Scene::create(){cube_=makeCube();ground_=makeGround(36,44);pine_=makePine();rock_=makeRock();lights_={{{-9,11,7},{.28f,.40f,.62f},42,2.2f},{{-4,3,-15},{1,.26f,.075f},10,5.5f}};return true;}
void Scene::draw(Shader&s,const Mesh&m,const Mat4&model,const Vec3&color,const Mat4&vp,const Vec3&cam,const Vec3&fp,const Vec3&fd,bool on,float time,int mat){
s.bind();s.setMat4("uModel",model);s.setMat4("uMVP",vp*model);s.setVec3("uColor",color);s.setVec3("uCamera",cam);s.setVec3("uFlashPos",fp);s.setVec3("uFlashDir",fd);s.setFloat("uFlashOn",on?1.f:0.f);s.setFloat("uTime",time);s.setInt("uMaterial",mat);
for(int i=0;i<2;i++){s.setVec3(i?"uLight1Pos":"uLight0Pos",lights_[i].position);s.setVec3(i?"uLight1Color":"uLight0Color",lights_[i].color);s.setFloat(i?"uLight1Radius":"uLight0Radius",lights_[i].radius);s.setFloat(i?"uLight1Intensity":"uLight0Intensity",lights_[i].intensity);}m.draw();}
void Scene::render(Shader&s,const Mat4&vp,const Vec3&cam,const Vec3&fp,const Vec3&fd,bool on,float time){
draw(s,ground_,Mat4::identity(),{.075f,.082f,.062f},vp,cam,fp,fd,on,time,0);
draw(s,cube_,Mat4::translation({0,-.12f,-4})*Mat4::scale({3.8f,.04f,35}),{.10f,.075f,.048f},vp,cam,fp,fd,on,time,0);
const float trees[][3]={{-13,15,1.55f},{-9,17,1.15f},{-4,19,1.7f},{2,18,1.3f},{8,17,1.8f},{14,14,1.45f},{-16,9,1.6f},{-12,5,1.15f},{12,6,1.5f},{16,2,1.8f},{-16,-4,1.55f},{14,-5,1.35f},{-15,-12,1.8f},{-9,-16,1.45f},{4,-18,1.75f},{13,-15,1.5f}};
for(auto&t:trees)draw(s,pine_,Mat4::translation({t[0],0,t[1]})*Mat4::rotationY(t[0]*.13f+sin(time*.7f+t[0])*.015f)*Mat4::scale({t[2],t[2],t[2]}),{.026f,.070f,.038f},vp,cam,fp,fd,on,time,2);
const float rocks[][3]={{-3.5f,.45f,-2},{3.5f,.42f,-6},{-5.5f,.35f,4},{6,.28f,7}};
for(auto&r:rocks)draw(s,rock_,Mat4::translation({r[0],r[1],r[2]})*Mat4::scale({1.2f,.65f,.9f}),{.09f,.095f,.085f},vp,cam,fp,fd,on,time,3);
draw(s,cube_,Mat4::translation({-4,1.65f,-16})*Mat4::scale({6.4f,3.3f,4.2f}),{.085f,.058f,.038f},vp,cam,fp,fd,on,time,4);
draw(s,cube_,Mat4::translation({-4,3.62f,-16})*Mat4::rotationY(.785f)*Mat4::scale({5.1f,.48f,5.1f}),{.025f,.028f,.027f},vp,cam,fp,fd,on,time,5);
draw(s,cube_,Mat4::translation({-4,1.3f,-13.83f})*Mat4::scale({1.25f,2.5f,.12f}),{.035f,.024f,.018f},vp,cam,fp,fd,on,time,4);
for(float x:{-5.7f,-2.3f})draw(s,cube_,Mat4::translation({x,1.85f,-13.84f})*Mat4::scale({1.3f,1.15f,.08f}),{.95f,.28f,.055f},vp,cam,fp,fd,on,time,6);
for(int i=0;i<7;i++)draw(s,cube_,Mat4::translation({-3.2f+i*.9f,.65f,-10.2f})*Mat4::rotationY((i%2?-1:1)*.22f)*Mat4::scale({.12f,1.3f,.12f}),{.055f,.034f,.02f},vp,cam,fp,fd,on,time,4);
float sx=.65f+sin(time*.17f)*.12f;draw(s,cube_,Mat4::translation({sx,1.5f,-11})*Mat4::scale({.42f,2.8f,.32f}),{.002f,.002f,.002f},vp,cam,fp,fd,on,time,8);
draw(s,cube_,Mat4::translation({sx,3.28f,-11})*Mat4::scale({.55f,.58f,.45f}),{.002f,.002f,.002f},vp,cam,fp,fd,on,time,8);
}
}
