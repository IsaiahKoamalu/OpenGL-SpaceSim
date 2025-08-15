#version 330 core
in vec3 vWorldPos;
in vec3 vWorldN;

uniform vec3  uAlbedoColor;
uniform vec3  uLightPosWS;      // Sun position in WORLD space
uniform vec3  uLightColor;
uniform float uLightIntensity;  // scale for your scene units

out vec4 FragColor;

void main()
{
    vec3 N = normalize(vWorldN);
    vec3 L = normalize(uLightPosWS + vWorldPos);  // Sun -> fragment

    float NdotL = max(dot(N, L), 0.0);

    // Simple inverse-square attenuation in WORLD space
    float dist2 = max(1e-4, dot(uLightPosWS - vWorldPos, uLightPosWS - vWorldPos));
    float atten = uLightIntensity / dist2;

    // Small ambient so the silhouette isn't pure black
    vec3 color = (uAlbedoColor * (0.15 + NdotL)) * uLightColor * atten;

    FragColor = vec4(color, 1.0);
}
