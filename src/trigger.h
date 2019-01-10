#ifndef TRIGGER_H
#define TRIGGER_H

#include <sp2/scene/node.h>


class Triggerable : public sp::Node
{
public:
    Triggerable(sp::P<sp::Node> parent) : sp::Node(parent) {}

    const sp::string& getTriggerName() { return trigger_name; }
    void setTriggerName(sp::string trigger_name) { this->trigger_name = trigger_name; }

    virtual void onTrigger() = 0;
private:
    sp::string trigger_name;
};

class Trigger : public sp::Node
{
public:
    Trigger(sp::P<sp::Node> parent, sp::string target)
    : sp::Node(parent), trigger_target(target) {}

protected:
    void fire();
private:
    sp::string trigger_target;
};

class TriggerAllEnemiesDead : public Trigger
{
public:
    TriggerAllEnemiesDead(sp::P<sp::Node> parent, sp::string target)
    : Trigger(parent, target) {}
    
    virtual void onFixedUpdate() override;
};

#endif//TRIGGER_H
