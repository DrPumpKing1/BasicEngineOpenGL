#pragma once

#include <nlohmann/json.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/nil_generator.hpp>

enum class AssetType
{
    Shader,
    Texture,
    Model,
    Audio,
    Font,
};

std::string GetAssetTypeName(AssetType type);
AssetType GetAssetTypeByName(std::string typeName);
AssetType GetAssetTypeByExtension(const std::string &extension);
AssetType GetAssetTypeByPath(const std::filesystem::path &path);

struct AssetTag
{
    boost::uuids::uuid id;  // unique identifier for the asset
    std::string path;       // relative path to the resources asset file
    AssetType type;         // type of the asset
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AssetTag, id, path, type)
};

inline void to_json(nlohmann::json& j, const AssetTag& assetTag)
{
    j = nlohmann::json{
        {"id", boost::uuids::to_string(assetTag.id)},
        {"path", assetTag.path},
        {"type", GetAssetTypeName(assetTag.type)}
    };
}

inline void from_json(const nlohmann::json& j, AssetTag& assetTag)
{
    std::string idStr = j.at("id").get<std::string>();
    assetTag.id = boost::uuids::string_generator()(idStr);
    assetTag.path = j.at("path").get<std::string>();
    std::string typeStr = j.at("type").get<std::string>();
    assetTag.type = GetAssetTypeByName(typeStr);
}