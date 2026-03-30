#pragma once

#include "../../AssetProcessor/AssetProcessor.h"
#include "Asset.h"

const std::string USAGE_REGISTRY_RELATIVE_PATH = "resources/usage_registry.txt";

class ResourceManager
{
private:
    std::unordered_map<std::filesystem::path, std::filesystem::path> pathsTable;
    std::vector<ShaderAsset> shaders;
    std::vector<TextureAsset> textures;
    std::vector<CubemapAsset> cubemaps;
    std::vector<ModelAsset> models;
    std::vector<std::string> relativePaths;
public:
    static ResourceManager& Instance() {
        static ResourceManager instance;
        return instance;
    }

    void Release();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::filesystem::path GetAssetPath(const std::filesystem::path &relativePath);

    void NewShaderAsset(const std::string &relativePath, ShaderType type, std::shared_ptr<Shader> &shareTo);
    void NewTextureAsset(const std::string &relativePath, TextureType type, GLenum unit, bool transparency, std::shared_ptr<Texture> &shareTo);
    void NewCubemapAsset(const std::string &relativeFolderPath, std::vector<std::string> facesFilenames, GLenum unit, std::shared_ptr<Cubemap> &shareTo);
    void NewModelAsset(const std::string &relativePath, std::shared_ptr<Model> &shareTo);
private:
    ResourceManager();
};