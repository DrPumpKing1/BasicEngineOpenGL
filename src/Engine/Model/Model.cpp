#include "Model.h"
#include "../ResourceManager/ResourceManager.h"

Model::~Model() {
    meshes.clear();
    texturesLoaded.clear();
}

void Model::Draw(ShaderProgram &shaderProgram) {
    for(unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shaderProgram);
    }
}

void Model::loadModel(std::string const& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(absolutePath.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::filesystem::path parentPath = absolutePath.parent_path(); 
    
    auto it = std::find(parentPath.begin(), parentPath.end(), "resources");
    if(it == parentPath.end()) 
        throw std::runtime_error("Model is not in a resources folder");

    it++;
    
    for(; it != parentPath.end(); it++) {
        directory /= *it;
    }

    meshes.resize(scene->mNumMeshes);
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex3DTangent> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::weak_ptr<Texture>> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex3DTangent vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        if(mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        if(mesh->mTextureCoords[0]) {
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;

            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<std::weak_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<std::weak_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<std::weak_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::NORMAL);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<std::weak_ptr<Texture>> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::HEIGHT);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    meshes[meshCounter++].Construct(vertices, indices, textures);
}

std::vector<std::weak_ptr<Texture>> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType internalType) {
    std::vector<std::weak_ptr<Texture>> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);
        std::filesystem::path textureRelativePath = directory / texturePath.C_Str();
        std::filesystem::path textureAbsolutePath = absolutePath.parent_path() / texturePath.C_Str();
        bool alreadyLoaded = false;
        for(unsigned int j = 0; j < texturesLoaded.size(); j++) {
            if(std::strcmp(texturesLoaded[j]->path.c_str(), textureAbsolutePath.string().c_str()) == 0) {
                textures.push_back(texturesLoaded[j]);
                alreadyLoaded = true;
                break;
            }
        }
        if(!alreadyLoaded) {
            std::shared_ptr<Texture> texture;
            ResourceManager::Instance().NewTextureAsset(textureRelativePath.string(), internalType, texturesLoaded.size(), false, texture);
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }
    return textures;
}