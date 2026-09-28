#include "scene.hpp"
#include <cmath>
namespace cry {
bool Scene::create(){
 cube_=makeCube();ground_=makeGround(64,64);pine_=makePine();rock_=makeRock();
 realHouse_=loadObj("assets/level/house.obj");hasRealHouse_=realHouse_.valid();
 realTrees_.clear();
 for(int i=1;i<=8;i++){auto m=loadObj("assets/level/trees/tree"+std::to_string(i)+".obj");if(m.valid())realTrees_.push_back(m);}
 lights_={{{-8,8,8},{.10f,.16f,.28f},42,1.05f},{{-4,2.8f,-13},{1.0f,.12f,.025f},9,1.7f},{{6,2,1},{.05f,.09f,.15f},20,.42f}};
 return true;
}
void Scene::draw(Shader&s,const Mesh&m,const Mat4&model,const Vec3&color,const Mat4&vp,const Vec3&cam,const Vec3&fp,const Vec3&fd,bool on,float time,int mat){
 if(!m.valid())return;s.bind();s.setMat4("uModel",model);s.setMat4("uMVP",vp*model);s.setVec3("uColor",color);s.setVec3("uCamera",cam);s.setVec3("uFlashPos",fp);s.setVec3("uFlashDir",fd);s.setFloat("uFlashOn",on?1.f:0.f);s.setFloat("uTime",time);s.setInt("uMaterial",mat);s.setInt("uUseTexture",m.textured()?1:0);s.setInt("uAlphaCutout",mat==2?1:0);s.setInt("uTexture",0);
 for(int i=0;i<3;i++){const char*p=i==0?"uLight0Pos":i==1?"uLight1Pos":"uLight2Pos";const char*c=i==0?"uLight0Color":i==1?"uLight1Color":"uLight2Color";const char*r=i==0?"uLight0Radius":i==1?"uLight1Radius":"uLight2Radius";const char*in=i==0?"uLight0Intensity":i==1?"uLight1Intensity":"uLight2Intensity";s.setVec3(p,lights_[i].position);s.setVec3(c,lights_[i].color);s.setFloat(r,lights_[i].radius);s.setFloat(in,lights_[i].intensity);}m.draw();
}
void Scene::render(Shader&s,const Mat4&vp,const Vec3&cam,const Vec3&fp,const Vec3&fd,bool on,float time){
 draw(s,ground_,Mat4::identity(),{.022f,.027f,.023f},vp,cam,fp,fd,on,time,0);
 draw(s,cube_,Mat4::translation({0,-.16f,-8})*Mat4::scale({3.8f,.045f,44}),{.038f,.034f,.029f},vp,cam,fp,fd,on,time,0);
 const float spots[][4]={{-18,21,3.1f,.2f},{-14,23,3.5f,-.4f},{-9,20,3.0f,.7f},{-3,24,3.7f,-.3f},{4,22,3.2f,.6f},{11,24,3.8f,-.5f},{18,20,3.3f,.4f},{-21,15,3.8f,-.2f},{-17,9,3.2f,.5f},{19,10,3.6f,-.6f},{22,3,3.5f,.8f},{-21,-3,3.7f,.3f},{20,-8,3.4f,-.4f},{-20,-13,3.8f,.7f},{-13,-18,3.2f,-.6f},{-5,-22,3.7f,.2f},{4,-23,3.3f,-.5f},{14,-20,3.8f,.4f},{21,-15,3.5f,-.3f},{-14,1,2.9f,.1f},{15,-1,3.1f,-.8f},{-11,8,2.7f,.4f},{10,11,3.0f,-.3f},{-19,-7,3.2f,.5f},{18,14,3.2f,-.2f}};
 if(!realTrees_.empty()){int i=0;for(const auto&p:spots){const Mesh&m=realTrees_[i++%realTrees_.size()];float sc=p[2]*(.82f+(i%5)*.05f);draw(s,m,Mat4::translation({p[0],0,p[1]})*Mat4::rotationY(p[3])*Mat4::scale({sc,sc,sc}),{.18f,.23f,.14f},vp,cam,fp,fd,on,time,2);}}
 else {for(int i=0;i<12;i++)draw(s,pine_,Mat4::translation({-18.f+(i%4)*12.f,0,20.f-(i/4)*8.f})*Mat4::scale({2.5f,2.5f,2.5f}),{.012f,.035f,.018f},vp,cam,fp,fd,on,time,2);}
 if(hasRealHouse_)draw(s,realHouse_,Mat4::translation({-6,0,-17})*Mat4::rotationY(.35f)*Mat4::scale({4.0f,4.0f,4.0f}),{.30f,.27f,.23f},vp,cam,fp,fd,on,time,4);
 const float rocks[][3]={{-3.5f,.4f,-2},{3.5f,.35f,-6},{-5.5f,.3f,4},{6,.25f,7},{-7,.22f,9},{7,.28f,-13}};for(auto&r:rocks)draw(s,rock_,Mat4::translation({r[0],r[1],r[2]})*Mat4::scale({1.15f,.62f,.88f}),{.055f,.06f,.055f},vp,cam,fp,fd,on,time,3);
}
}