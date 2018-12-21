#ifndef PLAYER_PAWN_H
#define PLAYER_PAWN_H

#include <sp2/scene/node.h>

#include "controls.h"
#include "direction.h"


class Enemy;
class Entrance;
class LightSource;
class PlayerInfo;
class Equipment;
class PlayerPawn : public sp::Node
{
public:
    PlayerPawn(sp::P<sp::Node> parent, PlayerInfo& player_info, Controls& controls);

    virtual void onFixedUpdate() override;
    
    virtual bool onTakeDamage(int amount, sp::Vector2d source);

    //Show the player holding equipment above it's head when it gains a new item.
    void showEquipmentPickup(const Equipment* equipment);
    
    void setWarpEntrance(sp::P<Entrance> entrance) { warp_target = entrance; }
    const sp::P<Entrance>& getWarpTarget() { return warp_target; }

    PlayerInfo& player_info;
private:
    int shield_level;
    sp::P<sp::Node> active_item; //When we are using an inventory item, the UseItem is created in the world and will delete itself once the use is done.
    sp::P<LightSource> light_source;

    sp::Vector2d previous_position;
    Direction direction;
    Controls& controls;
    
    int hurt_delay = 0;
    int invincibility_time = 0;
    sp::Vector2d hurt_direction;
    
    sp::P<sp::Node> equipment_pickup;
    int equipment_pickup_timeout = 0;

    sp::P<Entrance> warp_target;
};

#endif // PLAYER_PAWN_H
