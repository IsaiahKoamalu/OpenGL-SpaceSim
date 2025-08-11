#version 330 core
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vUV;

out vec4 FragColor;

uniform vec3 uSunDir = normalize(vec3(1.0, -0.4, -0.3));
uniform vec3 uAlbedo = vec3(0.0, 0.5, 0.9); //PH color

void main()
{
    vec3 N = normalize(vNormal);
    float NdotL = max(dot(N, normalize(-uSunDir)), 0.0);
    vec3 diffuse = uAlbedo * NdotL;

    vec3 ambient = 0.05 * uAlbedo;
    FragColor = vec4(ambient + diffuse, 1.0);
}