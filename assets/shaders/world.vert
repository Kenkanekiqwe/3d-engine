#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUV;
uniform mat4 uMVP;
uniform mat4 uModel;
out vec3 vWorld;
out vec2 vUV;
void main(){vWorld=vec3(uModel*vec4(aPos,1.0));vUV=aUV;gl_Position=uMVP*vec4(aPos,1.0);}
