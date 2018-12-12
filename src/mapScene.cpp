#include "mapScene.h"
#include "playerPawn.h"
#include "enemy.h"

#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/collision/simple2d/shape.h>

#include <sp2/graphics/spriteAnimation.h>
#include <sp2/collision/simple2d/shape.h>


class Octo : public Enemy
{
public:
    Octo(sp::P<sp::Node> parent)
    : Enemy(parent)
    {
        setAnimation(sp::SpriteAnimation::load("zelda1/sprites/octo.txt"));
        render_data.shader = sp::Shader::get("object.shader");
        animationPlay("DOWN");

        sp::collision::Simple2DShape shape(sp::Vector2d(0.8, 0.8));
        shape.type = sp::collision::Shape::Type::Dynamic;
        setCollisionShape(shape);

        hp = 10;
        hit_damage = 5;
        state = State::Walk;
        state_delay = sp::random(100, 200);
        hurt_counter = 0;
    }

    virtual void onFixedUpdate() override
    {
        switch(state)
        {
        case State::Walk:
            setPosition(getPosition2D() + walk_direction.toVector() * 0.02);
            if (state_delay > 0)
            {
                state_delay -= 1;
            }
            else
            {
                walk_direction = Direction::random();
                state_delay = sp::random(100, 200);
            }
            break;
        case State::Attack:
            break;
        }
        if (hurt_counter > 0)
        {
            hurt_counter--;
            switch(hurt_counter & 3)
            {
            case 0: render_data.color = sp::Color(1, 1, 1); break;
            case 1: render_data.color = sp::Color(1, 1, 0); break;
            case 2: render_data.color = sp::Color(1, 0, 1); break;
            case 3: render_data.color = sp::Color(0, 1, 1); break;
            }
        }

        switch(walk_direction)
        {
        case Direction::Left:
            animationPlay("RIGHT");
            animationSetFlags(sp::SpriteAnimation::FlipFlag);
            break;
        case Direction::Right:
            animationPlay("RIGHT");
            animationSetFlags(0);
            break;
        case Direction::Up:
            animationPlay("UP");
            animationSetFlags(0);
            break;
        case Direction::Down:
            animationPlay("DOWN");
            animationSetFlags(0);
            break;
        }
    }

    virtual bool onTakeDamage(int amount, sp::P<PlayerPawn> source) override
    {
        if (hurt_counter > 0)
            return false;

        LOG(Debug, "Damage:", amount);
        //hp -= amount;
        hurt_counter = 35;
        if (hp <= 0)
            delete this; //TODO: Spawn death animation object.
        return true;
    }

    virtual void onCollision(sp::CollisionInfo& info) override
    {
        if (info.other->isSolid() && state == State::Walk)
            state_delay -= 10;
    
        sp::P<PlayerPawn> player = info.other;
        if (player)
            player->onTakeDamage(hit_damage, this);
    }

    enum class State
    {
        Walk,
        Attack
    };

private:
    int hp;
    int hit_damage;
    int hurt_counter;

    State state;
    int state_delay;
    Direction walk_direction;
    int walk_distance;
};


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

    (new Octo(getRoot()))->setPosition(sp::Vector2d(4, 4));
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
