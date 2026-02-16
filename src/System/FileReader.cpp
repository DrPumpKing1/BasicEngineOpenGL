#include "FileReader.h"

std::string FileReader::ReadFile(const std::string &filePath)
{
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        fileStream.close();
        return buffer.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        return "";
    }
}