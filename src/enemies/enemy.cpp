#include "enemy.h"
#include "../equipment.h"
#include "../equipmentPickup.h"

#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/meshdata.h>


Enemy::Enemy(sp::P<sp::Node> parent)
: sp::Node(parent)
{
}


void Enemy::setCarry(const Equipment* equipment)
{
    equipment_carry = equipment;

    if (!equipment)
        return;
    
    sp::P<sp::Node> node = new sp::Node(this);
    node->render_data.type = sp::RenderData::Type::Normal;
    node->render_data.texture = sp::texture_manager.get(equipment->sprite);
    node->render_data.mesh = sp::MeshData::createQuad(sp::Vector2f(1, 1), sp::Vector2f(equipment->sprite_uv.position), sp::Vector2f(equipment->sprite_uv.position + equipment->sprite_uv.size));
    node->render_data.shader = sp::Shader::get("object.shader");
    node->setPosition(sp::Vector3d(0, 0, -0.01));
}

void Enemy::die()
{
    if (equipment_carry)
    {
        EquipmentPickup* ep = new EquipmentPickup(getParent(), equipment_carry->id);
        ep->setPosition(getPosition2D());
    }
    delete this;
}