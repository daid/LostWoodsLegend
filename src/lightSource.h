#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <sp2/scene/node.h>


class LightSource : public sp::Node
{
public:
    LightSource(sp::P<sp::Node> parent, float radius)
    : sp::Node(parent), radius(radius) {}

    float radius;
};

#endif//LIGHTSOURCE_H