#include "mapScene.h"
#include "playerPawn.h"
#include "enemies/enemy.h"
#include "collisionBits.h"

#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/collision/simple2d/shape.h>
#include <sp2/tween.h>

#include <sp2/graphics/spriteAnimation.h>
#include <sp2/collision/simple2d/shape.h>

class OctoRock : public sp::Node
{
public:
    OctoRock(sp::P<Enemy> owner, Direction direction)
    : sp::Node(owner->getParent()), direction(direction)
    {
        setPosition(owner->getPosition2D() + direction.toVector() * 0.5);
        setAnimation(sp::SpriteAnimation::load("zelda1/sprites/octo_rock.txt"));
        render_data.shader = sp::Shader::get("object.shader");
        animationPlay("ROCK");

        sp::collision::Simple2DShape shape(sp::Vector2d(0.5, 0.5));
        shape.type = sp::collision::Shape::Type::Sensor;
        shape.setFilterCategory(CollisionCategory::enemy_projectile);
        shape.setMaskFilterCategory(CollisionCategory::enemy);
        shape.setMaskFilterCategory(CollisionCategory::enemy_projectile);
        setCollisionShape(shape);

        timeout = 60 * 10;
    }

    virtual void onFixedUpdate() override
    {
        setPosition(getPosition2D() + direction.toVector() * 0.15);
        if (timeout)
            timeout--;
        else
            delete this;
    }

    virtual void onCollision(sp::CollisionInfo& info)
    {
        if (info.other && info.other->isSolid())
        {
            delete this;
            return;
        }
        sp::P<PlayerPawn> player = info.other;
        if (player && player->onTakeDamage(5, this->getPosition2D()))
        {
            delete this;
            return;
        }
    }
private:
    Direction direction;
    int timeout;
};

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
        shape.setFilterCategory(CollisionCategory::enemy);
        shape.setMaskFilterCategory(CollisionCategory::enemy);
        shape.setMaskFilterCategory(CollisionCategory::enemy_projectile);
        setCollisionShape(shape);

        hp = 10;
        hit_damage = 5;
        state = State::Walk;
        state_delay = sp::random(100, 200);
        walk_direction = Direction::random();
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

                new OctoRock(this, walk_direction);
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
            player->onTakeDamage(hit_damage, this->getPosition2D());
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

void MapScene::onFixedUpdate()
{
    if (transition != Transition::None)
    {
        getCamera()->setPosition(sp::Tween<sp::Vector2d>::easeOutCubic(transition_counter, 0, transition_time, camera_source_position, camera_target_position));
        transition_counter++;
        if (transition_counter == transition_time)
        {
            transition = Transition::None;
            getCamera()->setPosition(camera_target_position);
        }
    }
}

void MapScene::loadMap(sp::string map_name)
{
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

    (new Octo(getRoot()))->setPosition(sp::Vector2d(4, 4));
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
