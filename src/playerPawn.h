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
    Direction direction;
    Controls& controls;
};

#endif // PLAYER_PAWN_H