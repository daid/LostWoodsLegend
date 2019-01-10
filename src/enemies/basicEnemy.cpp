#include "basicEnemy.h"
#include "../playerPawn.h"
#include "../collisionBits.h"
#include "../simpleEffect.h"

#include <sp2/collision/simple2d/shape.h>
#include <sp2/graphics/spriteAnimation.h>
#include <sp2/io/keyValueTreeLoader.h>


std::map<sp::string, BasicEnemy::Template> BasicEnemy::templates;

static int luaF_yield(lua_State* L)
{
    return lua_yield(L, 0);
}

class BasicEnemyProjectile : public sp::Node
{
public:
    BasicEnemyProjectile(sp::P<Enemy> owner, sp::string animation, sp::Vector2d velocity)
    : sp::Node(owner->getParent()), velocity(velocity)
    {
        setPosition(owner->getPosition2D() + velocity.normalized() * 0.5);
        setAnimation(sp::SpriteAnimation::load(animation));
        render_data.shader = sp::Shader::get("object.shader");
        
        if (velocity.x < 0)
            animationPlay("LEFT");
        else if (velocity.x > 0)
            animationPlay("RIGHT");
        else if (velocity.y > 0)
            animationPlay("UP");
        else
            animationPlay("DOWN");

        sp::collision::Simple2DShape shape(sp::Rect2d(0, 0, 0.3, 0.3));
        shape.type = sp::collision::Shape::Type::Sensor;
        shape.setFilterCategory(CollisionCategory::enemy_projectile);
        shape.setMaskFilterCategory(CollisionCategory::enemy);
        shape.setMaskFilterCategory(CollisionCategory::enemy_projectile);
        setCollisionShape(shape);

        timeout = 60 * 10;
    }

    virtual void onFixedUpdate() override
    {
        setPosition(getPosition2D() + velocity);
        if (timeout)
            timeout--;
        else
            delete this;
    }

    virtual void onCollision(sp::CollisionInfo& info)
    {
        if (info.other && info.other->isSolid())
        {
            delete this;
            return;
        }
        sp::P<PlayerPawn> player = info.other;
        if (player && player->onTakeDamage(8, this->getPosition2D()))
        {
            delete this;
            return;
        }
    }
private:
    sp::Vector2d velocity;
    int timeout;
};


BasicEnemy::BasicEnemy(sp::P<sp::Node> parent, const Template& enemy_template)
: Enemy(parent), enemy_template(enemy_template)
{
    setAnimation(sp::SpriteAnimation::load(enemy_template.sprite));
    render_data.shader = sp::Shader::get("object.shader");
    animationPlay("DOWN");

    sp::collision::Simple2DShape shape(enemy_template.collision_rect);
    shape.type = sp::collision::Shape::Type::Dynamic;
    shape.setFilterCategory(CollisionCategory::enemy);
    shape.setMaskFilterCategory(CollisionCategory::enemy);
    shape.setMaskFilterCategory(CollisionCategory::enemy_projectile);
    if (!enemy_template.wall_collision)
        shape.setMaskFilterCategory(CollisionCategory::walls);
    setCollisionShape(shape);

    hp = enemy_template.hp;
    hurt_counter = 0;

    script_environment = new sp::script::Environment();
    script_environment->setGlobal("this", sp::P<BasicEnemy>(this));
    script_environment->setGlobal("yield", luaF_yield);
    script_environment->setGlobal("random", sp::random);
    script_environment->setGlobal("irandom", sp::irandom);
    for (auto& it : enemy_template.properties)
        script_environment->setGlobal(it.first, it.second);
    script_environment->load("script/basic_enemy.lua");
}

void BasicEnemy::onFixedUpdate()
{
    if (!update_coroutine || !update_coroutine->resume())
        update_coroutine = script_environment->callCoroutine("update");
    script_environment->call("updateAnimation");
    script_environment->setGlobal("touch_wall", false);

    if (hurt_counter > 0)
    {
        hurt_counter--;
        switch(hurt_counter & 3)
        {
        case 0: render_data.color = sp::Color(1, 1, 1); break;
        case 1: render_data.color = sp::Color(1, 1, 0); break;
        case 2: render_data.color = sp::Color(1, 0, 1); break;
        case 3: render_data.color = sp::Color(0, 1, 1); break;
        }
    }
}

bool BasicEnemy::onTakeDamage(int amount, sp::P<PlayerPawn> source)
{
    if (hurt_counter > 0)
        return false;

    hp -= amount;
    hurt_counter = 35;
    if (hp <= 0)
    {
        (new SimpleEffect(getParent(), "zelda1/sprites/death.txt"))->setPosition(getPosition2D());
        die();
    }
    return true;
}

void BasicEnemy::onCollision(sp::CollisionInfo& info)
{
    if (info.other->isSolid())
        script_environment->setGlobal("touch_wall", true);

    sp::P<PlayerPawn> player = info.other;
    if (player)
        player->onTakeDamage(enemy_template.hit_player_damage, this->getPosition2D());
}

void BasicEnemy::loadEnemyTemplates()
{
    for(auto& entry : sp::io::KeyValueTreeLoader::load("zelda1/enemies.txt")->getFlattenNodesByIds())
    {
        Template t;
        t.sprite = entry.second["sprite"];
        t.collision_rect.size = sp::stringutil::convert::toVector2d(entry.second["collision"]);
        t.hp = sp::stringutil::convert::toInt(entry.second["hp"]);
        t.hit_player_damage = sp::stringutil::convert::toInt(entry.second["hit_player_damage"]);
        t.wall_collision = !sp::stringutil::convert::toBool(entry.second["walk_trough_walls"]);;

        t.properties = entry.second;
        templates[entry.first] = t;
    }
}

void BasicEnemy::onRegisterScriptBindings(sp::ScriptBindingClass& bindings)
{
    bindings.bind("setPosition", &BasicEnemy::setPosition2D);
    bindings.bind("getPosition", &sp::Node::getPosition2D);
    bindings.bind("animation", &BasicEnemy::playAnimation);
    bindings.bind("createProjectile", &BasicEnemy::createProjectile);
}

void BasicEnemy::setPosition2D(sp::Vector2d position)
{
    setPosition(position);
}

void BasicEnemy::playAnimation(sp::string animation)
{
    animationPlay(animation);
}

void BasicEnemy::createProjectile(sp::Vector2d velocity, sp::string sprite_name)
{
    new BasicEnemyProjectile(this, sprite_name, velocity);
}
