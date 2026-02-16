#include "ShaderProgram.h"

void ShaderProgram::SetBool(const std::string &name, bool value) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform1i(location, (int)value);
}

void ShaderProgram::SetInt(const std::string &name, int value) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform1i(location, value);
}

void ShaderProgram::SetFloat(const std::string &name, float value) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform1f(location, value);
}

void ShaderProgram::SetVec2(const std::string &name, const glm::vec2 &value) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform2fv(location, 1, &value[0]);
}

void ShaderProgram::SetVec2(const std::string &name, float x, float y) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform2f(location, x, y);
}

void ShaderProgram::SetVec3(const std::string &name, const glm::vec3 &value) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform3fv(location, 1, &value[0]);
}

void ShaderProgram::SetVec3(const std::string &name, float x, float y, float z) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform3f(location, x, y, z);
}

void ShaderProgram::SetVec4(const std::string &name, const glm::vec4 &value) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform4fv(location, 1, &value[0]);
}

void ShaderProgram::SetVec4(const std::string &name, float x, float y, float z, float w) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniform4f(location, x, y, z, w);
}

void ShaderProgram::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
    GLint location = uniforms.GetUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}