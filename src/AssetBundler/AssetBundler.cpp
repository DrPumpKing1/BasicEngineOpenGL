#include "AssetBundler.h"

AssetBundler::AssetBundler(const std::string &inputDirectory, const std::string &outputDirectory) : inputDirectory(inputDirectory), outputDirectory(outputDirectory)
{
    ReadUsageRegistry();
    for (const auto &relativePath : relativePaths)
    {
        CopyFile(relativePath);
    }
}

AssetBundler::~AssetBundler()
{
    relativePaths.clear();
}

void AssetBundler::ReadUsageRegistry() {
    std::filesystem::path usageRegistryFullPath = std::filesystem::path(project_source_directory) / USAGE_REGISTRY_RELATIVE_PATH;
    std::ifstream readFile(usageRegistryFullPath);
    if(!readFile.is_open()) {
        std::cerr << "Usage registry not found in " << usageRegistryFullPath << std::endl;
        return;
    }
    std::string line;
    while(std::getline(readFile, line)) {
        relativePaths.push_back(line);
    }
}

void AssetBundler::CopyFile(const std::string &relativePath) {
    if (relativePath.empty()) return;

    std::filesystem::path sourcePath = std::filesystem::path(inputDirectory) / relativePath;
    std::filesystem::path destinationPath = std::filesystem::path(outputDirectory) / relativePath;

    std::filesystem::path folderPath = destinationPath.parent_path();
    if (!folderPath.empty()) {
        std::filesystem::create_directories(folderPath);
    }

    std::filesystem::copy_file(sourcePath, destinationPath, std::filesystem::copy_options::overwrite_existing);
}