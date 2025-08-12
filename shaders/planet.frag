#version 330 core
in vec2 vUV;
in vec3 vWorldPos;
in vec3 vWorldN;
out vec4 FragColor;

struct PointLight { vec3 position; vec3 color; float intensity; float radius; };
uniform PointLight uPL;

uniform vec3 uCameraWorld;      // world-space camera position
uniform vec3 uAlbedoColor;
uniform bool uHasAlbedo;
uniform sampler2D uAlbedo;

uniform float uShininess = 64.0;
//uniform float uSpecStrength = 0.2;
const float uSpecStrength = 0.0;


void main() {
    vec3 base = uHasAlbedo ? texture(uAlbedo, vUV).rgb : uAlbedoColor;

    vec3 N    = normalize(vWorldN);                       // world
    vec3 Lvec = uPL.position - vWorldPos;                 // world
    float dist= length(Lvec);
    vec3 L    = Lvec / max(dist, 1e-6);

    float NdotL = max(dot(N, L), 0.0);

    vec3 V = normalize(uCameraWorld - vWorldPos);         // world
    vec3 H = normalize(L + V);
    float spec = (NdotL > 0.0) ? pow(max(dot(N, H), 0.0), uShininess) : 0.0;

    float falloff = 1.0 / max(dist * dist, 1.0);
    float smoothe  = clamp(1.0 - dist / uPL.radius, 0.0, 1.0);
    float atten   = falloff * smoothe;

    vec3 lightCol = uPL.color * uPL.intensity * atten;

    vec3 ambient  = 0.01 * base;
    vec3 diffuse  = base * NdotL;
    vec3 specular = uSpecStrength * spec * vec3(1.0);

    FragColor = vec4(ambient + (diffuse + specular) * lightCol, 1.0);
}