#version 330 core
in vec3 vWorld;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uCamera;

uniform vec3 uLight0Pos;
uniform vec3 uLight0Color;
uniform float uLight0Radius;
uniform float uLight0Intensity;

uniform vec3 uLight1Pos;
uniform vec3 uLight1Color;
uniform float uLight1Radius;
uniform float uLight1Intensity;

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

vec3 pointLight(
    vec3 position,
    vec3 lightColor,
    float radius,
    float intensity,
    vec3 normal
) {
    vec3 toLight = position - vWorld;
    float dist = length(toLight);
    vec3 direction = normalize(toLight);
    float diffuse = max(dot(normal, direction), 0.0);
    float falloff = clamp(1.0 - dist / radius, 0.0, 1.0);
    falloff *= falloff;
    return lightColor * diffuse * falloff * intensity;
}

void main() {
    vec3 normal = normalize(cross(dFdx(vWorld), dFdy(vWorld)));

    // Cool moonlight and warm cabin glow create the visual temperature contrast.
    vec3 lighting = vec3(0.012, 0.016, 0.019);
    lighting += uColor * pointLight(
        uLight0Pos, uLight0Color, uLight0Radius, uLight0Intensity, normal
    );
    lighting += uColor * pointLight(
        uLight1Pos, uLight1Color, uLight1Radius, uLight1Intensity, normal
    );

    // Flashlight: narrow cone, soft edge, strong center, distance attenuation.
    vec3 toPixel = vWorld - uFlashPos;
    float flashDist = length(toPixel);
    vec3 flashVector = normalize(toPixel);
    float cone = max(dot(normalize(uFlashDir), flashVector), 0.0);

    float outer = smoothstep(0.69, 0.93, cone);
    float inner = smoothstep(0.90, 0.995, cone);
    float coneShape = mix(outer * 0.55, inner, 0.72);

    float flashFalloff = 1.0 / (1.0 + 0.08 * flashDist + 0.024 * flashDist * flashDist);
    float flashDiffuse = max(dot(normal, -flashVector), 0.0);

    lighting += uColor * coneShape * flashFalloff * flashDiffuse * 6.2 * uFlashOn;

    float grain = noise(vWorld.xz * 3.0) * 0.20 + noise(vWorld.xz * 15.0) * 0.08;

    if (uMaterial == 0) { // ground
        lighting *= 0.62 + grain;
        float mud = noise(vWorld.xz * 0.85);
        lighting *= 0.78 + mud * 0.28;
    } else if (uMaterial == 1) { // bark
        float bark = 0.72 + 0.28 * noise(vWorld.xy * 7.0);
        lighting *= bark;
        lighting *= 0.85 + 0.15 * sin(vWorld.y * 13.0);
    } else if (uMaterial == 2) { // moss / foliage
        float leaf = 0.72 + 0.28 * noise(vWorld.xz * 5.0);
        lighting *= leaf;
    } else if (uMaterial == 3) { // rock
        lighting *= 0.65 + grain * 0.55;
    } else if (uMaterial == 4) { // wood
        float wood = 0.78 + 0.22 * sin(vWorld.y * 10.0 + noise(vWorld.xz * 2.0) * 3.0);
        lighting *= wood;
    } else if (uMaterial == 5) { // metal
        lighting *= 0.60 + grain * 0.45;
    } else if (uMaterial == 6) { // warm window
        float pulse = 0.93 + 0.05 * sin(uTime * 2.0);
        lighting = uColor * pulse * 2.3;
    } else if (uMaterial == 7) { // cold light
        lighting = uColor * 1.8;
    } else if (uMaterial == 8) { // silhouette
        lighting *= 0.16;
    } else if (uMaterial == 9) { // water
        lighting *= 0.38 + 0.25 * noise(vWorld.xz * 5.0);
    }

    // Height-aware fog: thicker close to the forest floor and in the distance.
    float distanceToCamera = length(vWorld - uCamera);
    float heightFog = 1.0 - smoothstep(0.5, 5.0, vWorld.y);
    float distanceFog = exp(-0.011 * distanceToCamera * distanceToCamera);
    float fog = clamp(distanceFog * (1.0 - heightFog * 0.22), 0.0, 1.0);

    // Cold blue-gray atmospheric color.
    vec3 fogColor = vec3(0.012, 0.021, 0.027);
    vec3 color = mix(fogColor, lighting, fog);

    // Cinematic vignette.
    vec2 uv = gl_FragCoord.xy / vec2(1280.0, 720.0);
    float edge = length(uv - vec2(0.5));
    color *= 1.0 - smoothstep(0.25, 0.78, edge) * 0.34;

    // Subtle low-light film grain.
    float film = hash(gl_FragCoord.xy + uTime * 3.0) - 0.5;
    color += film * 0.010;

    FragColor = vec4(max(color, vec3(0.0)), 1.0);
}
