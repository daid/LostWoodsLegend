#include "playerPawn.h"
#include "playerInfo.h"
#include "enemies/enemy.h"
#include "collisionBits.h"
#include "lightSource.h"

#include <sp2/graphics/spriteAnimation.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/meshdata.h>
#include <sp2/collision/simple2d/shape.h>

class UseSword : public sp::Node
{
public:
    UseSword(sp::P<PlayerPawn> source, Direction direction)
    : sp::Node(source->getParent())
    {
        setAnimation(sp::SpriteAnimation::load("zelda1/sprites/items.txt"));
        render_data.shader = sp::Shader::get("object.shader");
        animationPlay("SWORD_0");
        setPosition(source->getPosition2D() + sp::Vector2d(0, -0.1) + direction.toVector() * 0.8);
        setRotation(direction.toAngle());

        sp::collision::Simple2DShape shape(sp::Vector2d(0.6, 1.1));
        if (direction == Direction::Left || direction == Direction::Right)
            std::swap(shape.rect.size.x, shape.rect.size.y);
        shape.type = sp::collision::Shape::Type::Sensor;
        shape.setFilterCategory(CollisionCategory::player_projectile);
        setCollisionShape(shape);
    }

    virtual void onFixedUpdate() override
    {
        if (!timeout--)
            delete this;
    }

    virtual void onCollision(sp::CollisionInfo& info) override
    {
        sp::P<Enemy> enemy = info.other;
        if (enemy)
            enemy->onTakeDamage(10, source);
    }
private:
    sp::P<PlayerPawn> source;
    int timeout = 12;
};


PlayerPawn::PlayerPawn(sp::P<sp::Node> parent, PlayerInfo& player_info, Controls& controls)
: sp::Node(parent), player_info(player_info), controls(controls)
{
    setAnimation(sp::SpriteAnimation::load("zelda1/sprites/link.txt"));
    render_data.shader = sp::Shader::get("object.shader");
    setPosition(sp::Vector2d(8, 3.5));
    sp::collision::Simple2DShape shape(sp::Rect2d(0, -0.20, 0.8, 0.6));
    shape.setFilterCategory(CollisionCategory::player);
    shape.setMaskFilterCategory(CollisionCategory::player);//No need to get collision with other players.
    shape.setMaskFilterCategory(CollisionCategory::level_edge);//We ignore the level edge, this edge is to prevent enemies from leaving the level.
    shape.setMaskFilterCategory(CollisionCategory::player_projectile);//Ignore any projectile we fire.
    setCollisionShape(shape);

    shield_level = 0;
    
    light_source = new LightSource(this, 0.4);
}

void PlayerPawn::onFixedUpdate()
{
    if (equipment_pickup)
    {
        animationPlay("PICKUP_LARGE");
        animationSetFlags(0);
        equipment_pickup_timeout--;
        if (equipment_pickup_timeout < 1)
            equipment_pickup.destroy();
        return;
    }

    sp::Vector2d position = getPosition2D();
    sp::Vector2d move_request;
    if (active_item || hurt_delay > 0)
    {
        //We cannot do anything right now. No movement, no item use.
    }
    else
    {
        if (controls.primary_action.getDown())
        {
            if (player_info.active_items[0].equipment)
            {
                if (player_info.active_items[0].equipment->id == "SWORD")
                    active_item = new UseSword(this, direction);
            }
        }
        else
        {
            move_request = controls.getMoveRequest();
        }
    }

    if (hurt_delay > 0 || invincibility_time > 0)
    {
        if (hurt_delay > 0)
            hurt_delay--;
        else
            invincibility_time--;
        
        switch((hurt_delay + invincibility_time) & 3)
        {
        case 0: render_data.color = sp::Color(1, 1, 1); break;
        case 1: render_data.color = sp::Color(1, 1, 0); break;
        case 2: render_data.color = sp::Color(1, 0, 1); break;
        case 3: render_data.color = sp::Color(0, 1, 1); break;
        }
    }
    
    if (hurt_delay > 0)
    {
        setPosition(getPosition2D() + hurt_direction * 10.0 / 60.0);
    }
    else
    {
        setPosition(getPosition2D() + move_request * 5.0 / 60.0);
    }
    direction.update(move_request);


    sp::string animation_name = "IDLE";
    if (move_request.x || move_request.y)
    {
        if (position != previous_position)
            animation_name = "WALK";
    }
    if (active_item)
        animation_name = "ATTACK";

    switch(direction)
    {
    case Direction::Left:
        animationPlay(animation_name + "_R_" + sp::string(shield_level));
        animationSetFlags(sp::SpriteAnimation::FlipFlag);
        break;
    case Direction::Right:
        animationPlay(animation_name + "_R_" + sp::string(shield_level));
        animationSetFlags(0);
        break;
    case Direction::Up:
        animationPlay(animation_name + "_U_" + sp::string(shield_level));
        animationSetFlags(0);
        break;
    case Direction::Down:
        animationPlay(animation_name + "_D_" + sp::string(shield_level));
        animationSetFlags(0);
        break;
    }
    previous_position = position;
}

bool PlayerPawn::onTakeDamage(int amount, sp::Vector2d source)
{
    if (equipment_pickup)
        return false;
    if (hurt_delay > 0 || invincibility_time > 0)
        return false;

    hurt_delay = 10;
    invincibility_time = 30;
    hurt_direction = (getPosition2D() - source).normalized();
    return true;
}

void PlayerPawn::showEquipmentPickup(const Equipment* equipment)
{
    equipment_pickup = new sp::Node(this);
    equipment_pickup->render_data.type = sp::RenderData::Type::Normal;
    equipment_pickup->render_data.texture = sp::texture_manager.get(equipment->sprite);
    equipment_pickup->render_data.mesh = sp::MeshData::createQuad(sp::Vector2f(1, 1), sp::Vector2f(equipment->sprite_uv.position), sp::Vector2f(equipment->sprite_uv.position + equipment->sprite_uv.size));
    equipment_pickup->render_data.shader = sp::Shader::get("object.shader");
    equipment_pickup->setPosition(sp::Vector3d(0, 1, 0.1));
    equipment_pickup_timeout = 120;
}
