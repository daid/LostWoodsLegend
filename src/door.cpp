#include "door.h"
#include "playerPawn.h"
#include "playerInfo.h"
#include "equipment.h"
#include "collisionBits.h"


#include <sp2/collision/simple2d/shape.h>
#include <sp2/graphics/spriteAnimation.h>


Door::Door(sp::P<sp::Node> parent, sp::string sprite_name, sp::string animation_name)
: sp::Node(parent)
{
    setAnimation(sp::SpriteAnimation::load(sprite_name + ".txt"));
    animationPlay(animation_name);

    sp::collision::Simple2DShape shape(sp::Rect2d(0, 0, 1, 1));
    shape.type = sp::collision::Shape::Type::Static;
    shape.setFilterCategory(CollisionCategory::walls);
    shape.setMaskFilterCategory(CollisionCategory::walls);
    setCollisionShape(shape);
}

void Door::setUseItem(const CollectableEquipment* equipment)
{
    use_equipment = equipment;
}

void Door::onCollision(sp::CollisionInfo& info)
{
    if (use_equipment)
    {
        sp::P<PlayerPawn> player = info.other;
        if (player)
        {
            if (player->player_info.takeCollectable(use_equipment))
            {
                delete this;
                return;
            }
        }
    }
}
