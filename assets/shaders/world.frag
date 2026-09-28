#version 330 core
in vec3 vWorld;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uCamera;
uniform vec3 uLightPos;
uniform vec3 uFlashDir;
uniform float uFlashOn;
uniform float uTime;

void main() {
    vec3 n = normalize(cross(dFdx(vWorld), dFdy(vWorld)));
    vec3 toLight = uLightPos - vWorld;
    float dist = length(toLight);
    vec3 l = normalize(toLight);

    float diffuse = max(dot(n,l),0.0);
    float attenuation = 1.0 / (1.0 + 0.09*dist + 0.032*dist*dist);

    vec3 toPixel = normalize(vWorld-uCamera);
    float cone = max(dot(normalize(uFlashDir),toPixel),0.0);
    float flashlight = uFlashOn * smoothstep(0.72,0.94,cone) * 1.8;

    float flicker = 0.88 + 0.12*sin(uTime*17.0) + 0.04*sin(uTime*41.0);
    vec3 lit = uColor * (0.025 + diffuse*attenuation*flicker + flashlight);

    float fog = exp(-0.018*length(vWorld-uCamera));
    vec3 fogColor = vec3(0.008,0.009,0.012);
    FragColor = vec4(mix(fogColor,lit,clamp(fog,0.0,1.0)),1.0);
}