#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec3 uEmissiveColor = vec3(1.0, 0.95, 0.8);
uniform float uEmissiveIntensity = 5.0;

uniform sampler2D uEmissiveTex;
uniform bool uUseEmissiveTex = false;

void main()
{
    vec3 emissive = uUseEmissiveTex
        ? texture(uEmissiveTex, vUV).rgb * uEmissiveIntensity
        : uEmissiveColor * uEmissiveIntensity;

    FragColor = vec4(emissive, 1.0);
}