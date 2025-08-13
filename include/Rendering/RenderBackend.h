#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "Core/Registry.h"

struct Mesh
{
    GLuint vao=0, vbo=0, eb0=0;
    GLsizei indexCount = 0;
};

class RenderBackend
{
public:
    bool init(const std::string& vsSrc, const std::string& fsSrc)
    {
        prog = link(compile(GL_VERTEX_SHADER, vsSrc), compile(GL_FRAGMENT_SHADER, fsSrc));
        if (!prog) return false;
    }
private:
    GLuint prog = 0;
    GLint uModel=-1, uView=-1, uProj=-1;
    GLint uAlbedoColor=-1, uLightPosWS=-1, uLightColor=-1, uLightIntensity=-1;
    std::vector<Mesh> meshes;

    static GLuint compile(GLenum type, const std::string& src){}
    static GLuint link(GLuint vs, Gluint fs){}
};