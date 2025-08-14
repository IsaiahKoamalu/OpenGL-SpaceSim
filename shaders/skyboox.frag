#version 330 core
layout(location=0) in vec3 aPos;
out vec3 vDir;
uniform vec4 FragColor;

uniform samplerCube uSky;
void main()
{
    FragColor = texture(uSky, normalize(vDir));
}