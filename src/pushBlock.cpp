#include "pushBlock.h"
#include "collisionBits.h"
#include "playerPawn.h"

#include <sp2/scene/scene.h>
#include <sp2/graphics/spriteAnimation.h>
#include <sp2/collision/simple2d/shape.h>


PushBlock::PushBlock(sp::P<sp::Node> parent, sp::Vector2d position, sp::string sprite_file, sp::string animation_name)
: Triggerable(parent)
{
    setPosition(position);
    setAnimation(sp::SpriteAnimation::load(sprite_file + ".txt"));
    animationPlay(animation_name);

    sp::collision::Simple2DShape shape(sp::Rect2d(0, 0, 1, 1));
    shape.type = sp::collision::Shape::Type::Static;
    shape.setFilterCategory(CollisionCategory::walls);
    shape.setMaskFilterCategory(CollisionCategory::walls);
    setCollisionShape(shape);
}

void PushBlock::onTrigger()
{
}

void PushBlock::onCollision(sp::CollisionInfo& info)
{
    sp::P<PlayerPawn> player = info.other;
    if (!player)
        return;
    pushed = true;
    if (push_delay)
    {
        push_delay--;
    }
    else
    {
        sp::Vector2d position = getPosition2D() + info.normal * 0.01;
        bool blocked = false;
        getScene()->queryCollision(sp::Rect2d(position - sp::Vector2d(0.5, 0.5), sp::Vector2d(1, 1)), [this, &blocked](sp::P<sp::Node> node) -> bool
        {
            if (node != this && node->isSolid())
                blocked = true;
            return true;
        });

        if (!blocked)
            setPosition(position);
    }
}

void PushBlock::onFixedUpdate()
{
    if (!pushed)
        push_delay = 10;
    pushed = false;
}
