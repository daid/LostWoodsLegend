#include "simpleEffect.h"

#include <sp2/graphics/spriteAnimation.h>


SimpleEffect::SimpleEffect(sp::P<sp::Node> parent, sp::string animation_name)
: sp::Node(parent)
{
    setAnimation(sp::SpriteAnimation::load(animation_name));
    render_data.shader = sp::Shader::get("object.shader");
    animationPlay("MAIN");
}

void SimpleEffect::onFixedUpdate()
{
    if (animationIsFinished())
        delete this;
}
