#ifndef PLAYER_PAWN_H
#define PLAYER_PAWN_H

#include <sp2/scene/node.h>

#include "controls.h"
#include "direction.h"


class PlayerPawn : public sp::Node
{
public:
    PlayerPawn(sp::P<sp::Node> parent, Controls& controls);

    virtual void onFixedUpdate() override;
private:
    int shield_level;
    sp::P<sp::Node> active_item; //When we are using an inventory item, the UseItem is created in the world and will delete itself once the use is done.

    sp::Vector2d previous_position;
    Direction direction;
    Controls& controls;
};

#endif // PLAYER_PAWN_H