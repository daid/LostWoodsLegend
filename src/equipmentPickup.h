#ifndef EQUIPMENT_PICKUP_H
#define EQUIPMENT_PICKUP_H

#include "trigger.h"

class Equipment;
class EquipmentPickup : public Triggerable
{
public:
    EquipmentPickup(sp::P<sp::Node> parent, sp::string equipment_id);

    void hideTillEnemiesKilled();

    virtual void onCollision(sp::CollisionInfo& info) override;
    virtual void onFixedUpdate() override;
    virtual void onTrigger() override;

    void show();
    void hide();
private:
    bool show_on_enemies_killed = false;
    const Equipment* equipment;
};

#endif//EQUIPMENT_PICKUP_H