#ifndef ENTRANCE_H
#define ENTRANCE_H

#include <sp2/scene/node.h>
#include <sp2/math/rect.h>

class Entrance : public sp::Node
{
public:
    Entrance(sp::P<sp::Node> parent, sp::Rect2d area, sp::string target, sp::Vector2d target_position);
    
    virtual void onCollision(sp::CollisionInfo& info) override;
    
    const sp::string& getMapName() { return target; }
    sp::Vector2d getTargetPosition() { return target_position; }
private:
    sp::string target;
    sp::Vector2d target_position;
};

#endif//ENTRANCE_H