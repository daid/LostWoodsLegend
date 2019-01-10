#ifndef DOOR_H
#define DOOR_H

#include "trigger.h"

class CollectableEquipment;
class Door : public Triggerable
{
public:
    Door(sp::P<sp::Node> parent, sp::Vector2d position, sp::string sprite_name, sp::string animation_name);

    void setUseItem(const CollectableEquipment* equipment);

    virtual void onCollision(sp::CollisionInfo& info) override;
    virtual void onTrigger() override;
private:
    const CollectableEquipment* use_equipment = nullptr;
};

#endif//DOOR_H
