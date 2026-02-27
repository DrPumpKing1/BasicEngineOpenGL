#pragma once

#include <vector>

#include "Shader.h"
#include "ShaderUniforms.h"

class ShaderProgram
{
private:
    GLuint ID;
    ShaderUniforms uniforms;

    std::shared_ptr<Shader> vertexShader;
    std::shared_ptr<Shader> fragmentShader;
    std::shared_ptr<Shader> computeShader;
    std::shared_ptr<Shader> geometryShader;
public:
    ShaderProgram();
    ~ShaderProgram();

    void Bind() const;
    void Unbind() const;

    void AttachShader(std::shared_ptr<Shader> reference);
    void Compile();

    GLuint GetID() const {
        return ID;
    }

    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetVec2(const std::string &name, const glm::vec2 &value) const;
    void SetVec2(const std::string &name, float x, float y) const;
    void SetVec3(const std::string &name, const glm::vec3 &value) const;
    void SetVec3(const std::string &name, float x, float y, float z) const;
    void SetVec4(const std::string &name, const glm::vec4 &value) const;
    void SetVec4(const std::string &name, float x, float y, float z, float w) const;
    void SetMat2(const std::string &name, const glm::mat2 &mat) const;
    void SetMat3(const std::string &name, const glm::mat3 &mat) const;
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;
private:
    void CheckLinkErrors() const;
};