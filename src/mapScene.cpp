#include "mapScene.h"
#include "playerPawn.h"

#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/collision/simple2d/shape.h>


MapScene::MapScene(sp::string scene_name)
: sp::Scene(scene_name)
{
    sp::P<sp::Camera> camera = new sp::Camera(getRoot());
    camera->setOrtographic(8.0, sp::Camera::Direction::Horizontal);
    setDefaultCamera(camera);
}

void MapScene::loadMap(sp::string map_name)
{
    map_data = std::unique_ptr<MapData>(new MapData(map_name));
    getCamera()->setPosition(sp::Vector2d(map_data->size.x / 2, map_data->size.y / 2));

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
                sp::collision::Simple2DShape shape(sp::Rect2d(0.5, 0.5, 1, 1));
                shape.type = sp::collision::Shape::Type::Static;
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
            }break;
            case MapData::TileType::SolidCorner_UL:{
                sp::Node* n = new sp::Node(getRoot());
                sp::collision::Simple2DShape shape(sp::Rect2d(0.25, 0.25, 0.5, 0.5));
                shape.type = sp::collision::Shape::Type::Static;
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
                n = new sp::Node(getRoot());
                shape.rect = sp::Rect2d(0.75, 0.5, 0.5, 1);
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
            }break;
            case MapData::TileType::SolidCorner_UR:{
                sp::Node* n = new sp::Node(getRoot());
                sp::collision::Simple2DShape shape(sp::Rect2d(0.75, 0.25, 0.5, 0.5));
                shape.type = sp::collision::Shape::Type::Static;
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
                n = new sp::Node(getRoot());
                shape.rect = sp::Rect2d(0.25, 0.5, 0.5, 1);
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
            }break;
            case MapData::TileType::SolidCorner_DL:{
                sp::Node* n = new sp::Node(getRoot());
                sp::collision::Simple2DShape shape(sp::Rect2d(0.25, 0.75, 0.5, 0.5));
                shape.type = sp::collision::Shape::Type::Static;
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
                n = new sp::Node(getRoot());
                shape.rect = sp::Rect2d(0.75, 0.5, 0.5, 1);
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
            }break;
            case MapData::TileType::SolidCorner_DR:{
                sp::Node* n = new sp::Node(getRoot());
                sp::collision::Simple2DShape shape(sp::Rect2d(0.75, 0.75, 0.5, 0.5));
                shape.type = sp::collision::Shape::Type::Static;
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
                n = new sp::Node(getRoot());
                shape.rect = sp::Rect2d(0.25, 0.5, 0.5, 1);
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
            }break;
            case MapData::TileType::SolidSide_L:{
                sp::Node* n = new sp::Node(getRoot());
                sp::collision::Simple2DShape shape(sp::Rect2d(0.25, 0.5, 0.5, 1));
                shape.type = sp::collision::Shape::Type::Static;
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
            }break;
            case MapData::TileType::SolidSide_R:{
                sp::Node* n = new sp::Node(getRoot());
                sp::collision::Simple2DShape shape(sp::Rect2d(0.75, 0.5, 0.5, 1));
                shape.type = sp::collision::Shape::Type::Static;
                n->setCollisionShape(shape);
                n->setPosition(sp::Vector2d(x, y));
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
}

void MapScene::unloadMap()
{
    for(auto node : getRoot()->getChildren())
    {
        sp::P<PlayerPawn> player = sp::P<sp::Node>(node);
        sp::P<sp::Camera> camera = sp::P<sp::Node>(node);
        if (!player && !camera)
        {
            delete node;
        }
    }
}
