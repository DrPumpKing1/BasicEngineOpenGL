#include "AssetProcessor.h"

std::filesystem::path AssetProcessor::GetAssetTagPath(const std::filesystem::path& assetPath) {
    return std::filesystem::path(project_source_directory) / inputDirectory / assetPath.parent_path() / (assetPath.filename().string() + TAG_EXTENSION);
}

std::string GetAssetTypeName(AssetType type) {
    switch (type)
    {
    case AssetType::Shader:
        return "Shader";
    case AssetType::Texture:
        return "Texture";
    case AssetType::Model:
        return "Model";
    case AssetType::Audio:
        return "Audio";
    case AssetType::Font:
        return "Font";
    default:
        return "Unknown";
    }
}

AssetType GetAssetTypeByName(const std::string typeName){
    if (typeName == "Shader") return AssetType::Shader;
    if (typeName == "Texture") return AssetType::Texture;
    if (typeName == "Model") return AssetType::Model;
    if (typeName == "Audio") return AssetType::Audio;
    if (typeName == "Font") return AssetType::Font;
    throw std::runtime_error("Unknown asset type: " + typeName);
}

AssetType GetAssetTypeByExtension(const std::string &extension)
{
    auto it = assetTypeMap.find(extension);
    if (it != assetTypeMap.end())
    {
        return it->second;
    }
    throw std::runtime_error("Unsupported asset type: " + extension);
}

AssetType GetAssetTypeByPath(const std::filesystem::path &path)
{
    std::string extension = path.extension().string();
    return GetAssetTypeByExtension(extension);
}