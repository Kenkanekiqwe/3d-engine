#version 330 core
in vec3 vWorld;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uCamera;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uFlashPos;
uniform vec3 uFlashDir;
uniform float uFlashOn;
uniform float uTime;
uniform int uMaterial;

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main() {
    vec3 n = normalize(cross(dFdx(vWorld), dFdy(vWorld)));

    vec3 toLight = uLightPos - vWorld;
    float lightDist = length(toLight);
    vec3 l = normalize(toLight);

    float diffuse = max(dot(n, l), 0.0);
    float attenuation = 1.0 / (1.0 + 0.11 * lightDist + 0.035 * lightDist * lightDist);

    // Warm ceiling light.
    vec3 roomLight = uLightColor * diffuse * attenuation * 1.65;

    // Flashlight with a soft central hot spot and distance falloff.
    vec3 toPixel = vWorld - uFlashPos;
    float flashDist = length(toPixel);
    vec3 flashToPixel = normalize(toPixel);

    float cone = max(dot(normalize(uFlashDir), flashToPixel), 0.0);
    float spot = smoothstep(0.78, 0.985, cone);
    float flashAttenuation = 1.0 / (1.0 + 0.06 * flashDist + 0.018 * flashDist * flashDist);
    float flashlight = uFlashOn * spot * flashAttenuation * 5.0;
    float flashDiffuse = max(dot(n, -flashToPixel), 0.0);

    vec3 lighting = uColor * (
        0.018 +
        roomLight +
        flashlight * flashDiffuse
    );

    // Cheap procedural surface variation keeps primitive geometry from looking flat.
    float grain = noise(vWorld.xz * 3.4) * 0.22 + noise(vWorld.xz * 13.0) * 0.07;

    if (uMaterial == 0) { // concrete
        lighting *= 0.78 + grain;
        float stains = smoothstep(0.64, 0.92, noise(vWorld.xz * 1.7));
        lighting *= 1.0 - stains * 0.28;
    } else if (uMaterial == 1) { // metal
        lighting *= 0.72 + grain * 0.65;
    } else if (uMaterial == 2) { // wood
        float grainWood = 0.78 + 0.22 * sin(vWorld.y * 11.0 + noise(vWorld.xz * 2.0) * 4.0);
        lighting *= grainWood;
    } else if (uMaterial == 3) { // dirty tile
        float tile = step(0.045, fract(vWorld.x * 0.72)) * step(0.045, fract(vWorld.z * 0.72));
        lighting *= 0.72 + 0.28 * tile;
        lighting *= 0.82 + 0.18 * grain;
    } else if (uMaterial == 4) { // lamp
        float flicker = 0.88 + 0.12 * sin(uTime * 11.0) + 0.035 * sin(uTime * 37.0);
        lighting = uColor * (0.35 + flicker * 2.2);
    } else if (uMaterial == 5) { // almost black
        lighting *= 0.25;
    } else if (uMaterial == 6) { // rust
        float rust = noise(vWorld.yz * 5.0);
        lighting *= 0.55 + rust * 0.55;
    }

    // Exponential fog: preserve nearby detail, dissolve the far end into atmosphere.
    float distanceToCamera = length(vWorld - uCamera);
    float fog = exp(-0.018 * distanceToCamera * distanceToCamera);
    vec3 fogColor = vec3(0.012, 0.014, 0.018);
    vec3 color = mix(fogColor, lighting, clamp(fog, 0.0, 1.0));

    // Subtle darkening toward the edges reinforces the flashlight-driven composition.
    float edge = length(gl_FragCoord.xy / vec2(1280.0, 720.0) - vec2(0.5));
    float vignette = 1.0 - smoothstep(0.28, 0.76, edge) * 0.28;
    color *= vignette;

    // Mild film grain, strongest in darkness.
    float film = hash(gl_FragCoord.xy + uTime) - 0.5;
    color += film * 0.012 * (1.0 - clamp(length(color) * 3.0, 0.0, 1.0));

    FragColor = vec4(color, 1.0);
}
