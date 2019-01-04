#include "door.h"
#include "playerPawn.h"
#include "playerInfo.h"
#include "equipment.h"
#include "collisionBits.h"


#include <sp2/collision/simple2d/shape.h>
#include <sp2/graphics/spriteAnimation.h>


Door::Door(sp::P<sp::Node> parent, sp::Vector2d position, sp::string sprite_name, sp::string animation_name)
: sp::Node(parent)
{
    setPosition(position);
    setAnimation(sp::SpriteAnimation::load(sprite_name + ".txt"));
    animationPlay(animation_name);

    sp::collision::Simple2DShape shape(sp::Rect2d(0, 0, 1, 1));
    shape.type = sp::collision::Shape::Type::Static;
    shape.setFilterCategory(CollisionCategory::walls);
    shape.setMaskFilterCategory(CollisionCategory::walls);
    setCollisionShape(shape);

    for(sp::Node* node : parent->getChildren())
    {
        sp::P<PlayerPawn> player = sp::P<sp::Node>(node);
        if (player)
        {
            sp::Vector2d pposition = player->getPosition2D();
            sp::Vector2d diff = position - pposition;
            if (std::max(std::fabs(diff.x), std::fabs(diff.y)) < 0.6)
            {
                //Player is inside the door, move the player more towards the center of the map
                //so the door does not push the player outside of the map again.
                if (std::fabs(diff.x) < std::fabs(diff.y))
                {
                    if (diff.y < 0)
                        player->setPosition(player->getPosition2D() + sp::Vector2d(0, -1));
                    else
                        player->setPosition(player->getPosition2D() + sp::Vector2d(0, 1));
                }
                else
                {
                    if (diff.x < 0)
                        player->setPosition(player->getPosition2D() + sp::Vector2d(-1, 0));
                    else
                        player->setPosition(player->getPosition2D() + sp::Vector2d(1, 0));
                }
            }
        }
    }
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
