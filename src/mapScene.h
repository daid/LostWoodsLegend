#ifndef MAP_SCENE_H
#define MAP_SCENE_H

#include <sp2/scene/scene.h>
#include <memory>

#include "mapData.h"

#include <sp2/scene/tilemap.h>

class MapScene : public sp::Scene
{
public:
    enum class Transition
    {
        None,
        Left,
        Right,
        Up,
        Down
    };

    MapScene(sp::string scene_name);

    void loadMap(sp::string map_name);
    void unloadMap(Transition transition=Transition::None);

    virtual void onFixedUpdate() override;
private:
    std::unique_ptr<MapData> map_data;

    sp::P<sp::Tilemap> previous_tilemap;

    static constexpr int transition_time = 15;
    Transition transition = Transition::None;
    int transition_counter = 0;
    sp::Vector2d camera_source_position;
    sp::Vector2d camera_target_position;
};

#endif//MAP_SCENE_H