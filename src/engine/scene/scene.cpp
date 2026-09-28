#include "scene.hpp"
#include <cmath>
#include <filesystem>
namespace cry {
bool Scene::create(){
 cube_=makeCube();ground_=makeGround(64,64);pine_=makePine();rock_=makeRock();realHouse_=loadObj("assets/level/house.obj");hasRealHouse_=realHouse_.valid();
 realTrees_.clear();
 for(int i=1;i<=8;i++){auto m=loadObj("assets/level/trees/tree"+std::to_string(i)+".obj");if(m.valid())realTrees_.push_back(m);}
 lights_={{{-8,10,8},{.18f,.26f,.42f},45,1.25f},{{-4,3,-13},{1.0f,.16f,.035f},10,2.4f},{{5,2,2},{.08f,.13f,.18f},16,.65f}};
 return true;
}
void Scene::draw(Shader&s,const Mesh&m,const Mat4&model,const Vec3&color,const Mat4&vp,const Vec3&cam,const Vec3&fp,const Vec3&fd,bool on,float time,int mat){
 if(!m.valid())return;s.bind();s.setMat4("uModel",model);s.setMat4("uMVP",vp*model);s.setVec3("uColor",color);s.setVec3("uCamera",cam);s.setVec3("uFlashPos",fp);s.setVec3("uFlashDir",fd);s.setFloat("uFlashOn",on?1.f:0.f);s.setFloat("uTime",time);s.setInt("uMaterial",mat);s.setInt("uUseTexture",m.textured()?1:0);s.setInt("uAlphaCutout",mat==2?1:0);s.setInt("uTexture",0);
 for(int i=0;i<3;i++){const char*p=i==0?"uLight0Pos":i==1?"uLight1Pos":"uLight2Pos";const char*c=i==0?"uLight0Color":i==1?"uLight1Color":"uLight2Color";const char*r=i==0?"uLight0Radius":i==1?"uLight1Radius":"uLight2Radius";const char*in=i==0?"uLight0Intensity":i==1?"uLight1Intensity":"uLight2Intensity";s.setVec3(p,lights_[i].position);s.setVec3(c,lights_[i].color);s.setFloat(r,lights_[i].radius);s.setFloat(in,lights_[i].intensity);}m.draw();
}
void Scene::render(Shader&s,const Mat4&vp,const Vec3&cam,const Vec3&fp,const Vec3&fd,bool on,float time){
 draw(s,ground_,Mat4::identity(),{.035f,.042f,.033f},vp,cam,fp,fd,on,time,0);
 draw(s,cube_,Mat4::translation({0,-.16f,-7})*Mat4::scale({3.6f,.05f,42}),{.055f,.045f,.036f},vp,cam,fp,fd,on,time,0);
 if(!realTrees_.empty()){
  const float spots[][4]={{-17,19,3.5f,.2f},{-13,22,3.0f,-.4f},{-8,20,3.2f,.7f},{-2,22,3.8f,-.3f},{5,21,3.4f,.6f},{12,20,3.7f,-.5f},{18,17,3.1f,.4f},{-20,13,3.8f,-.2f},{-16,7,3.0f,.5f},{18,8,3.6f,-.6f},{20,1,3.4f,.8f},{-20,-2,3.8f,.3f},{18,-6,3.2f,-.4f},{-19,-10,3.5f,.7f},{-13,-16,3.1f,-.6f},{-5,-20,3.8f,.2f},{4,-21,3.2f,-.5f},{14,-18,3.6f,.4f},{20,-13,3.4f,-.3f},{-21,3,3.7f,.5f},{21,13,3.8f,-.2f},{-14,-2,2.8f,.1f},{15,-1,3.0f,-.8f},{-10,7,2.5f,.4f},{10,10,2.9f,-.3f}};
  int i=0;for(const auto&p:spots){const Mesh&m=realTrees_[i++%realTrees_.size()];float s0=p[2]*(.82f+((i%5)*.055f));draw(s,m,Mat4::translation({p[0],0,p[1]})*Mat4::rotationY(p[3])*Mat4::scale({s0,s0,s0}),{.20f,.25f,.14f},vp,cam,fp,fd,on,time,2);}
 }else{
  const float trees[][3]={{-16,18,1.8f},{-12,20,1.5f},{-7,21,1.9f},{0,20,1.6f},{7,21,2.0f},{14,19,1.7f},{18,14,1.8f},{-19,9,1.8f},{-15,3,1.5f},{17,5,1.8f},{-18,-5,1.9f},{17,-7,1.7f},{-17,-13,2.0f},{-10,-18,1.7f},{2,-20,2.0f},{13,-18,1.8f}};
  for(auto&t:trees)draw(s,pine_,Mat4::translation({t[0],0,t[1]})*Mat4::rotationY(t[0]*.13f)*Mat4::scale({t[2],t[2],t[2]}),{.018f,.055f,.028f},vp,cam,fp,fd,on,time,2);
 }
 if(hasRealHouse_)draw(s,realHouse_,Mat4::translation({-6,0,-16})*Mat4::rotationY(.35f)*Mat4::scale({4.5f,4.5f,4.5f}),{.24f,.19f,.14f},vp,cam,fp,fd,on,time,4);
 else draw(s,cube_,Mat4::translation({-6,1.7f,-16})*Mat4::scale({6.8f,3.4f,4.6f}),{.07f,.045f,.03f},vp,cam,fp,fd,on,time,4);
 const float rocks[][3]={{-3.5f,.45f,-2},{3.5f,.42f,-6},{-5.5f,.35f,4},{6,.28f,7},{-7,.25f,9},{7,.3f,-13}};for(auto&r:rocks)draw(s,rock_,Mat4::translation({r[0],r[1],r[2]})*Mat4::scale({1.2f,.65f,.9f}),{.07f,.075f,.068f},vp,cam,fp,fd,on,time,3);
 for(int i=0;i<11;i++)draw(s,cube_,Mat4::translation({-3.1f+i*.62f,.62f,-10.5f})*Mat4::rotationY((i%2?-1:1)*.22f)*Mat4::scale({.10f,1.25f,.10f}),{.045f,.028f,.017f},vp,cam,fp,fd,on,time,4);
 float sx=.65f+std::sin(time*.17f)*.12f;draw(s,cube_,Mat4::translation({sx,1.5f,-11})*Mat4::scale({.42f,2.8f,.32f}),{.001f,.001f,.001f},vp,cam,fp,fd,on,time,8);draw(s,cube_,Mat4::translation({sx,3.28f,-11})*Mat4::scale({.55f,.58f,.45f}),{.001f,.001f,.001f},vp,cam,fp,fd,on,time,8);
}
}