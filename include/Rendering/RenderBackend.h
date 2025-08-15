#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <cstdio>
#include <Utils/ImageLoader.h>

struct Mesh {
    GLuint vao=0, vbo=0, ebo=0;
    GLsizei indexCount=0;
};

class RenderBackend
{
public:
    bool initSkyBoxProgram(const std::string& vsSrc, const std::string& fsSrc)
    {
        skyBoxProg = link(compile(GL_VERTEX_SHADER, vsSrc), compile(GL_FRAGMENT_SHADER, fsSrc));
        if (!skyBoxProg) return false;
        uSB_View = glGetUniformLocation(skyBoxProg, "uViewNoTrans");
        uSB_Proj = glGetUniformLocation(skyBoxProg, "uProj");
        uSB_Sampler = glGetUniformLocation(skyBoxProg, "uSky");
        return true;
    }

    bool init(const std::string& vsSrc, const std::string& fsSrc) {
        GLuint vs = compile(GL_VERTEX_SHADER,   vsSrc);
        GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);
        prog = link(vs, fs);
        if (!prog) return false;

        uModel          = glGetUniformLocation(prog, "uModel");
        uView           = glGetUniformLocation(prog, "uView");
        uProj           = glGetUniformLocation(prog, "uProj");
        uAlbedoColor    = glGetUniformLocation(prog, "uAlbedoColor");
        uLightPosWS     = glGetUniformLocation(prog, "uLightPosWS");   // POINT LIGHT (world)
        uLightColor     = glGetUniformLocation(prog, "uLightColor");
        uLightIntensity = glGetUniformLocation(prog, "uLightIntensity");
        uCamPos         = glGetUniformLocation(prog, "uCamPos");       // camera (world)

        auto req = [](GLint loc, const char* n){
            if (loc == -1) std::fprintf(stderr, "Missing uniform: %s\n", n);
        };
        req(uModel,"uModel"); req(uView,"uView"); req(uProj,"uProj");
        req(uAlbedoColor,"uAlbedoColor"); req(uLightPosWS,"uLightPosWS");
        req(uLightColor,"uLightColor"); req(uLightIntensity,"uLightIntensity"); req(uCamPos,"uCamPos");
        return true;
    }

    void createSkyBoxCube()
    {
        const float verts[] =
        {
            -1,-1,-1,  -1, 1,-1,   1, 1,-1,   1, 1,-1,   1,-1,-1,  -1,-1,-1,
            -1,-1, 1,   1,-1, 1,   1, 1, 1,   1, 1, 1,  -1, 1, 1,  -1,-1, 1,
            -1, 1, 1,   1, 1, 1,   1, 1,-1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,
            -1,-1, 1,  -1,-1,-1,   1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,
             1,-1, 1,   1,-1,-1,   1, 1,-1,   1, 1,-1,   1, 1, 1,   1,-1, 1,
            -1,-1, 1,  -1, 1, 1,  -1, 1,-1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1
        };
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    int createCubeMap(const std::array<unsigned char*, 6>& images,
                      const std::array<int,6>& widths,
                      const std::array<int,6>& heights,
                      bool srgb = false)
    {
        GLuint tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

        GLenum internal = srgb ? GL_SRGB8_ALPHA8 : GL_RGB8;
        for (int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal,
                          widths[i], heights[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return (int)tex;
    }

    int createCubeMapFromFile(const std::array<std::string,6>& files, bool srgb = true)
    {
        std::array<LoadedImage, 6> imgs;
        for (int i=0; i<6; ++i)
        {
            imgs[i] = loadImageRGBA(files[i]);
            if (!imgs[i].ok())
            {
                std::fprintf(stderr, "[cubemap] Failed Face %d: %s\n", i, files[i].c_str());
                return -1;
            }
        }
        std::array<unsigned char*, 6> ptrs{};
        std::array<int, 6> widths{}, heights{};
        for (int i = 0; i < 6; ++i)
        {
            ptrs[i] = imgs[i].pixels.data();
            widths[i] = imgs[i].w;
            heights[i] = imgs[i].h;
        }

        return createCubeMap(ptrs, widths, heights, srgb);
    };

    void drawSkyBox(const glm::mat4& view, const glm::mat4& proj, int cubemapTex)
    {
        if (!skyBoxProg || cubemapTex <= 0) return;

        auto viewNoTrans = glm::mat4(glm::mat3(view));

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glDisable(GL_CULL_FACE);

        glUseProgram(skyBoxProg);
        glUniformMatrix4fv(uSB_View, 1, GL_FALSE, glm::value_ptr(viewNoTrans));
        glUniformMatrix4fv(uSB_Proj, 1, GL_FALSE, glm::value_ptr(proj));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, (GLuint)cubemapTex);
        glUniform1i(uSB_Sampler, 0);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Unbind texture + restore depth state
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    int createUnitSphere(int stacks=32, int slices=32){
        Mesh m{};
        std::vector<float> verts;    // pos(3) + normal(3)
        std::vector<unsigned int> idx;
        verts.reserve(size_t(stacks+1)*(slices+1)*6);
        idx.reserve(size_t(stacks)*slices*6);

        for (int i=0; i<=stacks; ++i){
            float v = float(i)/stacks;
            float phi = v * 3.14159265f; // 0..pi
            for (int j=0; j<=slices; ++j){
                float u = float(j)/slices;
                float theta = u * 2.0f * 3.14159265f; // 0..2pi
                float x = sinf(phi)*cosf(theta);
                float y = cosf(phi);
                float z = sinf(phi)*sinf(theta);
                verts.push_back(x); verts.push_back(y); verts.push_back(z); // pos
                verts.push_back(x); verts.push_back(y); verts.push_back(z); // normal
            }
        }

        auto ix = [&](int i, int j){ return i * (slices + 1) + j; };
        for (int i=0; i<stacks; ++i){
            for (int j=0; j<slices; ++j){
                unsigned a = ix(i,   j);
                unsigned b = ix(i+1, j);
                unsigned c = ix(i+1, j+1);
                unsigned d = ix(i,   j+1);
                idx.push_back(a); idx.push_back(b); idx.push_back(c);
                idx.push_back(a); idx.push_back(c); idx.push_back(d);
            }
        }

        glGenVertexArrays(1, &m.vao);
        glGenBuffers(1, &m.vbo);
        glGenBuffers(1, &m.ebo);

        glBindVertexArray(m.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned), idx.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // aPos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); // aNormal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

        glBindVertexArray(0);

        m.indexCount = (GLsizei)idx.size();
        meshes.push_back(m);
        return (int)meshes.size()-1;
    }

    GLsizei getMeshIndexCount(int meshId) const {
        if (meshId < 0 || meshId >= (int)meshes.size()) return 0;
        return meshes[meshId].indexCount;
    }

    struct DrawParams
    {
        glm::mat4 M, V, P;
        glm::vec3 albedo;
        glm::vec3 lightPosWS;
        glm::vec3 lightColor;
        float lightIntensity;
        glm::vec3 camPos;
    };

    void drawMesh(int meshId, const DrawParams& p)
    {
        glUseProgram(prog);

        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(p.M));
        glUniformMatrix4fv(uView,  1, GL_FALSE, glm::value_ptr(p.V));
        glUniformMatrix4fv(uProj,  1, GL_FALSE, glm::value_ptr(p.P));

        glUniform3fv(uAlbedoColor, 1, glm::value_ptr(p.albedo));

        // WORLD-space light & camera
        glUniform3fv(uLightPosWS,  1, glm::value_ptr(p.lightPosWS));
        glUniform3fv(uLightColor,  1, glm::value_ptr(p.lightColor));
        glUniform1f (uLightIntensity, p.lightIntensity);

        if (uCamPos != -1) { // only if used by the shader
            glUniform3fv(uCamPos, 1, glm::value_ptr(p.camPos));
        }

        const Mesh& m = meshes[meshId];
        glBindVertexArray(m.vao);
        glDrawElements(GL_TRIANGLES, m.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // Skybox
    GLuint skyBoxProg = 0;
    GLint uSB_View=-1, uSB_Proj=-1, uSB_Sampler=-1;
    GLuint skyboxVAO = 0, skyboxVBO = 0;

    //Planet/Sun
    GLuint prog=0;
    GLint uModel=-1, uView=-1, uProj=-1;
    GLint uAlbedoColor=-1, uLightPosWS=-1, uLightColor=-1, uLightIntensity=-1, uCamPos=-1;
    std::vector<Mesh> meshes;

    static GLuint compile(GLenum type, const std::string& src){
        GLuint s = glCreateShader(type);
        const char* c = src.c_str();
        glShaderSource(s, 1, &c, nullptr);
        glCompileShader(s);
        GLint ok=GL_FALSE; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok){
            char log[2048]; glGetShaderInfoLog(s, sizeof log, nullptr, log);
            std::fprintf(stderr, "Shader error:\n%s\n", log);
        }
        return s;
    }
    static GLuint link(GLuint vs, GLuint fs){
        GLuint p = glCreateProgram();
        glAttachShader(p, vs); glAttachShader(p, fs);
        glLinkProgram(p);
        GLint ok=GL_FALSE; glGetProgramiv(p, GL_LINK_STATUS, &ok);
        if (!ok){
            char log[2048]; glGetProgramInfoLog(p, sizeof log, nullptr, log);
            std::fprintf(stderr, "Link error:\n%s\n", log);
        }
        glDeleteShader(vs); glDeleteShader(fs);
        return p;
    }
};
