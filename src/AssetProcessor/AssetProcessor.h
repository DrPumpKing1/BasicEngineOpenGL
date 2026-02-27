#pragma once

#include "root_directory.h"
#include "AssetTag.h"

#include <string>
#include <filesystem>
#include <vector>
#include <format>
#include <map>
#include <fstream>
#include <iostream>

const std::string REGISTRY_FILENAME = "asset_registry.json";
const std::string TAG_EXTENSION = ".json";

const std::unordered_map<std::string, AssetType> assetTypeMap = {
    {".vs", AssetType::Shader},
    {".fs", AssetType::Shader},
    {".gs", AssetType::Shader},
    {".tcs", AssetType::Shader},
    {".tes", AssetType::Shader},
    {".cs", AssetType::Shader},
    {".png", AssetType::Texture},
    {".jpg", AssetType::Texture},
    {".jpeg", AssetType::Texture},
    {".bmp", AssetType::Texture},
    {".obj", AssetType::Model},
    {".fbx", AssetType::Model},
    {".wav", AssetType::Audio},
    {".mp3", AssetType::Audio},
    {".ttf", AssetType::Font}
};

inline std::vector<std::string> supportedExtensions = {
    ".vs", ".fs", ".gs", ".tcs", ".tes", ".cs",
    ".png", ".jpg", ".jpeg", ".bmp",
    ".obj", ".fbx", ".wav", ".mp3", ".ttf"
};

class AssetProcessor
{
private:
    std::string inputDirectory;
    std::string outputDirectory;

    std::unordered_map<boost::uuids::uuid, AssetTag> assetRegistry = {};
public:
    AssetProcessor(const std::string &inputDirectory, const std::string &outputDirectory);
private:
    void LoadAssetRegistry();
    void SaveAssetRegistry();

    void ProcessAssets(std::filesystem::path path);
    void ProcessAsset(std::filesystem::path assetPath);

    std::filesystem::path GetAssetTagPath(const std::filesystem::path& assetPath);
};