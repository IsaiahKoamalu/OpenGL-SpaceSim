#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader
{
public:
    GLuint ID;

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const {glUseProgram(ID);}

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const float* value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

private:
    static std::string loadFromFile(const std::string& path);
    static GLuint compile(GLenum type, const std::string& src);
};

#endif
