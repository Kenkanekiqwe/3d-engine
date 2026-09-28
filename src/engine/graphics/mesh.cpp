#define STB_IMAGE_IMPLEMENTATION
#include "../../../third_party/stb_image.h"
#include "mesh.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
namespace cry {
static void vtx(std::vector<float>&v,Vec3 p,float u,float t){v.insert(v.end(),{p.x,p.y,p.z,u,t});}
static void tri(std::vector<float>&v,Vec3 a,Vec3 b,Vec3 c){vtx(v,a,0,0);vtx(v,b,1,0);vtx(v,c,1,1);}
Mesh::Mesh(const std::vector<float>&d){if(d.empty())return;gl::GenVertexArrays(1,&vao);gl::BindVertexArray(vao);gl::GenBuffers(1,&vbo);gl::BindBuffer(GL_ARRAY_BUFFER,vbo);gl::BufferData(GL_ARRAY_BUFFER,(std::ptrdiff_t)(d.size()*sizeof(float)),d.data(),GL_STATIC_DRAW);gl::VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);gl::EnableVertexAttribArray(0);gl::VertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));gl::EnableVertexAttribArray(1);count=(int)d.size()/5;}
static unsigned texture(const std::string&p){if(p.empty())return 0;int w=0,h=0,n=0;stbi_uc*d=stbi_load(p.c_str(),&w,&h,&n,4);if(!d)return 0;unsigned t=0;gl::GenTextures(1,&t);gl::BindTexture(GL_TEXTURE_2D,t);gl::TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);gl::TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);gl::TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);gl::TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);gl::TexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,d);gl::GenerateMipmap(GL_TEXTURE_2D);stbi_image_free(d);return t;}
void Mesh::draw()const{if(!valid())return;if(texture){gl::ActiveTexture(GL_TEXTURE0);gl::BindTexture(GL_TEXTURE_2D,texture);}gl::BindVertexArray(vao);glDrawArrays(GL_TRIANGLES,0,count);}
static int idx(int n,int s){return n>0?n-1:(n<0?s+n:-1);}
Mesh loadObj(const std::string&path){
 std::ifstream f(path);if(!f)return{};std::vector<Vec3>pos,uv;std::vector<float>out;std::string line,mtl,tex;
 std::filesystem::path base=std::filesystem::path(path).parent_path();
 while(std::getline(f,line)){
  if(line.rfind("mtllib ",0)==0)mtl=line.substr(7);
  else if(line.rfind("v ",0)==0){std::istringstream s(line.substr(2));Vec3 p{};s>>p.x>>p.y>>p.z;pos.push_back(p);}
  else if(line.rfind("vt ",0)==0){std::istringstream s(line.substr(3));float u=0,t=0;s>>u>>t;uv.push_back({u,t,0});}
  else if(line.rfind("f ",0)==0){std::istringstream s(line.substr(2));std::vector<std::pair<int,int>>face;std::string q;while(s>>q){std::stringstream z(q);std::string a,b,c;std::getline(z,a,'/');std::getline(z,b,'/');std::getline(z,c,'/');int pi=idx(std::stoi(a),(int)pos.size()),ui=b.empty()?-1:idx(std::stoi(b),(int)uv.size());face.push_back({pi,ui});}for(size_t i=1;i+1<face.size();++i){auto A=face[0],B=face[i],C=face[i+1];if(A.first<0||B.first<0||C.first<0)continue;Vec3 ua=A.second>=0?uv[A.second]:Vec3{},ub=B.second>=0?uv[B.second]:Vec3{1,0,0},uc=C.second>=0?uv[C.second]:Vec3{1,1,0};vtx(out,pos[A.first],ua.x,ua.y);vtx(out,pos[B.first],ub.x,ub.y);vtx(out,pos[C.first],uc.x,uc.y);}}
 }
 if(!mtl.empty()){std::ifstream m(base/mtl);while(std::getline(m,line))if(line.rfind("map_Kd ",0)==0){tex=line.substr(7);break;}}
 Mesh r(out);if(!tex.empty()){r.texture=texture((base/tex).lexically_normal().string());if(!r.texture)std::cerr<<"texture failed: "<<tex<<"\n";}return r;
}
static void fr(std::vector<float>&v,float y0,float y1,float r0,float r1,int n){for(int i=0;i<n;i++){float a=6.2831853f*i/n,b=6.2831853f*(i+1)/n;Vec3 p0{std::cos(a)*r0,y0,std::sin(a)*r0},p1{std::cos(b)*r0,y0,std::sin(b)*r0},q0{std::cos(a)*r1,y1,std::sin(a)*r1},q1{std::cos(b)*r1,y1,std::sin(b)*r1};tri(v,p0,p1,q1);tri(v,p0,q1,q0);}}
Mesh makePine(){std::vector<float>v;fr(v,0,2.5f,.2f,.12f,12);fr(v,.9f,3.15f,1.25f,.06f,14);fr(v,1.75f,4.15f,.96f,.05f,14);fr(v,2.65f,5.35f,.68f,.02f,14);return Mesh(v);}
Mesh makeRock(){std::vector<float>v;for(int r=0;r<3;r++)for(int i=0;i<12;i++){float y0=-.45f+r*.4f,y1=y0+.4f,r0=1-r*.2f,r1=1-(r+1)*.2f,a=6.2831853f*i/12,b=6.2831853f*(i+1)/12,w0=.85f+.15f*std::sin(i*5.7f),w1=.85f+.15f*std::sin((i+1)*5.7f);Vec3 p0{std::cos(a)*r0*w0,y0,std::sin(a)*r0*w0},p1{std::cos(b)*r0*w1,y0,std::sin(b)*r0*w1},q0{std::cos(a)*r1*w0,y1,std::sin(a)*r1*w0},q1{std::cos(b)*r1*w1,y1,std::sin(b)*r1*w1};tri(v,p0,p1,q1);tri(v,p0,q1,q0);}return Mesh(v);}
Mesh makeGround(int n,float size){std::vector<float>v;for(int z=0;z<n;z++)for(int x=0;x<n;x++){float x0=-size/2+size*x/n,x1=-size/2+size*(x+1)/n,z0=-size/2+size*z/n,z1=-size/2+size*(z+1)/n;auto h=[](float x,float z){return .08f*std::sin(x*.55f+z*.21f)+.035f*std::sin(z*1.7f-x*.3f);};Vec3 a{x0,h(x0,z0),z0},b{x1,h(x1,z0),z0},c{x1,h(x1,z1),z1},d{x0,h(x0,z1),z1};vtx(v,a,x/4.f,z/4.f);vtx(v,c,(x+1)/4.f,(z+1)/4.f);vtx(v,b,(x+1)/4.f,z/4.f);vtx(v,a,x/4.f,z/4.f);vtx(v,d,x/4.f,(z+1)/4.f);vtx(v,c,(x+1)/4.f,(z+1)/4.f);}return Mesh(v);}
Mesh makeCube(){std::vector<float>v;Vec3 p[8]={{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},{.5f,.5f,-.5f},{-.5f,.5f,-.5f},{-.5f,-.5f,.5f},{.5f,-.5f,.5f},{.5f,.5f,.5f},{-.5f,.5f,.5f}};int f[6][4]={{0,1,2,3},{5,4,7,6},{4,0,3,7},{1,5,6,2},{3,2,6,7},{4,5,1,0}};for(auto&q:f){vtx(v,p[q[0]],0,0);vtx(v,p[q[1]],1,0);vtx(v,p[q[2]],1,1);vtx(v,p[q[0]],0,0);vtx(v,p[q[2]],1,1);vtx(v,p[q[3]],0,1);}return Mesh(v);}
}