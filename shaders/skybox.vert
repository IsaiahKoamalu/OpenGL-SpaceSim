#version 330 core
layout(location=0) in vec3 aPos;

uniform mat4 uViewNoTrans;  // view with translation removed
uniform mat4 uProj;

out vec3 vDir;

void main() {
    vDir = aPos;  // direction into cube map
    gl_Position = uProj * uViewNoTrans * vec4(aPos * 50.0, 1.0);
}