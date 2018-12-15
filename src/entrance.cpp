#include "entrance.h"
#include "collisionBits.h"
#include "playerPawn.h"

#include <sp2/collision/simple2d/shape.h>


Entrance::Entrance(sp::P<sp::Node> parent, sp::Rect2d area, sp::string target, sp::Vector2d target_position)
: sp::Node(parent), target(target), target_position(target_position)
{
    setPosition(area.position + area.size * 0.5);

    sp::collision::Simple2DShape shape(area.size - sp::Vector2d(0.9, 0.9));
    shape.type = sp::collision::Shape::Type::Sensor;
    shape.setFilterCategory(CollisionCategory::other);
    shape.setMaskFilterCategory(CollisionCategory::other);
    shape.setMaskFilterCategory(CollisionCategory::level_edge);
    shape.setMaskFilterCategory(CollisionCategory::walls);
    shape.setMaskFilterCategory(CollisionCategory::enemy);
    shape.setMaskFilterCategory(CollisionCategory::enemy_projectile);
    setCollisionShape(shape);
}


void Entrance::onCollision(sp::CollisionInfo& info)
{
    sp::P<PlayerPawn> player = info.other;
    if (player)
        player->setWarpEntrance(this);
}
