#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

struct Mesh
{
    GLuint VAO=0, VBO=0, EBO=0;
    GLsizei indexCount = 0;

    Mesh() = default;
    Mesh(const float* verts, size_t vertBytes, const unsigned int* inds, size_t indBytes);
    static Mesh fromCube(float size=1.0f);
    static Mesh fromSphere(float radius=1.0f, int stacks=32, int slices=64);
    void draw() const;
    void destroy();
};

#endif
