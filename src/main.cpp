#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "engine/math/math.hpp"
#include "engine/graphics/opengl.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/input/input.hpp"

using namespace cry;

enum class Material : int {
    Concrete = 0,
    Metal = 1,
    Wood = 2,
    DirtyTile = 3,
    Lamp = 4,
    Black = 5,
    Rust = 6
};

static void drawCube(
    unsigned vao,
    Shader& shader,
    const Mat4& model,
    const Vec3& color,
    const Mat4& vp,
    const Vec3& camera,
    const Vec3& light,
    const Vec3& lightColor,
    const Vec3& flashPos,
    const Vec3& flashDir,
    bool flashlight,
    float time,
    Material material
) {
    shader.bind();
    shader.setMat4("uModel", model);
    shader.setMat4("uMVP", vp * model);
    shader.setVec3("uColor", color);
    shader.setVec3("uCamera", camera);
    shader.setVec3("uLightPos", light);
    shader.setVec3("uLightColor", lightColor);
    shader.setVec3("uFlashPos", flashPos);
    shader.setVec3("uFlashDir", flashDir);
    shader.setFloat("uFlashOn", flashlight ? 1.0f : 0.0f);
    shader.setFloat("uTime", time);
    shader.setInt("uMaterial", static_cast<int>(material));

    gl::BindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

static Vec3 collideRoom(Vec3 p) {
    p.x = std::clamp(p.x, -6.15f, 6.15f);
    p.z = std::clamp(p.z, -6.15f, 6.15f);
    p.y = 1.62f;
    return p;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "CryHorror Engine", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gl::load()) {
        std::cerr << "OpenGL 3.3 function loading failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Shader shader;
    if (!shader.create("assets/shaders/world.vert", "assets/shaders/world.frag")) {
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
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::GenBuffers(1, &vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER, vbo);
    gl::BufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    gl::EnableVertexAttribArray(0);

    Input input(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Vec3 camera{0.0f, 1.62f, 5.0f};
    float yaw = -1.5708f;
    float pitch = -0.035f;
    bool flashlight = true;
    bool lastF = false;
    bool lastE = false;
    bool doorOpen = false;
    float doorAngle = 0.0f;
    float bobTime = 0.0f;

    const auto start = std::chrono::steady_clock::now();
    float previousTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        const float time = std::chrono::duration<float>(
            std::chrono::steady_clock::now() - start).count();

        float dt = std::clamp(time - previousTime, 0.001f, 0.033f);
        previousTime = time;

        glfwPollEvents();
        input.beginFrame();

        if (input.down(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        const Vec3 md = input.mouseDelta();
        constexpr float mouseSensitivity = 0.00225f;
        yaw += md.x * mouseSensitivity;
        pitch -= md.y * mouseSensitivity;
        pitch = std::clamp(pitch, -1.35f, 1.35f);

        const Vec3 forward{
            std::cos(pitch) * std::cos(yaw),
            std::sin(pitch),
            std::cos(pitch) * std::sin(yaw)
        };

        const Vec3 flatForward{std::cos(yaw), 0.0f, std::sin(yaw)};

        // Correct right vector: with the default -Z facing direction,
        // D must move toward +X rather than -X.
        const Vec3 right{-std::sin(yaw), 0.0f, std::cos(yaw)};

        const float speed = input.down(GLFW_KEY_LEFT_SHIFT) ? 3.5f : 2.15f;
        Vec3 movement{};

        if (input.down(GLFW_KEY_W)) movement += flatForward;
        if (input.down(GLFW_KEY_S)) movement -= flatForward;
        if (input.down(GLFW_KEY_D)) movement += right;
        if (input.down(GLFW_KEY_A)) movement -= right;

        const float movementLength = movement.length();
        if (movementLength > 0.001f) {
            movement = movement.normalized();
            camera += movement * speed * dt;
            bobTime += dt * (input.down(GLFW_KEY_LEFT_SHIFT) ? 9.0f : 6.0f);
        }

        camera = collideRoom(camera);

        const bool fNow = input.down(GLFW_KEY_F);
        if (fNow && !lastF)
            flashlight = !flashlight;
        lastF = fNow;

        const bool eNow = input.down(GLFW_KEY_E);
        if (eNow && !lastE && camera.z < -4.8f && std::abs(camera.x) < 2.2f)
            doorOpen = !doorOpen;
        lastE = eNow;

        const float targetDoor = doorOpen ? 1.28f : 0.0f;
        doorAngle += (targetDoor - doorAngle) * std::min(1.0f, dt * 7.5f);

        const float bob = movementLength > 0.001f
            ? std::sin(bobTime) * 0.018f
            : 0.0f;

        const Vec3 renderCamera = camera + Vec3{0.0f, bob, 0.0f};

        const Vec3 flashDir = forward.normalized();
        const Vec3 flashPos = renderCamera + Vec3{0.0f, 0.02f, 0.0f};

        int width = 1, height = 1;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.006f, 0.007f, 0.009f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const Mat4 projection = Mat4::perspective(
            1.02f,
            static_cast<float>(width) / static_cast<float>(height),
            0.05f,
            45.0f
        );

        const Mat4 view = Mat4::lookAt(
            renderCamera,
            renderCamera + flashDir,
            {0.0f, 1.0f, 0.0f}
        );

        const Mat4 vp = projection * view;

        const Vec3 ceilingLight{0.0f, 4.35f, -1.0f};
        const Vec3 ceilingLightColor{1.0f, 0.78f, 0.52f};

        // Floor.
        drawCube(vao, shader,
            Mat4::translation({0.0f, -0.18f, 0.0f}) * Mat4::scale({13.6f, 0.35f, 13.6f}),
            {0.19f, 0.18f, 0.16f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::DirtyTile);

        // Ceiling.
        drawCube(vao, shader,
            Mat4::translation({0.0f, 6.0f, 0.0f}) * Mat4::scale({13.6f, 0.35f, 13.6f}),
            {0.055f, 0.057f, 0.06f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Concrete);

        // Side and back walls.
        drawCube(vao, shader,
            Mat4::translation({-6.8f, 2.9f, 0.0f}) * Mat4::scale({0.35f, 5.8f, 13.6f}),
            {0.14f, 0.145f, 0.15f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Concrete);

        drawCube(vao, shader,
            Mat4::translation({6.8f, 2.9f, 0.0f}) * Mat4::scale({0.35f, 5.8f, 13.6f}),
            {0.14f, 0.145f, 0.15f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Concrete);

        drawCube(vao, shader,
            Mat4::translation({0.0f, 2.9f, -6.8f}) * Mat4::scale({13.6f, 5.8f, 0.35f}),
            {0.11f, 0.115f, 0.12f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Concrete);

        drawCube(vao, shader,
            Mat4::translation({0.0f, 2.9f, 6.8f}) * Mat4::scale({13.6f, 5.8f, 0.35f}),
            {0.11f, 0.115f, 0.12f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Concrete);

        // Wall panels / trim: break up the flat primitive look.
        for (int z = -5; z <= 5; z += 2) {
            drawCube(vao, shader,
                Mat4::translation({-6.57f, 2.15f, static_cast<float>(z)}) *
                Mat4::scale({0.08f, 2.0f, 0.055f}),
                {0.06f, 0.062f, 0.065f}, vp, renderCamera,
                ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
                Material::Metal);

            drawCube(vao, shader,
                Mat4::translation({6.57f, 2.15f, static_cast<float>(z)}) *
                Mat4::scale({0.08f, 2.0f, 0.055f}),
                {0.06f, 0.062f, 0.065f}, vp, renderCamera,
                ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
                Material::Metal);
        }

        // Ceiling beams.
        for (int z = -5; z <= 5; z += 2) {
            drawCube(vao, shader,
                Mat4::translation({0.0f, 5.72f, static_cast<float>(z)}) *
                Mat4::scale({13.1f, 0.18f, 0.28f}),
                {0.055f, 0.05f, 0.045f}, vp, renderCamera,
                ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
                Material::Wood);
        }

        // Flickering ceiling lamps.
        for (float z : {-4.5f, -1.0f, 2.5f}) {
            const float pulse = 0.72f + 0.18f * std::sin(time * 8.0f + z * 2.0f);
            drawCube(vao, shader,
                Mat4::translation({0.0f, 5.72f, z}) * Mat4::scale({1.15f, 0.08f, 0.28f}),
                {0.48f * pulse, 0.37f * pulse, 0.23f * pulse},
                vp, renderCamera,
                {0.0f, 5.15f, z}, ceilingLightColor,
                flashPos, flashDir, flashlight, time, Material::Lamp);
        }

        // Old wooden table.
        drawCube(vao, shader,
            Mat4::translation({-3.7f, 1.05f, -0.6f}) * Mat4::scale({2.8f, 0.18f, 1.25f}),
            {0.16f, 0.095f, 0.055f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Wood);

        for (float x : {-4.8f, -2.6f}) {
            for (float z : {-1.0f, -0.1f}) {
                drawCube(vao, shader,
                    Mat4::translation({x, 0.45f, z}) * Mat4::scale({0.16f, 1.0f, 0.16f}),
                    {0.10f, 0.058f, 0.035f}, vp, renderCamera,
                    ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
                    Material::Wood);
            }
        }

        // Tall cabinet.
        drawCube(vao, shader,
            Mat4::translation({4.65f, 1.75f, -2.8f}) * Mat4::scale({1.7f, 3.5f, 0.85f}),
            {0.085f, 0.09f, 0.095f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Metal);

        for (int i = 0; i < 4; ++i) {
            drawCube(vao, shader,
                Mat4::translation({4.65f, 0.75f + i * 0.68f, -2.33f}) *
                Mat4::scale({1.25f, 0.055f, 0.04f}),
                {0.035f, 0.038f, 0.04f}, vp, renderCamera,
                ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
                Material::Metal);
        }

        // Pipes along the right wall.
        for (int i = 0; i < 3; ++i) {
            drawCube(vao, shader,
                Mat4::translation({6.48f, 1.15f + i * 0.55f, -1.0f}) *
                Mat4::scale({0.12f, 0.12f, 8.0f}),
                {0.09f, 0.075f, 0.06f}, vp, renderCamera,
                ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
                Material::Rust);
        }

        // Door and frame.
        const Mat4 door =
            Mat4::translation({0.0f, 1.45f, -6.57f}) *
            Mat4::translation({1.25f, 0.0f, 0.0f}) *
            Mat4::rotationY(doorAngle) *
            Mat4::translation({-1.25f, 0.0f, 0.0f}) *
            Mat4::scale({2.5f, 2.9f, 0.18f});

        drawCube(vao, shader, door,
            {0.12f, 0.065f, 0.038f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Wood);

        drawCube(vao, shader,
            Mat4::translation({-1.55f, 1.45f, -6.55f}) * Mat4::scale({0.22f, 3.15f, 0.32f}),
            {0.065f, 0.055f, 0.045f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Metal);

        drawCube(vao, shader,
            Mat4::translation({1.55f, 1.45f, -6.55f}) * Mat4::scale({0.22f, 3.15f, 0.32f}),
            {0.065f, 0.055f, 0.045f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Metal);

        drawCube(vao, shader,
            Mat4::translation({0.0f, 3.0f, -6.55f}) * Mat4::scale({3.32f, 0.22f, 0.32f}),
            {0.065f, 0.055f, 0.045f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Metal);

        // A restrained silhouette at the far end. It stays mostly in darkness.
        const float creatureX = 2.6f + std::sin(time * 0.23f) * 0.12f;
        const float creatureZ = -4.55f;

        drawCube(vao, shader,
            Mat4::translation({creatureX, 1.25f, creatureZ}) * Mat4::scale({0.52f, 2.5f, 0.42f}),
            {0.006f, 0.006f, 0.007f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Black);

        drawCube(vao, shader,
            Mat4::translation({creatureX, 2.88f, creatureZ}) * Mat4::scale({0.68f, 0.72f, 0.58f}),
            {0.006f, 0.006f, 0.007f}, vp, renderCamera,
            ceilingLight, ceilingLightColor, flashPos, flashDir, flashlight, time,
            Material::Black);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
