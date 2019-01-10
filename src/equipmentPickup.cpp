#include "equipmentPickup.h"
#include "equipment.h"
#include "playerPawn.h"
#include "playerInfo.h"
#include "collisionBits.h"
#include "enemies/enemy.h"

#include <sp2/scene/scene.h>
#include <sp2/collision/simple2d/shape.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/meshdata.h>


EquipmentPickup::EquipmentPickup(sp::P<sp::Node> parent, sp::string equipment_id)
: Triggerable(parent)
{
    equipment = findEquipment<Equipment>(equipment_id);
    show();
}

void EquipmentPickup::hideTillEnemiesKilled()
{
    show_on_enemies_killed = true;
    hide();
}

void EquipmentPickup::show()
{
    if (!equipment)
        return;

    equipment->setIcon(this);

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

void EquipmentPickup::hide()
{
    render_data.type = sp::RenderData::Type::None;
    removeCollisionShape();
}

void EquipmentPickup::onFixedUpdate()
{
    if (show_on_enemies_killed)
    {
        for(sp::Node* n : getScene()->getRoot()->getChildren())
        {
            sp::P<Enemy> e = sp::P<sp::Node>(n);
            if (e)
                return;
        }
        show_on_enemies_killed = false;
        show();
    }
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

void EquipmentPickup::onTrigger()
{
    show();
}
