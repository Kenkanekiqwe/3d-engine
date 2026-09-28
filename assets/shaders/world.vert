#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;
layout(location=3) in vec3 aTangent;
uniform mat4 uMVP;
uniform mat4 uModel;
out vec3 vWorld;
out vec3 vNormal;
out vec3 vTangent;
out vec2 vUV;
void main(){
 vWorld=vec3(uModel*vec4(aPos,1.0));
 vNormal=normalize(mat3(uModel)*aNormal);
 vTangent=normalize(mat3(uModel)*aTangent);
 vUV=aUV;
 gl_Position=uMVP*vec4(aPos,1.0);
}