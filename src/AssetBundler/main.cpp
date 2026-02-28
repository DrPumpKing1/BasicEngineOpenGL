#include "AssetBundler.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: AssetBundler <asset_directory> <output_path>" << std::endl;
        return 1;
    }

    std::filesystem::path assetDirectory = argv[1];
    std::filesystem::path outputPath = argv[2];
    AssetBundler processor(assetDirectory.string(), outputPath.string());
    return 0;
}
