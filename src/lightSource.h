#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <sp2/scene/node.h>


class LightSource : public sp::Node
{
public:
    LightSource(sp::P<sp::Node> parent)
    : sp::Node(parent) {}

    float radius = 1.0;
};

#endif//LIGHTSOURCE_H