#pragma once
#include <GLFW/glfw3.h>
#include "../math/math.hpp"

namespace cry {
class Input {
    GLFWwindow* window_{};
    double lastX_{}, lastY_{};
    Vec3 mouse_{};
public:
    explicit Input(GLFWwindow* window);
    void beginFrame();
    bool down(int key) const;
    Vec3 mouseDelta();
};
}