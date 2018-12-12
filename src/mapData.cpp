#include "mapData.h"

#include <sp2/io/resourceProvider.h>
#include <json11/json11.hpp>


MapData::MapData(sp::string name)
{
    sp::string err;
    json11::Json map_json = json11::Json::parse(sp::io::ResourceProvider::get(name)->readAll(), err);

    size.x = map_json["width"].int_value();
    size.y = map_json["height"].int_value();
    int index_offset = map_json["tilesets"][0]["firstgid"].int_value();

    const json11::Json& tile_data_json = map_json["layers"][0]["data"];
    tiles.resize(size.x * size.y);
    for (int y = 0; y < size.y; y++)
        for (int x = 0; x < size.x; x++)
            tiles[x + y * size.x] = tile_data_json[x + (size.y - 1 - y) * size.x].int_value() - index_offset;

    sp::string tileset_name = map_json["tilesets"][0]["source"].string_value();
    tileset_name = name.substr(0, name.rfind("/")) + "/" + tileset_name;
    json11::Json tileset_json = json11::Json::parse(sp::io::ResourceProvider::get(tileset_name)->readAll(), err);
    tile_types.resize(tileset_json["tilecount"].int_value(), TileType::Open);
    for (auto& tile_info_json : tileset_json["tiles"].array_items())
    {
        int id = tile_info_json["id"].int_value();
        sp::string type = tile_info_json["type"].string_value();
        type = type.upper();
        if (type == "WATER")
            tile_types[id] = TileType::Water;
        else if (type == "SOLID")
            tile_types[id] = TileType::Solid;
        else if (type == "CORNER_UL")
            tile_types[id] = TileType::SolidCorner_UL;
        else if (type == "CORNER_UR")
            tile_types[id] = TileType::SolidCorner_UR;
        else if (type == "CORNER_DL")
            tile_types[id] = TileType::SolidCorner_DL;
        else if (type == "CORNER_DR")
            tile_types[id] = TileType::SolidCorner_DR;
        else if (type == "LADDER")
            tile_types[id] = TileType::Ladder;
        else if (type == "CLIFFEDGE")
            tile_types[id] = TileType::CliffEdge;
        else if (type == "SHALLOWWATER")
            tile_types[id] = TileType::ShallowWater;
    }
    tilemap_size.x = tileset_json["columns"].int_value();
    tilemap_size.y = (tileset_json["tilecount"].int_value() + tilemap_size.x - 1) / tilemap_size.x;
    tilemap_texture = name.substr(0, name.rfind("/")) + "/" + tileset_json["image"].string_value();
}
