#include "Rendering/Mesh.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

static constexpr int STRIDE = 8; // remember: pos(3), normal(3), uv(2)

Mesh::Mesh(const float* verts, size_t vertBytes, const unsigned int* inds, size_t indBytes)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertBytes, verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBytes, inds, GL_STATIC_DRAW);
    GLsizei stride = STRIDE * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,stride,(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,stride,(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Mesh::draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::destroy()
{
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1,&VAO);
    VAO=VBO=EBO=0;
    indexCount = 0;
}

Mesh Mesh::fromCube(float size)
{
    float s = size*0.5f;
    const float verts[] = {
        // pos             // normal      // uv
        -s,-s,-s,          0,0,-1,        0,0,
         s,-s,-s,          0,0,-1,        1,0,
         s, s,-s,          0,0,-1,        1,1,
        -s, s,-s,          0,0,-1,        0,1,
        // ... other faces ...
    };
    const unsigned int idx[] = {
        0,1,2, 2,3,0
        // ... other faces ...
    };
    Mesh m(verts, sizeof(verts), idx, sizeof(idx));
    m.indexCount = (GLsizei)(sizeof(idx)/sizeof(unsigned int));
    return m;
}

// UV-sphere
Mesh Mesh::fromSphere(float radius, int stacks, int slices) {
    std::vector<float> v;
    std::vector<unsigned int> i;
    v.reserve((stacks+1)*(slices+1)*STRIDE);
    i.reserve(stacks*slices*6);

    for (int a=0; a<=stacks; ++a) {
        float vCoord = float(a)/stacks;
        float phi = vCoord * glm::pi<float>();
        float y = std::cos(phi);
        float r = std::sin(phi);
        for (int b=0; b<=slices; ++b) {
            float uCoord = float(b)/slices;
            float theta = uCoord * glm::two_pi<float>();
            float x = r * std::cos(theta);
            float z = r * std::sin(theta);
            glm::vec3 n = glm::normalize(glm::vec3(x,y,z));
            glm::vec3 p = radius * n;
            v.insert(v.end(), { p.x,p.y,p.z, n.x,n.y,n.z, uCoord, 1.0f - vCoord });
        }
    }
    int stride = slices+1;
    for (int a=0; a<stacks; ++a) {
        for (int b=0; b<slices; ++b) {
            int A=a*stride+b, B=(a+1)*stride+b, C=(a+1)*stride+(b+1), D=a*stride+(b+1);
            i.insert(i.end(), { (unsigned)A,(unsigned)B,(unsigned)C, (unsigned)A,(unsigned)C,(unsigned)D });
        }
    }
    Mesh m(v.data(), v.size()*sizeof(float), i.data(), i.size()*sizeof(unsigned int));
    m.indexCount = (GLsizei)i.size();
    return m;
}
