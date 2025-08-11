#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT
{
    vec3 worldPos;
    vec3 worldN;
    vec3 worldT;
    vec3 worldB;
    vec2 uv;
} vs;

void main()
{
    // World Pos
    vec4 wp = model * vec4(aPos, 1.0);
    vs.worldPos = wp.xyz;

    // Normal Matrix
    mat3 N = mat3(transpose(inverse(model)));
    vec3 n = normalize(N * aNormal);
    vs.worldN = n;

    // Stable tangent basis from norm
    vec3 up = (abs(n.y) > 0.00) ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 t = normalize(cross(up, n));
    vec3 b = cross(n, t);
    vs.worldT = t;
    vs.worldB = b;

    vs.uv = aUV;

    gl_Position = projection * view * wp;
}