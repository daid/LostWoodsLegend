#ifndef NPC_H
#define NPC_H

#include <sp2/scene/node.h>

class NPC : public sp::Node
{
public:
    NPC(sp::P<sp::Node> parent, sp::string sprite, std::map<sp::string, sp::string>& properties);
};

#endif//NPC_H