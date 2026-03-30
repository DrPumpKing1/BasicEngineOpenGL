#include "ResourceManager.h"
#include "../ResourceManager/ResourceManager.h"

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

void ResourceManager::NewShaderAsset(const std::string &relativePath, ShaderType type, std::shared_ptr<Shader>& shareTo)
{
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

void ResourceManager::NewTextureAsset(const std::string &relativePath, TextureType type, GLenum unit, bool transparency, std::shared_ptr<Texture>& shareTo) 
{
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

void ResourceManager::NewCubemapAsset(const std::string &relativeFolderPath, std::vector<std::string> facesFilenames, GLenum unit, std::shared_ptr<Cubemap>& shareTo) 
{
    if(facesFilenames.size() < 6) {
        std::cerr << "Not enough faces to make a cubemap texture" << std::endl;
        return;
    }

    std::vector<std::filesystem::path> texturesPaths;
    std::vector<std::filesystem::path> relativeTexturesPaths;
    for(unsigned int i = 0; i < 6; i++) 
    {
        std::filesystem::path relativePath(relativeFolderPath);
        relativePath /= facesFilenames[i];

        std::filesystem::path path = GetAssetPath(relativePath);
        if(path.empty()) {
            std::cerr << "Asset not found at path " << relativePath << std::endl;
            return;
        }

        texturesPaths.push_back(path);
        relativeTexturesPaths.push_back(relativePath);
    }

    CubemapAsset asset(texturesPaths, unit);
    cubemaps.push_back(asset);
    asset.Share(shareTo);

    for(const auto& path : relativeTexturesPaths) {
        if(std::find(relativePaths.begin(), relativePaths.end(), path.string()) == relativePaths.end()) 
        {
            relativePaths.push_back(path.string());
        }
    }
}

void ResourceManager::NewModelAsset(const std::string &relativePath, std::shared_ptr<Model>& shareTo)
{
    std::filesystem::path path = GetAssetPath(relativePath);
    if(path.empty()) {
        std::cerr << "Asset not found at path " << relativePath << std::endl;
        return;
    }
    ModelAsset asset(path.string());
    models.push_back(asset);
    asset.Share(shareTo);
    if(std::find(relativePaths.begin(), relativePaths.end(), relativePath) == relativePaths.end()) {
        relativePaths.push_back(relativePath);
    }

    std::filesystem::path relativeModelPath(relativePath);
    std::filesystem::path materialPath = std::filesystem::path(relativePath).parent_path() / (path.stem().string() + ".mtl");
    if (pathsTable.find(materialPath) != pathsTable.end())
    {
        if (std::find(relativePaths.begin(), relativePaths.end(), materialPath.string()) == relativePaths.end()) {
            relativePaths.push_back(materialPath.string());
        }
    }
}

void ResourceManager::Release() 
{
#ifdef DEBUG
    std::filesystem::path usageRegistryFullPath = std::filesystem::path(project_source_directory) / USAGE_REGISTRY_RELATIVE_PATH;
    std::ostringstream writeBuffer;
    for(const auto& relativePath : relativePaths) 
    {
        writeBuffer << relativePath << "\n";
    }
    std::ofstream writeFile(usageRegistryFullPath);
    if(!writeFile.is_open()) 
    {
        std::cerr << "Failed to open usage registry file for writing: " << usageRegistryFullPath << std::endl;
        return;
    }
    writeFile << writeBuffer.str();
#endif

    relativePaths.clear();
    shaders.clear();
    textures.clear();
    cubemaps.clear();
    models.clear();
    pathsTable.clear();
}