#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;

uniform mat4 model, view, projection;

out vec2 vUV;
out vec3 vWorldPos;
out vec3 vWorldN;

void main() {
    vec4 wp = model * vec4(aPos, 1.0);
    vWorldPos = wp.xyz;

    // IMPORTANT: build normal from MODEL only (not view*model)
    mat3 Nmat = mat3(transpose(inverse(model)));
    vWorldN   = normalize(Nmat * aNormal);

    vUV = aUV;
    gl_Position = projection * view * wp;
}