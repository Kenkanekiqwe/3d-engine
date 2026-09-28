#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "engine/math/math.hpp"
#include "engine/graphics/opengl.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/input/input.hpp"

using namespace cry;

static void drawCube(unsigned vao, Shader& shader, const Mat4& model, const Vec3& color,
                     const Mat4& vp, const Vec3& camera, const Vec3& light,
                     const Vec3& flashDir, bool flashlight, float time) {
    shader.bind();
    shader.setMat4("uModel", model);
    shader.setMat4("uMVP", vp * model);
    shader.setVec3("uColor", color);
    shader.setVec3("uCamera", camera);
    shader.setVec3("uLightPos", light);
    shader.setVec3("uFlashDir", flashDir);
    shader.setFloat("uFlashOn", flashlight ? 1.0f : 0.0f);
    shader.setFloat("uTime", time);
    gl::BindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main() {
    if(!glfwInit()) {
        std::cerr << "GLFW initialization failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "CryHorror Engine", nullptr, nullptr);
    if(!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(!gl::load()) {
        std::cerr << "OpenGL 3.3 function loading failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Shader shader;
    if(!shader.create("assets/shaders/world.vert", "assets/shaders/world.frag")) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    constexpr float cube[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
         0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,

        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,

         0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,

        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
         0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,

        -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
    };

    unsigned vao{}, vbo{};
    gl::GenVertexArrays(1,&vao);
    gl::BindVertexArray(vao);
    gl::GenBuffers(1,&vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER,vbo);
    gl::BufferData(GL_ARRAY_BUFFER,sizeof(cube),cube,GL_STATIC_DRAW);
    gl::VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),nullptr);
    gl::EnableVertexAttribArray(0);

    Input input(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Vec3 camera{0.0f,1.65f,6.0f};
    float yaw=-1.5708f;
    float pitch=0.0f;
    bool flashlight=true;
    bool lastF=false;
    bool lastE=false;
    float doorAngle=0.0f;
    bool doorOpen=false;

    const auto start=std::chrono::steady_clock::now();

    while(!glfwWindowShouldClose(window)) {
        const float time=std::chrono::duration<float>(
            std::chrono::steady_clock::now()-start).count();

        glfwPollEvents();
        input.beginFrame();

        if(input.down(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window,GLFW_TRUE);

        const Vec3 md=input.mouseDelta();
        yaw += md.x*0.0025f;
        pitch -= md.y*0.0025f;
        pitch=std::clamp(pitch,-1.45f,1.45f);

        Vec3 forward{
            std::cos(pitch)*std::cos(yaw),
            std::sin(pitch),
            std::cos(pitch)*std::sin(yaw)
        };
        forward=forward.normalized();

        Vec3 flatForward{std::cos(yaw),0.0f,std::sin(yaw)};
        Vec3 right{std::sin(yaw),0.0f,-std::cos(yaw)};

        const float dt=1.0f/60.0f;
        const float speed=input.down(GLFW_KEY_LEFT_SHIFT)?5.0f:2.5f;

        if(input.down(GLFW_KEY_W)) camera += flatForward*speed*dt;
        if(input.down(GLFW_KEY_S)) camera -= flatForward*speed*dt;
        if(input.down(GLFW_KEY_D)) camera += right*speed*dt;
        if(input.down(GLFW_KEY_A)) camera -= right*speed*dt;

        const bool fNow=input.down(GLFW_KEY_F);
        if(fNow && !lastF) flashlight=!flashlight;
        lastF=fNow;

        const bool eNow=input.down(GLFW_KEY_E);
        if(eNow && !lastE && camera.z < -2.0f && camera.z > -5.0f)
            doorOpen=!doorOpen;
        lastE=eNow;

        const float target=doorOpen?1.35f:0.0f;
        doorAngle += (target-doorAngle)*std::min(1.0f,dt*8.0f);

        int width=1,height=1;
        glfwGetFramebufferSize(window,&width,&height);
        glViewport(0,0,width,height);
        glClearColor(0.004f,0.005f,0.007f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        const Mat4 projection=Mat4::perspective(
            1.05f, static_cast<float>(width)/static_cast<float>(height),0.05f,100.0f);
        const Mat4 view=Mat4::lookAt(camera,camera+forward,{0,1,0});
        const Mat4 vp=projection*view;

        const Vec3 light{0.0f,3.0f,-1.5f};

        // Abandoned room.
        drawCube(vao,shader,Mat4::translation({0,-0.75f,0})*Mat4::scale({14,0.5f,14}),
                 {0.16f,0.17f,0.19f},vp,camera,light,forward,flashlight,time);
        drawCube(vao,shader,Mat4::translation({0,3.0f,-7})*Mat4::scale({14,6,0.5f}),
                 {0.10f,0.11f,0.12f},vp,camera,light,forward,flashlight,time);
        drawCube(vao,shader,Mat4::translation({-7,3.0f,0})*Mat4::scale({0.5f,6,14}),
                 {0.08f,0.09f,0.10f},vp,camera,light,forward,flashlight,time);
        drawCube(vao,shader,Mat4::translation({7,3.0f,0})*Mat4::scale({0.5f,6,14}),
                 {0.08f,0.09f,0.10f},vp,camera,light,forward,flashlight,time);

        // Door and frame.
        const Mat4 door=Mat4::translation({0,1.4f,-6.7f})*
                        Mat4::rotationY(doorAngle)*
                        Mat4::translation({1.35f,0,0})*
                        Mat4::scale({2.7f,2.8f,0.18f});
        drawCube(vao,shader,door,{0.18f,0.12f,0.09f},vp,camera,light,forward,flashlight,time);
        drawCube(vao,shader,Mat4::translation({-1.55f,1.4f,-6.7f})*
                 Mat4::scale({0.25f,3.1f,0.35f}),{0.08f,0.07f,0.06f},
                 vp,camera,light,forward,flashlight,time);
        drawCube(vao,shader,Mat4::translation({1.55f,1.4f,-6.7f})*
                 Mat4::scale({0.25f,3.1f,0.35f}),{0.08f,0.07f,0.06f},
                 vp,camera,light,forward,flashlight,time);

        // A distant silhouette.
        const float creatureX=std::sin(time*0.35f)*2.0f;
        const float creatureZ=-3.5f+std::sin(time*0.2f)*0.8f;
        drawCube(vao,shader,Mat4::translation({creatureX,1.1f,creatureZ})*
                 Mat4::scale({0.65f,2.2f,0.45f}),{0.012f,0.012f,0.016f},
                 vp,camera,light,forward,flashlight,time);
        drawCube(vao,shader,Mat4::translation({creatureX,2.55f,creatureZ})*
                 Mat4::scale({0.8f,0.8f,0.7f}),{0.012f,0.012f,0.016f},
                 vp,camera,light,forward,flashlight,time);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
