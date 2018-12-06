#include "playerPawn.h"

#include <sp2/graphics/spriteAnimation.h>


PlayerPawn::PlayerPawn(sp::P<sp::Node> parent, Controls& controls)
: sp::Node(parent), controls(controls)
{
    setAnimation(sp::SpriteAnimation::load("zelda1/sprites/link.txt"));
    animationPlay("STILL_D_1");
    setPosition(sp::Vector2d(8, 3.5));
}

void PlayerPawn::onFixedUpdate()
{
    sp::Vector2d move_request = controls.getMoveRequest();
    
    setPosition(getPosition2D() + move_request * 5.0 / 60.0);
    
    if (move_request.x < 0)
    {
        if (move_request.y < 0 && direction == Direction::Down)
            direction = Direction::Down;
        else if (move_request.y > 0 && direction == Direction::Up)
            direction = Direction::Up;
        else
            direction = Direction::Left;
    }
    else if (move_request.x > 0)
    {
        if (move_request.y < 0 && direction == Direction::Down)
            direction = Direction::Down;
        else if (move_request.y > 0 && direction == Direction::Up)
            direction = Direction::Up;
        else
            direction = Direction::Right;
    }
    else if (move_request.y < 0)
    {
        direction = Direction::Down;
    }
    else if (move_request.y > 0)
    {
        direction = Direction::Up;
    }

    sp::string animation_name = "IDLE";
    if (move_request.x || move_request.y)
        animation_name = "WALK";
    
    switch(direction)
    {
    case Direction::Left:
        animationPlay(animation_name + "_R_1");
        animationSetFlags(sp::SpriteAnimation::FlipFlag);
        break;
    case Direction::Right:
        animationPlay(animation_name + "_R_1");
        animationSetFlags(0);
        break;
    case Direction::Up:
        animationPlay(animation_name + "_U_1");
        animationSetFlags(0);
        break;
    case Direction::Down:
        animationPlay(animation_name + "_D_1");
        animationSetFlags(0);
        break;
    }
}
