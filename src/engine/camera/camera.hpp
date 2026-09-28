#pragma once
#include "../math/math.hpp"
namespace cry {
class Camera {
    Vec3 position_{};
    float yaw_{-1.5707963f}, pitch_{-0.06f};
    float sensitivity_{0.0019f};
    bool invertX_{true}, invertY_{false};
public:
    explicit Camera(Vec3 position):position_(position){}
    void look(Vec3 d){ yaw_+=d.x*sensitivity_*(invertX_?-1.f:1.f); pitch_+=d.y*sensitivity_*(invertY_?1.f:-1.f); pitch_=std::clamp(pitch_,-1.30f,1.30f); }
    Vec3 forward()const{return{std::cos(pitch_)*std::cos(yaw_),std::sin(pitch_),std::cos(pitch_)*std::sin(yaw_)};}
    Vec3 flatForward()const{return{std::cos(yaw_),0,std::sin(yaw_)};}
    Vec3 right()const{return{-std::sin(yaw_),0,std::cos(yaw_)};}
    Vec3 position()const{return position_;}
    void setPosition(Vec3 p){position_=p;}
    Mat4 view()const{return Mat4::lookAt(position_,position_+forward(),{0,1,0});}
};
}