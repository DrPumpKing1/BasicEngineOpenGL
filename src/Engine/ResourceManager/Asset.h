#pragma once

#include "../Shader/Shader.h"
#include "../Texture/Texture.h"

#include <boost/uuid/uuid.hpp>
#include <memory>
#include <filesystem>

class ShaderAsset 
{
private:
    std::shared_ptr<Shader> reference;
    std::filesystem::path path;
public:

    ShaderAsset(const std::string &path, ShaderType type);
    ~ShaderAsset() {
        reference.reset();
    }

    void Share(std::shared_ptr<Shader> &shareTo) {
        shareTo = reference;
    }
};

class TextureAsset
{
private:
    std::shared_ptr<Texture> reference;
    std::filesystem::path path;
public:

    TextureAsset(const std::string &path, TextureType type, GLenum unit, bool transparency = false);
    ~TextureAsset() {
        reference.reset();
    }

    void Share(std::shared_ptr<Texture> &shareTo) {
        shareTo = reference;
    }
};