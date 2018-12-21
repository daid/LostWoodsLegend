#include "equipmentPickup.h"
#include "equipment.h"
#include "playerPawn.h"
#include "playerInfo.h"
#include "collisionBits.h"

#include <sp2/collision/simple2d/shape.h>


EquipmentPickup::EquipmentPickup(sp::P<sp::Node> parent, sp::string equipment_id)
: sp::Node(parent)
{
    equipment = findEquipment<Equipment>(equipment_id);

    sp::collision::Simple2DShape shape(sp::Vector2d(0.7, 0.7));
    shape.type = sp::collision::Shape::Type::Sensor;
    shape.setFilterCategory(CollisionCategory::other);
    shape.setMaskFilterCategory(CollisionCategory::other);
    shape.setMaskFilterCategory(CollisionCategory::level_edge);
    shape.setMaskFilterCategory(CollisionCategory::walls);
    shape.setMaskFilterCategory(CollisionCategory::enemy);
    shape.setMaskFilterCategory(CollisionCategory::enemy_projectile);
    setCollisionShape(shape);
}

void EquipmentPickup::onCollision(sp::CollisionInfo& info)
{
    sp::P<PlayerPawn> player = info.other;
    if (player)
    {
        if (player->player_info.addItem(equipment->id))
        {
            player->showEquipmentPickup(equipment);
            delete this;
        }
    }
}
