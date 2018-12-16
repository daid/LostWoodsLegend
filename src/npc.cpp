#include "npc.h"
#include "collisionBits.h"
#include "lightSource.h"


#include <sp2/collision/simple2d/shape.h>
#include <sp2/graphics/spriteAnimation.h>


NPC::NPC(sp::P<sp::Node> parent, sp::string sprite, std::map<sp::string, sp::string>& properties)
: sp::Node(parent)
{
    sp::collision::Simple2DShape shape(sp::Vector2d(0.8, 0.8));
    shape.type = sp::collision::Shape::Type::Kinematic;
    shape.setFilterCategory(CollisionCategory::other);
    setCollisionShape(shape);

    setAnimation(sp::SpriteAnimation::load(sprite + ".txt"));
    render_data.shader = sp::Shader::get("object.shader");
    animationPlay("DOWN");
    
    float light_range = sp::stringutil::convert::toFloat(properties["light_range"]);
    if (light_range > 0)
        new LightSource(this, light_range);
}