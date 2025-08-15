#version 330 core
in vec3 vDir;
out vec4 FragColor;
uniform samplerCube uSky;
void main() {
    FragColor = texture(uSky, normalize(vDir));
}