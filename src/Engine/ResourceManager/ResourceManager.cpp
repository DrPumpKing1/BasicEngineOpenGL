#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
    std::ifstream registryFile(std::filesystem::current_path() / REGISTRY_FILENAME);
    if (registryFile.is_open())
    {
        nlohmann::json registryJson = nlohmann::json::array();
        registryFile >> registryJson;
        for(auto& element : registryJson.items())
        {
            AssetTag tag = element.value().get<AssetTag>();
            std::filesystem::path relativePath = std::filesystem::path(tag.path);
            pathsTable[relativePath] = std::filesystem::current_path() / "resources" / tag.path;
        }
    }
    else
    {
        std::cerr << "Failed to open asset registry file: " << (std::filesystem::current_path() / REGISTRY_FILENAME) << std::endl;
    }
}

std::filesystem::path ResourceManager::GetAssetPath(const std::filesystem::path &relativePath)
{
    auto it = pathsTable.find(relativePath);
    if (it != pathsTable.end())
    {
        return it->second;
    }
    else
    {
        std::cerr << "Asset not found: " << relativePath << std::endl;
        return std::filesystem::path("");
    }
}

void ResourceManager::NewShaderAsset(const std::string &relativePath, ShaderType type, std::shared_ptr<Shader>& shareTo) {
    std::filesystem::path path = GetAssetPath(relativePath);
    if(path.empty()) {
        std::cerr << "Asset not found at path " << relativePath << std::endl;
        return;
    }
    ShaderAsset asset(path.string(), type);
    shaders.push_back(asset);
    asset.Share(shareTo);
    if(std::find(relativePaths.begin(), relativePaths.end(), relativePath) == relativePaths.end()) {
        relativePaths.push_back(relativePath);
    }
}

void ResourceManager::NewTextureAsset(const std::string &relativePath, TextureType type, GLenum unit, bool transparency, std::shared_ptr<Texture>& shareTo) {
    std::filesystem::path path = GetAssetPath(relativePath);
    if(path.empty()) {
        std::cerr << "Asset not found at path " << relativePath << std::endl;
        return;
    }
    TextureAsset asset(path.string(), type, unit, transparency);
    textures.push_back(asset);
    asset.Share(shareTo);
    if(std::find(relativePaths.begin(), relativePaths.end(), relativePath) == relativePaths.end()) {
        relativePaths.push_back(relativePath);
    }
}

void ResourceManager::Release() {
#ifdef DEBUG
    std::filesystem::path usageRegistryFullPath = std::filesystem::path(project_source_directory) / USAGE_REGISTRY_RELATIVE_PATH;
    std::ostringstream writeBuffer;
    for(const auto& relativePath : relativePaths) {
        writeBuffer << relativePath << "\n";
    }
    std::ofstream writeFile(usageRegistryFullPath);
    if(!writeFile.is_open()) {
        std::cerr << "Failed to open usage registry file for writing: " << usageRegistryFullPath << std::endl;
        return;
    }
    writeFile << writeBuffer.str();
#endif

    relativePaths.clear();
    shaders.clear();
    textures.clear();
    pathsTable.clear();
}