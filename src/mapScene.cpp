#include "mapScene.h"
#include "playerPawn.h"
#include "simpleEffect.h"
#include "collisionBits.h"
#include "entrance.h"
#include "enemies/basicEnemy.h"

#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/collision/simple2d/shape.h>
#include <sp2/tween.h>

#include <sp2/graphics/spriteAnimation.h>
#include <sp2/collision/simple2d/shape.h>

///DropsA: {"Rupee": 5, "hp": 4, "fairy": 1}
///DropsB: {"Rupee": 3, "hp": 3, "clock": 1, "bomb": 3}
///DropsC: {"Rupee": 5, "hp": 2, "clock": 1, "5rupees": 2}
///DropsD: {"Rupee": 2, "hp": 6, "fairy": 2}


BasicEnemy::Template enemy_octo{
    sprite: "zelda1/sprites/octo.txt",
    collision_rect: sp::Rect2d(0, 0, 0.8, 0.8),
    hp: 10,
    hit_player_damage: 5,
    walk_speed: 1.5,

    projectile_sprite: "zelda1/sprites/octo_rock.txt",
    projectile_collision_rect: sp::Rect2d(0, 0, 0.2, 0.2),
    projectile_speed: 7,
    fire_delay: 35,
};
BasicEnemy::Template enemy_octo_blue{
    sprite: "zelda1/sprites/octo_blue.txt",
    collision_rect: sp::Rect2d(0, 0, 0.8, 0.8),
    hp: 20,
    hit_player_damage: 5,
    walk_speed: 1.5,

    projectile_sprite: "zelda1/sprites/octo_rock.txt",
    projectile_collision_rect: sp::Rect2d(0, 0, 0.2, 0.2),
    projectile_speed: 7,
    fire_delay: 35,
};


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
            previous_tilemap.destroy();
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

    sp::P<sp::Tilemap> tilemap = new sp::Tilemap(getRoot(), map_data->tilemap_texture, 1.0, 1.0, map_data->tilemap_size.x, map_data->tilemap_size.y);
    for(int y=0; y<map_data->size.y; y++)
    {
        for(int x=0; x<map_data->size.x; x++)
        {
            int idx = map_data->tiles[x + y * map_data->size.x];
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

    //Create the level edge collision, so enemies do not walk over the edge of the map.
    {
        sp::collision::Simple2DShape shape(sp::Rect2d(map_data->size.x / 2, 0, map_data->size.x, 1));
        shape.type = sp::collision::Shape::Type::Static;
        shape.setFilterCategory(CollisionCategory::level_edge);

        sp::Node* n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(0, -0.5));
        n->setCollisionShape(shape);

        n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(0, map_data->size.y + 0.5));
        n->setCollisionShape(shape);

        shape.rect = sp::Rect2d(0, map_data->size.y / 2, 1, map_data->size.y);
        n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(-0.5, 0));
        n->setCollisionShape(shape);

        n = new sp::Node(getRoot());
        n->setPosition(sp::Vector2d(map_data->size.x + 0.5, 0));
        n->setCollisionShape(shape);
    }

    if (transition == Transition::None)
        getCamera()->setPosition(camera_target_position);
    else
        getCamera()->setPosition(camera_source_position);

    for(auto& object : map_data->objects)
    {
        sp::Vector2d position = object.area.position + object.area.size * 0.5;
        if (object.type == "ENEMY")
        {
            if (object.properties.find("position")->second == "random")
            {
                do {
                    position = sp::Vector2d(findRandomPosition()) + sp::Vector2d(0.5, 0.5);
                } while(minimalPlayerDistance(position) < 3.0);
            }
            if (object.properties.find("position")->second == "random_side")
            {
                do {
                    position = sp::Vector2d(findRandomSidePosition()) + sp::Vector2d(0.5, 0.5);
                } while(minimalPlayerDistance(position) < 3.0);
            }
            auto t = BasicEnemy::templates.find(object.name);
            if (t != BasicEnemy::templates.end())
                (new BasicEnemy(getRoot(), t->second))->setPosition(position);
            else
                LOG(Warning, "Unknown enemy type:", object.name);
        }
        else if(object.type == "ENTRANCE")
        {
            new Entrance(getRoot(), object.area, object.name, sp::stringutil::convert::toVector2d(object.properties["exit"]));
        }
        else
        {
            LOG(Warning, "Unknown object type on map:", object.type);
        }
    }
}

void MapScene::unloadMap(Transition transition)
{
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
        if (tilemap && tilemap != previous_tilemap)
        {
            switch(transition)
            {
            case Transition::None: tilemap.destroy(); break;
            case Transition::Up: tilemap->setPosition(sp::Vector2d(0, -map_data->size.y)); break;
            case Transition::Down: tilemap->setPosition(sp::Vector2d(0, map_data->size.y)); break;
            case Transition::Left: tilemap->setPosition(sp::Vector2d(map_data->size.x, 0)); break;
            case Transition::Right: tilemap->setPosition(sp::Vector2d(-map_data->size.x, 0)); break;
            }
            previous_tilemap = tilemap;
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
        if (map_data->tile_types[map_data->tiles[position.x + position.y * map_data->size.x]] == MapData::TileType::Open)
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
        if (map_data->tile_types[map_data->tiles[position.x + position.y * map_data->size.x]] == MapData::TileType::Open)
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
