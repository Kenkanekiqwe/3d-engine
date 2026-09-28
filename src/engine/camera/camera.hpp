#pragma once
#include <algorithm>
#include <cmath>
#include "../math/math.hpp"

namespace cry {

class Camera {
    Vec3 position_{};
    float yaw_{-1.5707963f};
    float pitch_{-0.06f};

    // Standard FPS mouse controls:
    // mouse right  -> look right
    // mouse left   -> look left
    // mouse up     -> look up
    // mouse down   -> look down
    float sensitivity_{0.0019f};

public:
    explicit Camera(Vec3 position) : position_(position) {}

    void look(Vec3 delta) {
        yaw_ += delta.x * sensitivity_;
        pitch_ -= delta.y * sensitivity_;
        pitch_ = std::clamp(pitch_, -1.30f, 1.30f);
    }

    Vec3 forward() const {
        const float cp = std::cos(pitch_);
        return {
            cp * std::cos(yaw_),
            std::sin(pitch_),
            cp * std::sin(yaw_)
        };
    }

    Vec3 flatForward() const {
        return {std::cos(yaw_), 0.0f, std::sin(yaw_)};
    }

    Vec3 right() const {
        return {-std::sin(yaw_), 0.0f, std::cos(yaw_)};
    }

    Vec3 position() const { return position_; }
    void setPosition(Vec3 p) { position_ = p; }

    Mat4 view() const {
        return Mat4::lookAt(
            position_,
            position_ + forward(),
            {0.0f, 1.0f, 0.0f}
        );
    }
};

} // namespace cry
