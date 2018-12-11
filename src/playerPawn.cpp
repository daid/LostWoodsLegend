#include "playerPawn.h"

#include <sp2/graphics/spriteAnimation.h>
#include <sp2/collision/simple2d/shape.h>

class UseSword : public sp::Node
{
public:
    UseSword(sp::P<sp::Node> source, Direction direction)
    : sp::Node(source->getParent())
    {
        setAnimation(sp::SpriteAnimation::load("zelda1/sprites/items.txt"));
        render_data.shader = sp::Shader::get("object.shader");
        animationPlay("SWORD_0");
        setPosition(source->getPosition2D() + direction.toVector());
        setRotation(direction.toAngle());

        sp::collision::Simple2DShape shape(sp::Vector2d(0.8, 0.5));
        shape.type = sp::collision::Shape::Type::Sensor;
        setCollisionShape(shape);
    }

    virtual void onFixedUpdate()
    {
        if (!timeout--)
            delete this;
    }
private:
    int timeout = 15;
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
    if (active_item)
    {
    }
    else
    {
        move_request = controls.getMoveRequest();
        if (controls.primary_action.getDown())
        {
            active_item = new UseSword(this, direction);
        }
    }

    setPosition(getPosition2D() + move_request * 5.0 / 60.0);
    direction.update(move_request);


    sp::string animation_name = "IDLE";
    if (move_request.x || move_request.y)
    {
        if (position != previous_position)
            animation_name = "WALK";
    }

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
