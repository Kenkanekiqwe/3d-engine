#include "scene.hpp"
#include <cmath>

namespace cry {

bool Scene::create(){
    cube_=makeCube();
    ground_=makeGround(36,44);
    pine_=makePine();
    rock_=makeRock();

    realTree_=loadObj("assets/level/tree.obj");
    realHouse_=loadObj("assets/level/house.obj");
    hasRealTree_=realTree_.valid();
    hasRealHouse_=realHouse_.valid();

    lights_={
        {{-8,10,8},{.20f,.30f,.50f},45,2.0f},
        {{-4,3,-13},{1.0f,.22f,.06f},11,5.0f},
        {{5,2,2},{.10f,.16f,.20f},14,1.2f}
    };
    return true;
}

void Scene::draw(Shader&s,const Mesh&m,const Mat4&model,const Vec3&color,
                 const Mat4&vp,const Vec3&cam,const Vec3&fp,const Vec3&fd,
                 bool on,float time,int mat){
    if(!m.valid()) return;
    s.bind();
    s.setMat4("uModel",model);
    s.setMat4("uMVP",vp*model);
    s.setVec3("uColor",color);
    s.setVec3("uCamera",cam);
    s.setVec3("uFlashPos",fp);
    s.setVec3("uFlashDir",fd);
    s.setFloat("uFlashOn",on?1.f:0.f);
    s.setFloat("uTime",time);
    s.setInt("uMaterial",mat);

    for(int i=0;i<3;i++){
        const char* p=i==0?"uLight0Pos":i==1?"uLight1Pos":"uLight2Pos";
        const char* c=i==0?"uLight0Color":i==1?"uLight1Color":"uLight2Color";
        const char* r=i==0?"uLight0Radius":i==1?"uLight1Radius":"uLight2Radius";
        const char* in=i==0?"uLight0Intensity":i==1?"uLight1Intensity":"uLight2Intensity";
        s.setVec3(p,lights_[i].position);
        s.setVec3(c,lights_[i].color);
        s.setFloat(r,lights_[i].radius);
        s.setFloat(in,lights_[i].intensity);
    }
    m.draw();
}

void Scene::render(Shader&s,const Mat4&vp,const Vec3&cam,const Vec3&fp,
                   const Vec3&fd,bool on,float time){

    draw(s,ground_,Mat4::identity(),{.045f,.052f,.040f},vp,cam,fp,fd,on,time,0);
    draw(s,cube_,Mat4::translation({0,-.14f,-7})*Mat4::scale({3.4f,.05f,42}),
         {.075f,.055f,.038f},vp,cam,fp,fd,on,time,0);

    // Real downloaded forest meshes surround the playable road.
    if(hasRealTree_){
        const float trees[][4]={
            {-15,17,2.8f,.2f},{-11,20,3.2f,-.4f},{-6,19,2.5f,.7f},{1,20,3.0f,-.3f},
            {8,19,2.7f,.6f},{14,17,3.4f,-.5f},{-17,11,3.0f,.4f},{-14,5,2.4f,-.2f},
            {15,7,3.1f,.8f},{17,1,2.8f,-.7f},{-17,-2,3.3f,.3f},{15,-4,2.9f,-.4f},
            {-16,-10,3.0f,.7f},{-10,-15,2.6f,-.6f},{-3,-18,3.1f,.2f},{6,-19,2.7f,-.5f},
            {14,-15,3.2f,.4f},{-18,1,3.5f,-.2f},{18,11,3.4f,.5f}
        };
        for(const auto&t:trees){
            draw(s,realTree_,
                Mat4::translation({t[0],0,t[1]})*
                Mat4::rotationY(t[3])*
                Mat4::scale({t[2],t[2],t[2]}),
                {.20f,.24f,.14f},vp,cam,fp,fd,on,time,2);
        }
    }else{
        const float trees[][3]={{-13,15,1.55f},{-9,17,1.15f},{-4,19,1.7f},{2,18,1.3f},{8,17,1.8f},{14,14,1.45f},{-16,9,1.6f},{-12,5,1.15f},{12,6,1.5f},{16,2,1.8f},{-16,-4,1.55f},{14,-5,1.35f},{-15,-12,1.8f},{-9,-16,1.45f},{4,-18,1.75f},{13,-15,1.5f}};
        for(auto&t:trees)
            draw(s,pine_,Mat4::translation({t[0],0,t[1]})*Mat4::rotationY(t[0]*.13f)*Mat4::scale({t[2],t[2],t[2]}),{.026f,.070f,.038f},vp,cam,fp,fd,on,time,2);
    }

    // Main landmark: a real downloaded house mesh.
    if(hasRealHouse_){
        draw(s,realHouse_,
             Mat4::translation({-5,0,-15})*Mat4::scale({4.0f,4.0f,4.0f}),
             {.24f,.20f,.16f},vp,cam,fp,fd,on,time,4);
    }else{
        draw(s,cube_,Mat4::translation({-5,1.65f,-15})*Mat4::scale({6.4f,3.3f,4.2f}),
             {.085f,.058f,.038f},vp,cam,fp,fd,on,time,4);
    }

    const float rocks[][3]={{-3.5f,.45f,-2},{3.5f,.42f,-6},{-5.5f,.35f,4},{6,.28f,7}};
    for(auto&r:rocks)
        draw(s,rock_,Mat4::translation({r[0],r[1],r[2]})*Mat4::scale({1.2f,.65f,.9f}),
             {.09f,.095f,.085f},vp,cam,fp,fd,on,time,3);

    // Roadside landmark.
    for(int i=0;i<7;i++)
        draw(s,cube_,Mat4::translation({-3.2f+i*.9f,.65f,-10.2f})*
             Mat4::rotationY((i%2?-1:1)*.22f)*Mat4::scale({.12f,1.3f,.12f}),
             {.055f,.034f,.02f},vp,cam,fp,fd,on,time,4);

    // Distant silhouette.
    float sx=.65f+std::sin(time*.17f)*.12f;
    draw(s,cube_,Mat4::translation({sx,1.5f,-11})*Mat4::scale({.42f,2.8f,.32f}),
         {.002f,.002f,.002f},vp,cam,fp,fd,on,time,8);
    draw(s,cube_,Mat4::translation({sx,3.28f,-11})*Mat4::scale({.55f,.58f,.45f}),
         {.002f,.002f,.002f},vp,cam,fp,fd,on,time,8);
}

}
