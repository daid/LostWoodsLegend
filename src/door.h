#ifndef DOOR_H
#define DOOR_H

#include <sp2/scene/node.h>

class CollectableEquipment;
class Door : public sp::Node
{
public:
    Door(sp::P<sp::Node> parent, sp::Vector2d position, sp::string sprite_name, sp::string animation_name);

    void setUseItem(const CollectableEquipment* equipment);

    virtual void onCollision(sp::CollisionInfo& info) override;
private:
    const CollectableEquipment* use_equipment = nullptr;
};

#endif//DOOR_H
