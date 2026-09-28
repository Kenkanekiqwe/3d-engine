#include "mesh.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <array>

namespace cry {

Mesh::Mesh(const std::vector<float>& d){
    if(d.empty()) return;
    gl::GenVertexArrays(1,&vao);
    gl::BindVertexArray(vao);
    gl::GenBuffers(1,&vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER,vbo);
    gl::BufferData(GL_ARRAY_BUFFER,(std::ptrdiff_t)(d.size()*sizeof(float)),d.data(),GL_STATIC_DRAW);
    gl::VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),nullptr);
    gl::EnableVertexAttribArray(0);
    count=(int)d.size()/3;
}

void Mesh::draw()const{
    if(!valid()) return;
    gl::BindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,count);
}

static void tri(std::vector<float>&v,Vec3 a,Vec3 b,Vec3 c){
    v.insert(v.end(),{a.x,a.y,a.z,b.x,b.y,b.z,c.x,c.y,c.z});
}

static int objIndex(int index,int size){
    if(index>0) return index-1;
    if(index<0) return size+index;
    return -1;
}

Mesh loadObj(const std::string& path){
    std::ifstream file(path);
    if(!file) return {};

    std::vector<Vec3> positions;
    std::vector<float> out;
    std::string line;

    while(std::getline(file,line)){
        if(line.rfind("v ",0)==0){
            std::istringstream ss(line.substr(2));
            Vec3 p{};
            ss>>p.x>>p.y>>p.z;
            positions.push_back(p);
        }else if(line.rfind("f ",0)==0){
            std::istringstream ss(line.substr(2));
            std::vector<int> face;
            std::string token;
            while(ss>>token){
                const auto slash=token.find('/');
                const std::string pos=token.substr(0,slash);
                try{ face.push_back(objIndex(std::stoi(pos),(int)positions.size())); }
                catch(...){ face.push_back(-1); }
            }
            if(face.size()<3) continue;
            for(size_t i=1;i+1<face.size();++i){
                const int a=face[0],b=face[i],c=face[i+1];
                if(a>=0&&b>=0&&c>=0&&a<(int)positions.size()&&b<(int)positions.size()&&c<(int)positions.size())
                    tri(out,positions[a],positions[b],positions[c]);
            }
        }
    }
    return Mesh(out);
}

static void fr(std::vector<float>&v,float y0,float y1,float r0,float r1,int n){
    const float t=6.2831853f;
    for(int i=0;i<n;i++){
        float a=t*i/n,b=t*(i+1)/n;
        Vec3 p0{std::cos(a)*r0,y0,std::sin(a)*r0},p1{std::cos(b)*r0,y0,std::sin(b)*r0};
        Vec3 q0{std::cos(a)*r1,y1,std::sin(a)*r1},q1{std::cos(b)*r1,y1,std::sin(b)*r1};
        tri(v,p0,p1,q1);tri(v,p0,q1,q0);
    }
}

Mesh makePine(){
    std::vector<float>v;
    fr(v,0,2.5f,.2f,.12f,12);
    fr(v,.9f,3.15f,1.25f,.06f,14);
    fr(v,1.75f,4.15f,.96f,.05f,14);
    fr(v,2.65f,5.35f,.68f,.02f,14);
    return Mesh(v);
}

Mesh makeRock(){
    std::vector<float>v;
    for(int r=0;r<3;r++){
        float y0=-.45f+r*.4f,y1=y0+.4f,r0=1-r*.2f,r1=1-(r+1)*.2f;
        for(int i=0;i<12;i++){
            float a=6.2831853f*i/12,b=6.2831853f*(i+1)/12;
            float w0=.85f+.15f*std::sin(i*5.7f),w1=.85f+.15f*std::sin((i+1)*5.7f);
            Vec3 p0{std::cos(a)*r0*w0,y0,std::sin(a)*r0*w0},p1{std::cos(b)*r0*w1,y0,std::sin(b)*r0*w1};
            Vec3 q0{std::cos(a)*r1*w0,y1,std::sin(a)*r1*w0},q1{std::cos(b)*r1*w1,y1,std::sin(b)*r1*w1};
            tri(v,p0,p1,q1);tri(v,p0,q1,q0);
        }
    }
    return Mesh(v);
}

Mesh makeGround(int n,float size){
    std::vector<float>v;
    auto h=[](float x,float z){return .08f*std::sin(x*.55f+z*.21f)+.035f*std::sin(z*1.7f-x*.3f);};
    for(int z=0;z<n;z++)for(int x=0;x<n;x++){
        float x0=-size/2+size*x/n,x1=-size/2+size*(x+1)/n,z0=-size/2+size*z/n,z1=-size/2+size*(z+1)/n;
        Vec3 a{x0,h(x0,z0),z0},b{x1,h(x1,z0),z0},c{x1,h(x1,z1),z1},d{x0,h(x0,z1),z1};
        tri(v,a,c,b);tri(v,a,d,c);
    }
    return Mesh(v);
}

Mesh makeCube(){
    return Mesh({
        -.5f,-.5f,-.5f,.5f,-.5f,-.5f,.5f,.5f,-.5f,
        .5f,.5f,-.5f,-.5f,.5f,-.5f,-.5f,-.5f,-.5f,
        -.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,.5f,.5f,
        .5f,.5f,.5f,-.5f,.5f,.5f,-.5f,-.5f,.5f,
        -.5f,.5f,.5f,-.5f,.5f,-.5f,-.5f,-.5f,-.5f,
        -.5f,-.5f,-.5f,-.5f,-.5f,.5f,-.5f,.5f,.5f,
        .5f,-.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,.5f,
        .5f,.5f,.5f,.5f,-.5f,.5f,.5f,-.5f,-.5f,
        -.5f,.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,.5f,
        .5f,.5f,.5f,-.5f,.5f,.5f,-.5f,.5f,-.5f,
        -.5f,-.5f,-.5f,.5f,-.5f,.5f,.5f,-.5f,-.5f,
        .5f,-.5f,.5f,-.5f,-.5f,-.5f,-.5f,-.5f,.5f
    });
}

}
