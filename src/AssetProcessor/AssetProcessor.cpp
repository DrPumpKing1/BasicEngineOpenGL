#include "AssetProcessor.h"

AssetProcessor::AssetProcessor(std::string const& inputDirectory, std::string const& outputDirectory) : inputDirectory(inputDirectory), outputDirectory(outputDirectory)
{
    LoadAssetRegistry();
    ProcessAssets(std::filesystem::path(project_source_directory) / inputDirectory);
    SaveAssetRegistry();
}

void AssetProcessor::LoadAssetRegistry()
{
    std::cout << "Loading asset registry from: " << std::filesystem::path(project_source_directory) / outputDirectory / REGISTRY_FILENAME << std::endl;
    if(!std::filesystem::exists(std::filesystem::path(project_source_directory) / outputDirectory / REGISTRY_FILENAME))
    {
        return;
    }

    std::ifstream registryFile((std::filesystem::path(project_source_directory) / outputDirectory / REGISTRY_FILENAME).string());
    if (!registryFile.is_open())
    {
        std::cerr << "Failed to open asset registry file: " << std::filesystem::path(project_source_directory) / outputDirectory / REGISTRY_FILENAME << std::endl;
        return;
    }

    nlohmann::json registryJson = nlohmann::json::array();
    registryFile >> registryJson;
    registryFile.close();

    assetRegistry.clear();
    for(auto& item : registryJson.items())
    {
        AssetTag tag = item.value().get<AssetTag>();
        assetRegistry[tag.id] = tag;
    }
}

void AssetProcessor::SaveAssetRegistry()
{
    nlohmann::json registryJson = nlohmann::json::array();
    for (const auto& [id, tag] : assetRegistry)
    {
        registryJson.push_back(tag);
    }

    std::filesystem::path registryPath = std::filesystem::path(project_source_directory) / outputDirectory / REGISTRY_FILENAME;
    std::ofstream registryFile(registryPath);
    if (!registryFile.is_open())
    {
        std::cerr << "Failed to open asset registry file for writing: " << registryPath << std::endl;
        return;
    }

    registryFile << registryJson.dump(4);
    registryFile.close();
}

void AssetProcessor::ProcessAssets(std::filesystem::path path)
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            ProcessAsset(entry.path());
        }
    }
}

void AssetProcessor::ProcessAsset(std::filesystem::path assetPath)
{
    std::string extension = assetPath.extension().string();
    bool isTagFile = extension == TAG_EXTENSION;
    if(!isTagFile) {
        bool isSupportedExtension = std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end();
        if(!isSupportedExtension) {
            return;
        }
    }

    bool isNewItem = false;
    AssetTag existingTag;

    std::filesystem::path tagPath = isTagFile ? assetPath : GetAssetTagPath(assetPath);
    if (std::filesystem::exists(tagPath))
    {
        std::ifstream tagFile(tagPath.string());
        if (!tagFile.is_open())
        {
            std::cerr << "Failed to open asset tag file: " << tagPath << std::endl;
            return;
        }

        nlohmann::json tagJson;
        tagFile >> tagJson;
        tagFile.close();

        existingTag = tagJson.get<AssetTag>();
    }
    else 
    {
        existingTag.path = std::filesystem::relative(assetPath, std::filesystem::path(project_source_directory) / inputDirectory).string();
        std::cout << "No existing tag found for asset: " << assetPath << std::endl;
        isNewItem = true;
    }

    if (!isNewItem && existingTag.id == boost::uuids::nil_uuid())
    {
        existingTag.id = boost::uuids::random_generator()();
        isNewItem = true;
    }

    if(isTagFile) {
        std::filesystem::path tagAssetPath = std::filesystem::path(project_source_directory) / inputDirectory / existingTag.path;
        if(!std::filesystem::exists(tagAssetPath)) {
            std::filesystem::remove(tagPath);
        }
        return;
    }

    if (!isNewItem && assetRegistry.contains(existingTag.id))
    {
        std::filesystem::path registryAssetPath = std::filesystem::path(project_source_directory) / inputDirectory / assetRegistry[existingTag.id].path;
        if (registryAssetPath != assetPath)
        {
            std::cout << "Asset has been moved from " << registryAssetPath << " to " << assetPath << std::endl;
            std::filesystem::path oldTagPath = GetAssetTagPath(registryAssetPath);
            if(std::filesystem::exists(oldTagPath))
            {
                std::filesystem::remove(oldTagPath);
            }
            existingTag.path = std::filesystem::relative(assetPath, std::filesystem::path(project_source_directory) / inputDirectory).string();
            assetRegistry[existingTag.id].path = existingTag.path;
        }
    }
    else
    {
        std::cout << "New asset detected: " << assetPath << std::endl;
        existingTag.type = GetAssetTypeByPath(assetPath);
        assetRegistry[existingTag.id] = existingTag;
    }

    std::ofstream outTagFile(tagPath.string());
    if (!outTagFile.is_open()) {
        std::cerr << "Failed to open asset tag file for writing: " << tagPath << std::endl;
        return;
    }
    nlohmann::json outTagJson = existingTag;
    outTagFile << outTagJson.dump(4);
    outTagFile.close();
}