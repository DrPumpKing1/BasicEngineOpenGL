#include "FileSystem.h"

std::filesystem::path const &FileSystem::GetRoot()
{
    static char const *envRoot{getenv("LOGL_PATH_ROOT")};
    static char const *givenRoot = envRoot != nullptr ? envRoot : logl_root;
    static std::filesystem::path root{givenRoot != nullptr ? givenRoot : ""};
    return root;
}

FileSystem::Builder FileSystem::GetPathBuilder()
{
    std::string const &root {GetRoot().string()};
    if(GetRoot() != "") {
        return &FileSystem::GetPathRelativeRoot;
    } else {
        return &FileSystem::GetPathRelativeBinary;
    }
}

std::string FileSystem::GetPathRelativeRoot(const std::string &path)
{
    return (GetRoot()/path).string();
}

std::string FileSystem::GetPathRelativeBinary(const std::string &path)
{
    return (GetRoot()/".."/".."/".."/path).string();
}