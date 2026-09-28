#include "input.hpp"
namespace cry {
Input::Input(GLFWwindow* window):window_(window){ glfwGetCursorPos(window_,&lastX_,&lastY_); }
void Input::beginFrame(){
    double x{},y{}; glfwGetCursorPos(window_,&x,&y);
    mouse_={float(x-lastX_),float(y-lastY_),0}; lastX_=x; lastY_=y;
}
bool Input::down(int key) const { return glfwGetKey(window_,key)==GLFW_PRESS; }
Vec3 Input::mouseDelta(){ Vec3 d=mouse_; mouse_={}; return d; }
}