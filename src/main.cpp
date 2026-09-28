#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>

#include "engine/math/math.hpp"
#include "engine/graphics/opengl.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/input/input.hpp"

using namespace cry;

enum class Material : int {
    Ground,
    Bark,
    Moss,
    Rock,
    Wood,
    Metal,
    WarmLight,
    ColdLight,
    Black,
    Water
};

struct SceneLight {
    Vec3 position;
    Vec3 color;
    float radius;
    float intensity;
};

static void drawCube(
    unsigned vao, Shader& shader, const Mat4& model, const Vec3& color,
    const Mat4& vp, const Vec3& camera, const std::vector<SceneLight>& lights,
    const Vec3& flashPos, const Vec3& flashDir, bool flashlight,
    float time, Material material
) {
    shader.bind();
    shader.setMat4("uModel", model);
    shader.setMat4("uMVP", vp * model);
    shader.setVec3("uColor", color);
    shader.setVec3("uCamera", camera);
    shader.setVec3("uFlashPos", flashPos);
    shader.setVec3("uFlashDir", flashDir);
    shader.setFloat("uFlashOn", flashlight ? 1.0f : 0.0f);
    shader.setFloat("uTime", time);
    shader.setInt("uMaterial", static_cast<int>(material));

    // The shader supports two local lights: moon and cabin.
    for (int i = 0; i < 2; ++i) {
        shader.setVec3(i == 0 ? "uLight0Pos" : "uLight1Pos", lights[i].position);
        shader.setVec3(i == 0 ? "uLight0Color" : "uLight1Color", lights[i].color);
        shader.setFloat(i == 0 ? "uLight0Radius" : "uLight1Radius", lights[i].radius);
        shader.setFloat(i == 0 ? "uLight0Intensity" : "uLight1Intensity", lights[i].intensity);
    }

    gl::BindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

static void drawTree(
    unsigned vao, Shader& shader, const Mat4& vp, const Vec3& camera,
    const std::vector<SceneLight>& lights, const Vec3& flashPos,
    const Vec3& flashDir, bool flashlight, float time,
    float x, float z, float scale, float lean
) {
    const Vec3 bark{0.095f, 0.075f, 0.055f};
    const Vec3 darkBark{0.055f, 0.045f, 0.038f};

    Mat4 trunk =
        Mat4::translation({x, 2.2f * scale, z}) *
        Mat4::rotationY(lean) *
        Mat4::scale({0.46f * scale, 4.4f * scale, 0.46f * scale});

    drawCube(vao, shader, trunk, bark, vp, camera, lights,
             flashPos, flashDir, flashlight, time, Material::Bark);

    // Angular branches create a convincing silhouette without needing external models.
    const float h = 2.5f * scale;
    drawCube(vao, shader,
        Mat4::translation({x - 0.48f * scale, h, z}) *
        Mat4::rotationY(lean - 0.75f) *
        Mat4::scale({1.45f * scale, 0.16f * scale, 0.16f * scale}),
        darkBark, vp, camera, lights, flashPos, flashDir, flashlight, time, Material::Bark);

    drawCube(vao, shader,
        Mat4::translation({x + 0.52f * scale, 3.15f * scale, z + 0.05f}) *
        Mat4::rotationY(lean + 0.62f) *
        Mat4::scale({1.35f * scale, 0.14f * scale, 0.14f * scale}),
        darkBark, vp, camera, lights, flashPos, flashDir, flashlight, time, Material::Bark);

    drawCube(vao, shader,
        Mat4::translation({x - 0.28f * scale, 4.05f * scale, z}) *
        Mat4::rotationY(lean - 0.45f) *
        Mat4::scale({1.15f * scale, 0.12f * scale, 0.12f * scale}),
        darkBark, vp, camera, lights, flashPos, flashDir, flashlight, time, Material::Bark);

    // Sparse dark foliage clumps.
    for (int i = 0; i < 3; ++i) {
        const float y = (2.9f + i * 0.72f) * scale;
        const float side = (i % 2 == 0 ? -0.42f : 0.38f) * scale;
        drawCube(vao, shader,
            Mat4::translation({x + side, y, z}) *
            Mat4::scale({1.25f * scale, 0.5f * scale, 0.85f * scale}),
            {0.025f, 0.052f, 0.035f}, vp, camera, lights,
            flashPos, flashDir, flashlight, time, Material::Moss);
    }
}

static void drawRock(
    unsigned vao, Shader& shader, const Mat4& vp, const Vec3& camera,
    const std::vector<SceneLight>& lights, const Vec3& flashPos,
    const Vec3& flashDir, bool flashlight, float time,
    float x, float y, float z, float sx, float sy, float sz
) {
    drawCube(vao, shader,
        Mat4::translation({x, y, z}) * Mat4::rotationY(x * 0.4f) * Mat4::scale({sx, sy, sz}),
        {0.105f, 0.115f, 0.105f}, vp, camera, lights,
        flashPos, flashDir, flashlight, time, Material::Rock);
}


struct Mesh {
    unsigned vao{};
    unsigned vbo{};
    int count{};
};

static Mesh makeMesh(const std::vector<float>& v) {
    Mesh m{};
    gl::GenVertexArrays(1,&m.vao);
    gl::BindVertexArray(m.vao);
    gl::GenBuffers(1,&m.vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER,m.vbo);
    gl::BufferData(GL_ARRAY_BUFFER,static_cast<std::ptrdiff_t>(v.size()*sizeof(float)),v.data(),GL_STATIC_DRAW);
    gl::VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),nullptr);
    gl::EnableVertexAttribArray(0);
    m.count=static_cast<int>(v.size()/3);
    return m;
}
static void tri(std::vector<float>& v,Vec3 a,Vec3 b,Vec3 c) {
    v.insert(v.end(),{a.x,a.y,a.z,b.x,b.y,b.z,c.x,c.y,c.z});
}
static void frustum(std::vector<float>& v,float y0,float y1,float r0,float r1,int n) {
    const float tau=6.28318530718f;
    for(int i=0;i<n;i++){
        float a=tau*i/n,b=tau*(i+1)/n;
        Vec3 p0{std::cos(a)*r0,y0,std::sin(a)*r0},p1{std::cos(b)*r0,y0,std::sin(b)*r0};
        Vec3 q0{std::cos(a)*r1,y1,std::sin(a)*r1},q1{std::cos(b)*r1,y1,std::sin(b)*r1};
        tri(v,p0,q0,q1); tri(v,p0,q1,p1);
    }
}
static Mesh makePine() {
    std::vector<float> v;
    frustum(v,0,2.4f,.24f,.15f,10);
    frustum(v,1.0f,3.0f,1.18f,.07f,14);
    frustum(v,1.9f,4.1f,.92f,.06f,14);
    frustum(v,2.8f,5.25f,.66f,.025f,14);
    return makeMesh(v);
}
static Mesh makeRock() {
    std::vector<float> v;
    const int n=14;
    for(int r=0;r<3;r++){
        float y0=-.42f+r*.38f,y1=y0+.38f;
        float r0=1.0f-r*.18f,r1=1.0f-(r+1)*.18f;
        for(int i=0;i<n;i++){
            float a=6.2831853f*i/n,b=6.2831853f*(i+1)/n;
            float w0=.82f+.18f*std::sin(i*4.7f+r),w1=.82f+.18f*std::sin((i+1)*4.7f+r);
            Vec3 p0{std::cos(a)*r0*w0,y0,std::sin(a)*r0*w0};
            Vec3 p1{std::cos(b)*r0*w1,y0,std::sin(b)*r0*w1};
            Vec3 q0{std::cos(a)*r1*w0,y1,std::sin(a)*r1*w0};
            Vec3 q1{std::cos(b)*r1*w1,y1,std::sin(b)*r1*w1};
            tri(v,p0,q0,q1);tri(v,p0,q1,p1);
        }
    }
    return makeMesh(v);
}
static Mesh makeGround() {
    std::vector<float> v; const int n=32; const float size=44;
    auto h=[](float x,float z){return .08f*std::sin(x*.55f+z*.21f)+.035f*std::sin(z*1.7f-x*.3f);};
    for(int z=0;z<n;z++)for(int x=0;x<n;x++){
        float x0=-size/2+size*x/n,x1=-size/2+size*(x+1)/n;
        float z0=-size/2+size*z/n,z1=-size/2+size*(z+1)/n;
        Vec3 a{x0,h(x0,z0),z0},b{x1,h(x1,z0),z0},c{x1,h(x1,z1),z1},d{x0,h(x0,z1),z1};
        tri(v,a,b,c);tri(v,a,c,d);
    }
    return makeMesh(v);
}
static void drawMesh(const Mesh& m,Shader& shader,const Mat4& model,const Vec3& color,const Mat4& vp,
    const Vec3& camera,const std::vector<SceneLight>& lights,const Vec3& flashPos,const Vec3& flashDir,
    bool flashlight,float time,Material material){
    shader.bind(); shader.setMat4("uModel",model); shader.setMat4("uMVP",vp*model);
    shader.setVec3("uColor",color);shader.setVec3("uCamera",camera);shader.setVec3("uFlashPos",flashPos);
    shader.setVec3("uFlashDir",flashDir);shader.setFloat("uFlashOn",flashlight?1.f:0.f);
    shader.setFloat("uTime",time);shader.setInt("uMaterial",static_cast<int>(material));
    for(int i=0;i<2;i++){shader.setVec3(i?"uLight1Pos":"uLight0Pos",lights[i].position);
        shader.setVec3(i?"uLight1Color":"uLight0Color",lights[i].color);
        shader.setFloat(i?"uLight1Radius":"uLight0Radius",lights[i].radius);
        shader.setFloat(i?"uLight1Intensity":"uLight0Intensity",lights[i].intensity);}
    gl::BindVertexArray(m.vao);glDrawArrays(GL_TRIANGLES,0,m.count);
}

int main() {
    if(!glfwInit()){std::cerr<<"GLFW initialization failed\n";return 1;}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);glfwWindowHint(GLFW_SAMPLES,4);
    GLFWwindow* window=glfwCreateWindow(1280,720,"CryHorror Engine - The Black Forest",nullptr,nullptr);
    if(!window){glfwTerminate();return 1;}
    glfwMakeContextCurrent(window);glfwSwapInterval(1);
    if(!gl::load()){std::cerr<<"OpenGL loading failed\n";glfwDestroyWindow(window);glfwTerminate();return 1;}
    glEnable(GL_DEPTH_TEST);glEnable(GL_CULL_FACE);glCullFace(GL_BACK);glEnable(GL_MULTISAMPLE);

    Shader shader;
    if(!shader.create("assets/shaders/world.vert","assets/shaders/world.frag")){glfwDestroyWindow(window);glfwTerminate();return 1;}

    constexpr float cube[]={
      -0.5f,-0.5f,-0.5f, .5f,-.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,-.5f,-.5f,.5f,-.5f,-.5f,-.5f,-.5f,
      -.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,.5f,.5f,.5f,.5f,.5f,-.5f,.5f,.5f,-.5f,-.5f,.5f,
      -.5f,.5f,.5f,-.5f,.5f,-.5f,-.5f,-.5f,-.5f,-.5f,-.5f,-.5f,-.5f,-.5f,.5f,-.5f,.5f,.5f,
      .5f,.5f,.5f,.5f,.5f,-.5f,.5f,-.5f,-.5f,.5f,-.5f,-.5f,.5f,-.5f,.5f,.5f,.5f,.5f,
      -.5f,-.5f,-.5f,.5f,-.5f,-.5f,.5f,-.5f,.5f,.5f,-.5f,.5f,-.5f,-.5f,.5f,-.5f,-.5f,-.5f,
      -.5f,.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,.5f,.5f,.5f,.5f,-.5f,.5f,.5f,-.5f,.5f,-.5f};
    Mesh cubeMesh=makeMesh(std::vector<float>(cube,cube+sizeof(cube)/sizeof(float)));
    Mesh pine=makePine(),rock=makeRock(),ground=makeGround();

    Input input(window);glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    Vec3 camera{0,1.68f,6.5f};float yaw=-1.5708f,pitch=-.06f,flashYaw=yaw,flashPitch=pitch;
    bool flashlight=true,lastF=false;float walkTime=0;
    const auto startTime=std::chrono::steady_clock::now();float previous=0;

    while(!glfwWindowShouldClose(window)){
        float time=std::chrono::duration<float>(std::chrono::steady_clock::now()-startTime).count();
        float dt=std::clamp(time-previous,.001f,.033f);previous=time;
        glfwPollEvents();input.beginFrame();
        if(input.down(GLFW_KEY_ESCAPE))glfwSetWindowShouldClose(window,GLFW_TRUE);

        Vec3 md=input.mouseDelta();
        constexpr float sens=.0019f;
        yaw += md.x*sens; pitch -= md.y*sens; pitch=std::clamp(pitch,-1.28f,1.28f);
        Vec3 forward{std::cos(pitch)*std::cos(yaw),std::sin(pitch),std::cos(pitch)*std::sin(yaw)};
        Vec3 flat{std::cos(yaw),0,std::sin(yaw)},right{-std::sin(yaw),0,std::cos(yaw)};
        Vec3 move{};
        if(input.down(GLFW_KEY_W))move+=flat;if(input.down(GLFW_KEY_S))move-=flat;
        if(input.down(GLFW_KEY_D))move+=right;if(input.down(GLFW_KEY_A))move-=right;
        float moving=move.length();
        if(moving>.001f){move=move.normalized();camera+=move*(input.down(GLFW_KEY_LEFT_SHIFT)?3.1f:1.9f)*dt;walkTime+=dt*7;}
        camera.x=std::clamp(camera.x,-18.f,18.f);camera.z=std::clamp(camera.z,-20.f,20.f);camera.y=1.68f;

        bool f=input.down(GLFW_KEY_F);if(f&&!lastF)flashlight=!flashlight;lastF=f;
        flashYaw+=(yaw-flashYaw)*std::min(1.f,dt*14.f);flashPitch+=(pitch-flashPitch)*std::min(1.f,dt*14.f);
        Vec3 flashDir{std::cos(flashPitch)*std::cos(flashYaw),std::sin(flashPitch),std::cos(flashPitch)*std::sin(flashYaw)};
        float bob=moving>.001f?std::sin(walkTime)*.014f:0;
        Vec3 renderCamera=camera+Vec3{0,bob,0},flashPos=renderCamera+flashDir*.25f;

        int width=1,height=1;glfwGetFramebufferSize(window,&width,&height);glViewport(0,0,width,height);
        glClearColor(.002f,.004f,.006f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        Mat4 projection=Mat4::perspective(.82f,float(width)/float(height),.05f,85.f);
        Mat4 view=Mat4::lookAt(renderCamera,renderCamera+forward,{0,1,0});Mat4 vp=projection*view;

        std::vector<SceneLight> lights{
            {{-9,11,7},{.28f,.40f,.62f},42,2.2f},
            {{-4,3,-15},{1,.26f,.075f},10,5.5f}
        };

        drawMesh(ground,shader,Mat4::identity(),{.075f,.082f,.062f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Ground);
        drawCube(cubeMesh.vao,shader,Mat4::translation({0,-.13f,-4})*Mat4::scale({3.8f,.045f,35}),
            {.10f,.075f,.048f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Ground);
        drawCube(cubeMesh.vao,shader,Mat4::translation({-7.1f,-.01f,-5})*Mat4::scale({2.4f,.03f,27}),
            {.012f,.025f,.027f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Water);

        const float trees[][4]={
            {-13,15,1.55f,.1f},{-9,17,1.15f,-.1f},{-4,19,1.7f,.08f},{2,18,1.3f,-.05f},{8,17,1.8f,.1f},{14,14,1.45f,-.08f},
            {-16,9,1.6f,-.1f},{-12,5,1.15f,.05f},{12,6,1.5f,.1f},{16,2,1.8f,-.12f},
            {-16,-4,1.55f,.1f},{14,-5,1.35f,-.08f},{-15,-12,1.8f,-.1f},{-9,-16,1.45f,.08f},{4,-18,1.75f,-.08f},{13,-15,1.5f,.12f}
        };
        for(const auto&t:trees){
            float sway=std::sin(time*.7f+t[0]*.3f)*.018f;
            drawMesh(pine,shader,Mat4::translation({t[0],0,t[1]})*Mat4::rotationY(t[3]+sway)*Mat4::scale({t[2],t[2],t[2]}),
                {.026f,.070f,.038f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Moss);
        }

        const float rocks[][6]={{-3.5f,.45f,-2,1.2f,.55f,.9f},{3.5f,.42f,-6,1.5f,.6f,1.1f},{-5.5f,.35f,4,.9f,.5f,1.25f},{6,.28f,7,1.1f,.4f,.8f}};
        for(const auto&r:rocks)drawMesh(rock,shader,Mat4::translation({r[0],r[1],r[2]})*Mat4::rotationY(r[0]),{.09f,.095f,.085f},
            vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Rock);

        // Cabin: layered timber walls, overhanging roof, porch and warm windows.
        drawCube(cubeMesh.vao,shader,Mat4::translation({-4,1.65f,-16})*Mat4::scale({6.4f,3.3f,4.2f}),
            {.085f,.058f,.038f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Wood);
        drawCube(cubeMesh.vao,shader,Mat4::translation({-4,3.62f,-16})*Mat4::rotationY(.785f)*Mat4::scale({5.1f,.48f,5.1f}),
            {.025f,.028f,.027f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Metal);
        drawCube(cubeMesh.vao,shader,Mat4::translation({-4,1.3f,-13.83f})*Mat4::scale({1.25f,2.5f,.12f}),
            {.035f,.024f,.018f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Wood);
        for(float x:{-5.7f,-2.3f})drawCube(cubeMesh.vao,shader,Mat4::translation({x,1.85f,-13.84f})*Mat4::scale({1.3f,1.15f,.08f}),
            {.95f,.28f,.055f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::WarmLight);

        // Broken fence and fallen tree.
        for(int i=0;i<6;i++)drawCube(cubeMesh.vao,shader,Mat4::translation({-2.8f+i*.9f,.65f,-10.2f})*
            Mat4::rotationY((i%2?-1:1)*.22f)*Mat4::scale({.12f,1.3f,.12f}),{.055f,.034f,.02f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Wood);
        drawCube(cubeMesh.vao,shader,Mat4::translation({-4.8f,.48f,-8.8f})*Mat4::rotationY(.32f)*Mat4::scale({5.2f,.5f,.55f}),
            {.06f,.038f,.023f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Wood);

        // Distant silhouette + tiny red beacon.
        float sx=.65f+std::sin(time*.17f)*.12f;
        drawCube(cubeMesh.vao,shader,Mat4::translation({sx,1.5f,-11.0f})*Mat4::scale({.42f,2.8f,.32f}),
            {.002f,.002f,.002f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Black);
        drawCube(cubeMesh.vao,shader,Mat4::translation({sx,3.28f,-11.0f})*Mat4::scale({.55f,.58f,.45f}),
            {.002f,.002f,.002f},vp,renderCamera,lights,flashPos,flashDir,flashlight,time,Material::Black);

        glfwSwapBuffers(window);
    }
    glfwDestroyWindow(window);glfwTerminate();return 0;
}
