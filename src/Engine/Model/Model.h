#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "../Texture/Texture.h"

#include <vector>
#include <filesystem>
#include <memory>

class Model 
{
private:
    std::vector<std::shared_ptr<Texture>> texturesLoaded;
    std::vector<Mesh> meshes;
    std::filesystem::path absolutePath;
    std::filesystem::path directory;

    unsigned int meshCounter;
public:
    Model(std::string const &path) : absolutePath(path)
    {
        meshCounter = 0;
        loadModel(path);
    }
    ~Model();

    void Draw(ShaderProgram &shaderProgram);
private:
    void loadModel(std::string const &path);
    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *node, const aiScene *scene);
    std::vector<std::weak_ptr<Texture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType internalType);
};