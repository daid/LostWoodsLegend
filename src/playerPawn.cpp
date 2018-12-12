#include "playerPawn.h"
#include "enemy.h"

#include <sp2/graphics/spriteAnimation.h>
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


PlayerPawn::PlayerPawn(sp::P<sp::Node> parent, Controls& controls)
: sp::Node(parent), controls(controls)
{
    setAnimation(sp::SpriteAnimation::load("zelda1/sprites/link.txt"));
    render_data.shader = sp::Shader::get("object.shader");
    setPosition(sp::Vector2d(8, 3.5));
    setCollisionShape(sp::collision::Simple2DShape(sp::Rect2d(0, -0.20, 0.8, 0.6)));

    shield_level = 1;
}

void PlayerPawn::onFixedUpdate()
{
    sp::Vector2d position = getPosition2D();

    sp::Vector2d move_request;
    if (active_item || hurt_delay > 0)
    {
    }
    else
    {
        if (controls.primary_action.getDown())
        {
            active_item = new UseSword(this, direction);
        }
        else
        {
            move_request = controls.getMoveRequest();
        }
    }

    if (hurt_delay > 0)
    {
        hurt_delay--;
        switch(hurt_delay & 3)
        {
        case 0: render_data.color = sp::Color(1, 1, 1); break;
        case 1: render_data.color = sp::Color(1, 1, 0); break;
        case 2: render_data.color = sp::Color(1, 0, 1); break;
        case 3: render_data.color = sp::Color(0, 1, 1); break;
        }
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

void PlayerPawn::onTakeDamage(int amount, sp::P<Enemy> source)
{
    if (hurt_delay > 0)
        return;
    
    hurt_delay = 10;
    hurt_direction = (getPosition2D() - source->getPosition2D()).normalized();
    LOG(Debug, amount);
}
