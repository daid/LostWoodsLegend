#ifndef SIMPLE_EFFECT_H
#define SIMPLE_EFFECT_H

#include <sp2/scene/node.h>

class SimpleEffect : public sp::Node
{
public:
    SimpleEffect(sp::P<sp::Node> parent, sp::string animation_name);

    virtual void onFixedUpdate() override;
};

#endif//SIMPLE_EFFECT_H
