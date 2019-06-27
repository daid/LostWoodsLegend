#ifndef PUSH_BLOCK_H
#define PUSH_BLOCK_H

#include "trigger.h"


class PushBlock : public Triggerable
{
public:
    PushBlock(sp::P<sp::Node> parent, sp::Vector2d position, sp::string sprite_file, sp::string animation_name);

    virtual void onTrigger() override;
    virtual void onCollision(sp::CollisionInfo& info) override;
    virtual void onFixedUpdate() override;
private:
    int push_delay = 10;
    bool pushed = false;
};

#endif//PUSH_BLOCK_H
