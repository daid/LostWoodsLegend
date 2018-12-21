#ifndef EQUIPMENT_PICKUP_H
#define EQUIPMENT_PICKUP_H

#include <sp2/scene/node.h>

class Equipment;
class EquipmentPickup : public sp::Node
{
public:
    EquipmentPickup(sp::P<sp::Node> parent, sp::string equipment_id);

    virtual void onCollision(sp::CollisionInfo& info) override;
private:
    const Equipment* equipment;
};

#endif//EQUIPMENT_PICKUP_H