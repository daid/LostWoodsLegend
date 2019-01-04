#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <sp2/math/vector2.h>
#include <sp2/math/rect.h>
#include <sp2/string.h>
#include <vector>


class MapData
{
public:
    enum class TileType
    {
        Open,
        Solid,
        SolidCorner_UL,
        SolidCorner_UR,
        SolidCorner_DL,
        SolidCorner_DR,
        SolidSide_L,
        SolidSide_R,
        Water,
        Ladder,
        CliffEdge,
        ShallowWater,
    };
    class ObjectInfo
    {
    public:
        sp::Rect2d area;
        sp::string type;
        sp::string name;
        std::map<sp::string, sp::string> properties;
    };

    MapData(sp::string name);

    sp::Vector2i size;
    sp::Rect2d play_area;
    std::vector<TileType> tile_types;
    std::vector<std::vector<int>> tiles;
    sp::Vector2i tilemap_size;
    sp::string tilemap_texture;
    
    bool darkness = false;
    
    std::vector<ObjectInfo> objects;
};

#endif//MAP_DATA_H
