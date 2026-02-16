#include "FileSystem.h"

static void ReplaceAll(std::string &str, const std::string &from, const std::string &to) 
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

static std::string const &FileSystem::GetRoot()
{
    static char const *envRoot{getenv("LOGL_PATH_ROOT")};
    static char const *givenRoot = (envRoot != nullptr) ? envRoot : logl_root;
    static std::string root{givenRoot != nullptr ? givenRoot : ""};
    return root;
}

static FileSystem::Builder FileSystem::GetPathBuilder()
{
    std::string const &root {GetRoot()};
    if(root.empty()) {
        return GetPathRelativeBinary;
    } else {
        return GetPathRelativeRoot;
    }
}

static std::string FileSystem::GetPathRelativeRoot(const std::string &path)
{
    std::string fullPath {path};
    ReplaceAll(fullPath, "/", std::string(1, PATH_SEPARATOR));
    return GetRoot() + PATH_SEPARATOR + fullPath;
}

static std::string FileSystem::GetPathRelativeBinary(const std::string &path)
{
    std::string fullPath {path};
    ReplaceAll(fullPath, "/", std::string(1, PATH_SEPARATOR));
    return ".." + PATH_SEPARATOR + ".." + PATH_SEPARATOR + ".." + PATH_SEPARATOR + fullPath;
}