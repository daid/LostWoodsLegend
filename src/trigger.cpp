#include "trigger.h"
#include "enemies/enemy.h"

#include <sp2/scene/scene.h>

void Trigger::fire()
{
    for(sp::Node* n : getParent()->getChildren())
    {
        sp::P<Triggerable> t = sp::P<sp::Node>(n);
        if (t && t->getTriggerName() == trigger_target)
            t->onTrigger();
    }
}

void TriggerAllEnemiesDead::onFixedUpdate()
{
    for(sp::Node* n : getScene()->getRoot()->getChildren())
    {
        sp::P<Enemy> e = sp::P<sp::Node>(n);
        if (e)
            return;
    }
    fire();
    delete this;
}
