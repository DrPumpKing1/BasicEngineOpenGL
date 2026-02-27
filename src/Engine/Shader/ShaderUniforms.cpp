#include "ShaderUniforms.h"

ShaderUniforms::ShaderUniforms(GLuint programID) : programID(programID)
{
    uniformLocations.reserve(UNIFORM_CACHE_INITIAL_SIZE);
}

ShaderUniforms::~ShaderUniforms()
{
    uniformLocations.clear();
}

GLint ShaderUniforms::GetUniformLocation(const std::string &name) const
{
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end())
        return it->second;

    GLint location = glGetUniformLocation(programID, name.c_str());
    uniformLocations[name] = location;

    return location;
}