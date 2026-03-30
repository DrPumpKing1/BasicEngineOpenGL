#include "Asset.h"

ShaderAsset::ShaderAsset(const std::string &path, ShaderType type) : path(path) 
{
    reference = std::make_shared<Shader>(path, type);
}

TextureAsset::TextureAsset(const std::string &path, TextureType type, GLenum unit, bool transparency) : path(path) 
{
    reference = std::make_shared<Texture>(path.c_str(), type, unit, transparency);
}

ModelAsset::ModelAsset(const std::string &path) : path(path) 
{
    reference = std::make_shared<Model>(path);
}

CubemapAsset::CubemapAsset(std::vector<std::filesystem::path> texturesPaths, GLenum unit) 
{
    reference = std::make_shared<Cubemap>(texturesPaths, unit);
}
