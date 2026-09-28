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

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "CryHorror Engine - The Black Forest", nullptr, nullptr);
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

    Vec3 camera{0.0f, 1.65f, 5.8f};
    float yaw = -1.5708f;
    float pitch = -0.045f;

    // Independent flashlight orientation prevents the beam from snapping with the camera.
    float flashlightYaw = yaw;
    float flashlightPitch = pitch;

    bool flashlight = true;
    bool lastF = false;
    float walkTime = 0.0f;

    const auto start = std::chrono::steady_clock::now();
    float previousTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        const float time = std::chrono::duration<float>(
            std::chrono::steady_clock::now() - start).count();

        const float dt = std::clamp(time - previousTime, 0.001f, 0.033f);
        previousTime = time;

        glfwPollEvents();
        input.beginFrame();

        if (input.down(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        const Vec3 md = input.mouseDelta();

        // Standard FPS convention: mouse right = look right.
        constexpr float mouseSensitivity = 0.00215f;
        yaw += md.x * mouseSensitivity;
        pitch -= md.y * mouseSensitivity;
        pitch = std::clamp(pitch, -1.30f, 1.30f);

        const Vec3 forward{
            std::cos(pitch) * std::cos(yaw),
            std::sin(pitch),
            std::cos(pitch) * std::sin(yaw)
        };

        const Vec3 flatForward{std::cos(yaw), 0.0f, std::sin(yaw)};
        const Vec3 right{-std::sin(yaw), 0.0f, std::cos(yaw)};

        const float speed = input.down(GLFW_KEY_LEFT_SHIFT) ? 3.2f : 2.0f;
        Vec3 movement{};

        if (input.down(GLFW_KEY_W)) movement += flatForward;
        if (input.down(GLFW_KEY_S)) movement -= flatForward;
        if (input.down(GLFW_KEY_D)) movement += right;
        if (input.down(GLFW_KEY_A)) movement -= right;

        const float moving = movement.length();
        if (moving > 0.001f) {
            movement = movement.normalized();
            camera += movement * speed * dt;
            walkTime += dt * (input.down(GLFW_KEY_LEFT_SHIFT) ? 10.0f : 6.5f);
        }

        // Forest boundary.
        camera.x = std::clamp(camera.x, -18.0f, 18.0f);
        camera.z = std::clamp(camera.z, -20.0f, 20.0f);
        camera.y = 1.65f;

        const bool fNow = input.down(GLFW_KEY_F);
        if (fNow && !lastF)
            flashlight = !flashlight;
        lastF = fNow;

        // Smooth the flashlight separately from the view.
        flashlightYaw += (yaw - flashlightYaw) * std::min(1.0f, dt * 12.0f);
        flashlightPitch += (pitch - flashlightPitch) * std::min(1.0f, dt * 12.0f);

        const Vec3 flashDir{
            std::cos(flashlightPitch) * std::cos(flashlightYaw),
            std::sin(flashlightPitch),
            std::cos(flashlightPitch) * std::sin(flashlightYaw)
        };

        const float bob = moving > 0.001f ? std::sin(walkTime) * 0.012f : 0.0f;
        const Vec3 renderCamera = camera + Vec3{0.0f, bob, 0.0f};
        const Vec3 flashPos = renderCamera + flashDir * 0.22f;

        int width = 1, height = 1;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.004f, 0.007f, 0.010f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const Mat4 projection = Mat4::perspective(
            0.91f,
            static_cast<float>(width) / static_cast<float>(height),
            0.05f,
            80.0f
        );

        const Mat4 view = Mat4::lookAt(
            renderCamera,
            renderCamera + forward,
            {0.0f, 1.0f, 0.0f}
        );

        const Mat4 vp = projection * view;

        // Cold moonlight + a distant warm cabin light.
        std::vector<SceneLight> lights{
            {{-8.0f, 12.0f, 6.0f}, {0.32f, 0.43f, 0.62f}, 38.0f, 2.0f},
            {{-4.0f, 2.5f, -15.5f}, {1.0f, 0.38f, 0.12f}, 9.0f, 4.0f}
        };

        // Ground plane.
        drawCube(vao, shader,
            Mat4::translation({0.0f, -0.25f, 0.0f}) * Mat4::scale({44.0f, 0.5f, 48.0f}),
            {0.085f, 0.095f, 0.075f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Ground);

        // Muddy path.
        drawCube(vao, shader,
            Mat4::translation({0.0f, 0.015f, -4.0f}) * Mat4::scale({4.0f, 0.035f, 36.0f}),
            {0.095f, 0.075f, 0.055f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Ground);

        // Shallow black-water ditch.
        drawCube(vao, shader,
            Mat4::translation({-7.2f, -0.01f, -5.0f}) * Mat4::scale({2.2f, 0.025f, 26.0f}),
            {0.018f, 0.028f, 0.026f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Water);

        // Dense forest: art-directed rather than a random empty grid.
        const float treeData[][4] = {
            {-11,  13, 1.35f, -0.10f}, {-7,  16, 1.15f, 0.16f}, {-2,  18, 1.55f, -0.05f},
            {  5,  17, 1.30f, 0.08f}, {11,  14, 1.55f, -0.14f},
            {-15,  7, 1.50f, 0.10f}, {-10,  5, 1.05f, -0.08f}, {9,  6, 1.30f, 0.04f},
            {14,  3, 1.55f, -0.10f}, {-16, -2, 1.30f, 0.12f}, {13, -3, 1.15f, -0.06f},
            {-15, -9, 1.55f, -0.10f}, {14, -10, 1.40f, 0.08f},
            {-13, -16, 1.75f, -0.08f}, {-7, -18, 1.40f, 0.10f}, {7, -18, 1.65f, -0.12f},
            {14, -17, 1.30f, 0.07f}
        };

        for (const auto& t : treeData)
            drawTree(vao, shader, vp, renderCamera, lights, flashPos, flashDir,
                     flashlight, time, t[0], t[1], t[2], t[3]);

        // Fallen trunks and rocks along the path.
        drawCube(vao, shader,
            Mat4::translation({-4.8f, 0.42f, -9.0f}) *
            Mat4::rotationY(0.28f) * Mat4::scale({5.2f, 0.55f, 0.65f}),
            {0.075f, 0.048f, 0.032f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Wood);

        drawCube(vao, shader,
            Mat4::translation({5.0f, 0.36f, 1.5f}) *
            Mat4::rotationY(-0.42f) * Mat4::scale({4.0f, 0.42f, 0.55f}),
            {0.065f, 0.043f, 0.03f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Wood);

        drawRock(vao, shader, vp, renderCamera, lights, flashPos, flashDir, flashlight, time,
                 -3.5f, 0.35f, -2.0f, 1.1f, 0.7f, 0.85f);
        drawRock(vao, shader, vp, renderCamera, lights, flashPos, flashDir, flashlight, time,
                 3.8f, 0.28f, -6.0f, 1.5f, 0.55f, 1.0f);
        drawRock(vao, shader, vp, renderCamera, lights, flashPos, flashDir, flashlight, time,
                 -5.5f, 0.22f, 5.0f, 0.9f, 0.45f, 1.25f);

        // Abandoned ranger cabin at the end of the trail.
        drawCube(vao, shader,
            Mat4::translation({-4.0f, 1.7f, -16.0f}) * Mat4::scale({6.0f, 3.4f, 4.0f}),
            {0.105f, 0.075f, 0.052f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Wood);

        // Roof.
        drawCube(vao, shader,
            Mat4::translation({-4.0f, 3.65f, -16.0f}) *
            Mat4::rotationY(0.785f) * Mat4::scale({5.0f, 0.45f, 5.0f}),
            {0.045f, 0.042f, 0.038f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Metal);

        // Door.
        drawCube(vao, shader,
            Mat4::translation({-4.0f, 1.35f, -13.92f}) * Mat4::scale({1.25f, 2.5f, 0.12f}),
            {0.045f, 0.034f, 0.026f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Wood);

        // Warm windows: the only strong safe-looking color in the scene.
        drawCube(vao, shader,
            Mat4::translation({-5.65f, 1.85f, -13.91f}) * Mat4::scale({1.25f, 1.15f, 0.08f}),
            {0.72f, 0.28f, 0.075f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::WarmLight);

        drawCube(vao, shader,
            Mat4::translation({-2.35f, 1.85f, -13.91f}) * Mat4::scale({1.25f, 1.15f, 0.08f}),
            {0.72f, 0.28f, 0.075f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::WarmLight);

        // Small sign by the path.
        drawCube(vao, shader,
            Mat4::translation({2.8f, 1.15f, -11.5f}) *
            Mat4::rotationY(-0.35f) * Mat4::scale({0.12f, 2.1f, 0.12f}),
            {0.055f, 0.04f, 0.025f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Wood);

        drawCube(vao, shader,
            Mat4::translation({2.65f, 1.9f, -11.5f}) *
            Mat4::rotationY(-0.35f) * Mat4::scale({1.55f, 0.5f, 0.10f}),
            {0.12f, 0.095f, 0.055f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Wood);

        // Distant silhouette on the path. It barely moves.
        const float silhouetteX = 0.7f + std::sin(time * 0.18f) * 0.18f;
        drawCube(vao, shader,
            Mat4::translation({silhouetteX, 1.45f, -10.5f}) *
            Mat4::scale({0.42f, 2.7f, 0.34f}),
            {0.004f, 0.005f, 0.005f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Black);

        drawCube(vao, shader,
            Mat4::translation({silhouetteX, 3.25f, -10.5f}) *
            Mat4::scale({0.58f, 0.65f, 0.5f}),
            {0.004f, 0.005f, 0.005f}, vp, renderCamera, lights,
            flashPos, flashDir, flashlight, time, Material::Black);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
