#ifndef MAP_SCENE_H
#define MAP_SCENE_H

#include <sp2/scene/scene.h>
#include <memory>

#include "mapData.h"


class MapScene : public sp::Scene
{
public:
    MapScene(sp::string scene_name);

    void loadMap(sp::string map_name);
    void unloadMap();
private:
    std::unique_ptr<MapData> map_data;
};

#endif//MAP_SCENE_H