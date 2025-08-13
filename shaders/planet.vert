#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

uniform mat4 uModel;   // model -> world
uniform mat4 uView;    // world -> view (for gl_Position only)
uniform mat4 uProj;    // view  -> clip

out vec3 vWorldPos;    // WORLD space
out vec3 vWorldN;      // WORLD space

void main(){
    vec4 wp = uModel * vec4(aPos, 1.0);
    vWorldPos = wp.xyz;

    mat3 Nmat = mat3(transpose(inverse(uModel))); // MODEL only
    vWorldN   = normalize(Nmat * aNormal);

    gl_Position = uProj * uView * wp;             // only for rasterization
}
