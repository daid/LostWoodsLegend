#ifndef ENEMY_H
#define ENEMY_H

#include <sp2/scene/node.h>

class PlayerPawn;
class Enemy : public sp::Node
{
public:
    Enemy(sp::P<sp::Node> parent);

    //Try to make this enemy take damage. Returns true if the damage was accepted.
    //Returns false if this damage was rejected. Note that this still might return true but do no actual damage.
    //This is when an enemy does absorb projectiles but accepts no damage from them.
    //If it returns false then projectiles are not absorbed and will pass trough this enemy.
    virtual bool onTakeDamage(int amount, sp::P<PlayerPawn> source) = 0;
};

#endif//ENEMY_H