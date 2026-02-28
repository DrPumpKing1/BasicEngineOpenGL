#pragma once

#include "root_directory.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>

const std::string USAGE_REGISTRY_RELATIVE_PATH = "resources/usage_registry.txt";

class AssetBundler 
{
private:
    std::string inputDirectory;
    std::string outputDirectory;

    std::vector<std::string> relativePaths;
public:
    AssetBundler(const std::string &inputDirectory, const std::string &outputDirectory);
    ~AssetBundler();
private:
    void ReadUsageRegistry();
    void CopyFile(const std::string &relativePath);
};