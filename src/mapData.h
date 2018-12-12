#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <sp2/math/vector2.h>
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
        Water,
        Ladder,
        CliffEdge,
        ShallowWater,
    };

    MapData(sp::string name);

    sp::Vector2i size;
    std::vector<TileType> tile_types;
    std::vector<int> tiles;
    sp::Vector2i tilemap_size;
    sp::string tilemap_texture;
};

#endif//MAP_DATA_H
