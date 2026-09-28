#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cmath>
#include "engine/math/math.hpp"
#include "engine/graphics/opengl.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/input/input.hpp"
#include "engine/camera/camera.hpp"
#include "engine/scene/scene.hpp"
#include "engine/audio/audio.hpp"
using namespace cry;

int main(){
    if(!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES,4);
    GLFWwindow* window=glfwCreateWindow(1280,720,"CryHorror Engine",nullptr,nullptr);
    if(!window){glfwTerminate();return 1;}
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if(!gl::load()){glfwDestroyWindow(window);glfwTerminate();return 1;}
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Shader shader;
    if(!shader.create("assets/shaders/world.vert","assets/shaders/world.frag")) return 1;
    Input input(window);
    Audio audio;
    Camera camera({0,1.68f,6.5f});
    Scene scene;
    scene.create();
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

    audio.playAmbient("assets/audio/ambient_horror.wav", true);

    bool lightOn=true,lastF=false;
    const auto start=std::chrono::steady_clock::now();
    float previous=0,walk=0;

    while(!glfwWindowShouldClose(window)){
        float time=std::chrono::duration<float>(std::chrono::steady_clock::now()-start).count();
        float dt=std::clamp(time-previous,.001f,.033f); previous=time;
        glfwPollEvents();
        input.beginFrame();
        if(input.down(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window,GLFW_TRUE);

        camera.look(input.mouseDelta());
        Vec3 move{};
        if(input.down(GLFW_KEY_W)) move+=camera.flatForward();
        if(input.down(GLFW_KEY_S)) move-=camera.flatForward();
        if(input.down(GLFW_KEY_D)) move+=camera.right();
        if(input.down(GLFW_KEY_A)) move-=camera.right();

        if(move.length()>.001f){
            move=move.normalized();
            float speed=input.down(GLFW_KEY_LEFT_SHIFT)?3.1f:1.9f;
            camera.setPosition(camera.position()+move*speed*dt);
            walk+=dt*7.0f;
        }

        Vec3 p=camera.position();
        p.x=std::clamp(p.x,-18.f,18.f);
        p.z=std::clamp(p.z,-20.f,20.f);
        p.y=1.68f+((move.length()>.001f)?std::sin(walk)*.012f:0.f);
        camera.setPosition(p);

        bool f=input.down(GLFW_KEY_F);
        if(f&&!lastF) lightOn=!lightOn;
        lastF=f;

        int w=1,h=1;
        glfwGetFramebufferSize(window,&w,&h);
        glViewport(0,0,w,h);
        glClearColor(.002f,.004f,.006f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        Mat4 projection=Mat4::perspective(.82f,float(w)/float(h),.05f,85.f);
        Mat4 vp=projection*camera.view();

        Vec3 beamDirection=camera.forward();
        Vec3 beamOrigin=camera.position()+camera.right()*.28f+Vec3{0,-.16f,.0f}+beamDirection*.20f;
        scene.render(shader,vp,camera.position(),beamOrigin,beamDirection,lightOn,time);

        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
