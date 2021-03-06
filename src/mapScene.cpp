#include "mapScene.h"
#include "playerPawn.h"
#include "simpleEffect.h"
#include "collisionBits.h"
#include "entrance.h"
#include "npc.h"
#include "door.h"
#include "equipment.h"
#include "pushBlock.h"
#include "equipmentPickup.h"
#include "enemies/basicEnemy.h"

#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/tween.h>

#include <sp2/graphics/spriteAnimation.h>
#include <sp2/collision/simple2d/shape.h>

///DropsA: {"Rupee": 5, "hp": 4, "fairy": 1}
///DropsB: {"Rupee": 3, "hp": 3, "clock": 1, "bomb": 3}
///DropsC: {"Rupee": 5, "hp": 2, "clock": 1, "5rupees": 2}
///DropsD: {"Rupee": 2, "hp": 6, "fairy": 2}


MapScene::MapScene(sp::string scene_name)
: sp::Scene(scene_name)
{
    sp::P<sp::Camera> camera = new sp::Camera(getRoot());
    camera->setOrtographic(8.0, sp::Camera::Direction::Horizontal);
    setDefaultCamera(camera);
}

void MapScene::onFixedUpdate()
{
    if (transition != Transition::None)
    {
        getCamera()->setPosition(sp::Tween<sp::Vector2d>::easeOutCubic(transition_counter, 0, transition_time, camera_source_position, camera_target_position));
        transition_counter++;
        if (transition_counter == transition_time)
        {
            for(auto p : previous_tilemaps)
                delete p;
            transition = Transition::None;
            getCamera()->setPosition(camera_target_position);
        }
    }
}

void MapScene::loadMap(sp::string map_name)
{
    LOG(Info, "Loading map:", map_name);

    map_data = std::unique_ptr<MapData>(new MapData(map_name));
    camera_target_position = sp::Vector2d(map_data->size.x / 2, map_data->size.y / 2);

    sp::collision::Simple2DShape shape(sp::Rect2d(0, 0, 1, 1));
    shape.type = sp::collision::Shape::Type::Static;
    shape.setFilterCategory(CollisionCategory::walls);

    for(unsigned int tile_layer_nr=0; tile_layer_nr<map_data->tiles.size(); tile_layer_nr++)
    {
        sp::P<sp::Tilemap> tilemap = new sp::Tilemap(getRoot(), map_data->tilemap_texture, 1.0, 1.0, map_data->tilemap_size.x, map_data->tilemap_size.y);
        tilemap->setPosition(sp::Vector3d(0, 0, tile_layer_nr * 5.0));
        for(int y=0; y<map_data->size.y; y++)
        {
            for(int x=0; x<map_data->size.x; x++)
            {
                int idx = map_data->tiles[tile_layer_nr][x + y * map_data->size.x];
                tilemap->setTile(x, y, idx);

                switch(map_data->tile_types[idx])
                {
                case MapData::TileType::Open:
                    break;
                case MapData::TileType::Solid:{
                    sp::Node* n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.5, 0.5, 1, 1);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::SolidCorner_UL:{
                    sp::Node* n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.25, 0.25, 0.5, 0.5);
                    n->setCollisionShape(shape);
                    n->setPosition(sp::Vector2d(x, y));
                    n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.75, 0.5, 0.5, 1);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::SolidCorner_UR:{
                    sp::Node* n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.75, 0.25, 0.5, 0.5);
                    n->setCollisionShape(shape);
                    n->setPosition(sp::Vector2d(x, y));
                    n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.25, 0.5, 0.5, 1);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::SolidCorner_DL:{
                    sp::Node* n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.25, 0.75, 0.5, 0.5);
                    n->setCollisionShape(shape);
                    n->setPosition(sp::Vector2d(x, y));
                    n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.75, 0.5, 0.5, 1);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::SolidCorner_DR:{
                    sp::Node* n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.75, 0.75, 0.5, 0.5);
                    n->setCollisionShape(shape);
                    n->setPosition(sp::Vector2d(x, y));
                    n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.25, 0.5, 0.5, 1);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::SolidSide_L:{
                    sp::Node* n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.25, 0.5, 0.5, 1);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::SolidSide_R:{
                    sp::Node* n = new sp::Node(getRoot());
                    shape.rect = sp::Rect2d(0.75, 0.5, 0.5, 1);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::Water:{
                    sp::Node* n = new sp::Node(getRoot());
                    sp::collision::Simple2DShape shape(sp::Rect2d(0.5, 0.5, 1, 1));
                    shape.type = sp::collision::Shape::Type::Static;
                    shape.setFilterCategory(CollisionCategory::water);
                    n->setPosition(sp::Vector2d(x, y));
                    n->setCollisionShape(shape);
                }break;
                case MapData::TileType::Ladder:{
                }break;
                case MapData::TileType::CliffEdge:{
                }break;
                case MapData::TileType::ShallowWater:{
                }break;
                }
            }
        }
    }

    //Create the level edge collision, so enemies do not walk over the edge of the map.
    {
        sp::collision::Simple2DShape shape(sp::Rect2d(map_data->size.x / 2, 0, map_data->size.x, 1));
        shape.type = sp::collision::Shape::Type::Static;
        shape.setFilterCategory(CollisionCategory::level_edge);

        sp::Node* n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(0, map_data->play_area.position.x-0.5));
        n->setCollisionShape(shape);

        n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(0, map_data->play_area.position.y + map_data->play_area.size.y + 0.5));
        n->setCollisionShape(shape);

        shape.rect = sp::Rect2d(0, map_data->size.y / 2, 1, map_data->size.y);
        n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(map_data->play_area.position.x-0.5, 0));
        n->setCollisionShape(shape);

        n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(map_data->play_area.position.x + map_data->play_area.size.x + 0.5, 0));
        n->setCollisionShape(shape);
    }

    if (transition == Transition::None)
        getCamera()->setPosition(camera_target_position);
    else
        getCamera()->setPosition(camera_source_position);

    for(auto& object : map_data->objects)
    {
        sp::Vector2d position = object.area.center();
        if (object.type == "ENEMY")
        {
            if (object.properties["position"] == "random")
            {
                do {
                    position = sp::Vector2d(findRandomPosition()) + sp::Vector2d(0.5, 0.5);
                } while(minimalPlayerDistance(position) < 4.0);
            }
            if (object.properties["position"] == "random_side")
            {
                do {
                    position = sp::Vector2d(findRandomSidePosition()) + sp::Vector2d(0.5, 0.5);
                } while(minimalPlayerDistance(position) < 4.0);
            }
            auto t = BasicEnemy::templates.find(object.name);
            sp::P<Enemy> enemy;
            if (t != BasicEnemy::templates.end())
                enemy = new BasicEnemy(getRoot(), t->second);
            else
                LOG(Warning, "Unknown enemy type:", object.name);
            if (enemy)
            {
                enemy->setPosition(position);
                if (object.properties["carry"] != "")
                    enemy->setCarry(findEquipment<Equipment>(object.properties["carry"]));
            }
        }
        else if(object.type == "NPC")
        {
            NPC* npc = new NPC(getRoot(), object.name, object.properties);
            npc->setPosition(position);
        }
        else if(object.type == "PICKUP")
        {
            EquipmentPickup* pickup = new EquipmentPickup(getRoot(), object.name);
            pickup->setPosition(object.area.center());
            if (object.properties["SPAWN"] == "no enemies")
                pickup->hideTillEnemiesKilled();
        }
        else if(object.type == "ENTRANCE")
        {
            new Entrance(getRoot(), object.area, object.name, sp::stringutil::convert::toVector2d(object.properties["exit"]), object.properties["dungeon"]);
        }
        else if(object.type == "DOOR")
        {
            Door* door = new Door(getRoot(), position, object.name.split("#")[0], object.name.split("#")[1]);
            if (object.properties["use_item"] != "")
                door->setUseItem(findEquipment<CollectableEquipment>(object.properties["use_item"]));
            if (object.properties["trigger"] != "")
                door->setTriggerName(object.properties["trigger"]);
        }
        else if(object.type == "PUSH_BLOCK")
        {
            PushBlock* push_block = new PushBlock(getRoot(), position, object.name.split("#")[0], object.name.split("#")[1]);
        }
        else if(object.type == "TRIGGER_ALL_DEAD")
        {
            new TriggerAllEnemiesDead(getRoot(), object.name);
        }
        else
        {
            LOG(Warning, "Unknown object type on map:", object.type);
        }
    }
}

void MapScene::unloadMap(Transition transition)
{
    for(auto p : previous_tilemaps)
        delete p;

    this->transition = transition;
    transition_counter = 0;
    switch(transition)
    {
    case Transition::None: break;
    case Transition::Up: camera_source_position = getCamera()->getPosition2D() + sp::Vector2d(0, -map_data->size.y); break;
    case Transition::Down: camera_source_position = getCamera()->getPosition2D() + sp::Vector2d(0, map_data->size.y); break;
    case Transition::Left: camera_source_position = getCamera()->getPosition2D() + sp::Vector2d(map_data->size.x, 0); break;
    case Transition::Right: camera_source_position = getCamera()->getPosition2D() + sp::Vector2d(-map_data->size.x, 0); break;
    }

    for(auto _node : getRoot()->getChildren())
    {
        sp::P<sp::Node> node(_node);

        sp::P<sp::Tilemap> tilemap = node;
        if (tilemap)
        {
            switch(transition)
            {
            case Transition::None: tilemap.destroy(); break;
            case Transition::Up: tilemap->setPosition(sp::Vector2d(0, -map_data->size.y)); break;
            case Transition::Down: tilemap->setPosition(sp::Vector2d(0, map_data->size.y)); break;
            case Transition::Left: tilemap->setPosition(sp::Vector2d(map_data->size.x, 0)); break;
            case Transition::Right: tilemap->setPosition(sp::Vector2d(-map_data->size.x, 0)); break;
            }
            previous_tilemaps.add(tilemap);
            continue;
        }

        sp::P<PlayerPawn> player = node;
        sp::P<sp::Camera> camera = node;
        if (!player && !camera)
            node.destroy();
    }
}

sp::Vector2i MapScene::findRandomPosition()
{
    while(true)
    {
        sp::Vector2i position(sp::irandom(1, map_data->size.x - 1), sp::irandom(1, map_data->size.y - 1));
        if (map_data->tile_types[map_data->tiles[0][position.x + position.y * map_data->size.x]] == MapData::TileType::Open)
            return position;
    }
}

sp::Vector2i MapScene::findRandomSidePosition()
{
    while(true)
    {
        sp::Vector2i position;
        switch(sp::irandom(0, 3))
        {
        case 0: position = sp::Vector2i(0, sp::irandom(1, map_data->size.y - 1)); break;
        case 1: position = sp::Vector2i(map_data->size.x - 1, sp::irandom(1, map_data->size.y - 1)); break;
        case 2: position = sp::Vector2i(sp::irandom(1, map_data->size.x - 1), 0); break;
        case 3: position = sp::Vector2i(sp::irandom(1, map_data->size.x - 1), map_data->size.y - 1); break;
        }
        if (map_data->tile_types[map_data->tiles[0][position.x + position.y * map_data->size.x]] == MapData::TileType::Open)
            return position;
    }
}

double MapScene::minimalPlayerDistance(sp::Vector2d position)
{
    double distance = map_data->size.x + map_data->size.y;
    for(sp::Node* obj : getRoot()->getChildren())
    {
        sp::P<PlayerPawn> player = sp::P<sp::Node>(obj);
        if (player)
            distance = std::min(distance, (player->getPosition2D() - position).length());
    }
    return distance;
}
