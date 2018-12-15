#include "basicEnemy.h"
#include "../playerPawn.h"
#include "../collisionBits.h"
#include "../simpleEffect.h"

#include <sp2/collision/simple2d/shape.h>
#include <sp2/graphics/spriteAnimation.h>
#include <sp2/io/keyValueTreeLoader.h>

std::map<sp::string, BasicEnemy::Template> BasicEnemy::templates;


class BasicEnemyProjectile : public sp::Node
{
public:
    BasicEnemyProjectile(sp::P<Enemy> owner, Direction direction, const BasicEnemy::Template& enemy_template)
    : sp::Node(owner->getParent()), direction(direction), enemy_template(enemy_template)
    {
        setPosition(owner->getPosition2D() + direction.toVector() * 0.5);
        setAnimation(sp::SpriteAnimation::load(enemy_template.projectile_sprite));
        render_data.shader = sp::Shader::get("object.shader");
        switch(direction)
        {
        case Direction::Up: animationPlay("UP"); break;
        case Direction::Down: animationPlay("DOWN"); break;
        case Direction::Left: animationPlay("LEFT"); break;
        case Direction::Right: animationPlay("RIGHT"); break;
        }

        sp::collision::Simple2DShape shape(enemy_template.projectile_collision_rect);
        shape.type = sp::collision::Shape::Type::Sensor;
        shape.setFilterCategory(CollisionCategory::enemy_projectile);
        shape.setMaskFilterCategory(CollisionCategory::enemy);
        shape.setMaskFilterCategory(CollisionCategory::enemy_projectile);
        setCollisionShape(shape);

        timeout = 60 * 10;
    }

    virtual void onFixedUpdate() override
    {
        setPosition(getPosition2D() + direction.toVector() * enemy_template.projectile_speed / 60.0);
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
        if (player && player->onTakeDamage(5, this->getPosition2D()))
        {
            delete this;
            return;
        }
    }
private:
    Direction direction;
    const BasicEnemy::Template& enemy_template;
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
    setCollisionShape(shape);

    hp = enemy_template.hp;
    state = State::Walk;
    state_delay = sp::irandom(50, 100);
    walk_direction = Direction::random();
    hurt_counter = 0;
}

void BasicEnemy::onFixedUpdate()
{
    if (state_delay > 0)
        state_delay--;
    else
        state_delay = 0;

    switch(state)
    {
    case State::Walk:
        setPosition(getPosition2D() + walk_direction.toVector() * enemy_template.walk_speed / 60.0);
        if (!state_delay)
        {
            if (enemy_template.projectile_speed > 0 && sp::random(0, 100) < 50)
            {
                new BasicEnemyProjectile(this, walk_direction, enemy_template);
                state = State::Attack;
                state_delay = enemy_template.fire_delay;
            }
            else
            {
                walk_direction = Direction::random();
                state_delay = sp::irandom(50, 100);
            }
        }
        break;
    case State::Attack:
        if (!state_delay)
        {
            walk_direction = Direction::random();
            state = State::Walk;
            state_delay = sp::irandom(50, 100);
        }
        break;
    }
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

    switch(walk_direction)
    {
    case Direction::Left:
        animationPlay("RIGHT");
        animationSetFlags(sp::SpriteAnimation::FlipFlag);
        break;
    case Direction::Right:
        animationPlay("RIGHT");
        animationSetFlags(0);
        break;
    case Direction::Up:
        animationPlay("UP");
        animationSetFlags(0);
        break;
    case Direction::Down:
        animationPlay("DOWN");
        animationSetFlags(0);
        break;
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
        delete this;
    }
    return true;
}

void BasicEnemy::onCollision(sp::CollisionInfo& info)
{
    if (info.other->isSolid() && state == State::Walk)
        state_delay -= 10;

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
        t.walk_speed = sp::stringutil::convert::toFloat(entry.second["walk_speed"]);

        t.projectile_sprite = entry.second["projectile_sprite"];
        t.projectile_collision_rect.size = sp::stringutil::convert::toVector2d(entry.second["projectile_collision"]);
        t.projectile_speed = sp::stringutil::convert::toFloat(entry.second["projectile_speed"]);
        t.fire_delay = sp::stringutil::convert::toInt(entry.second["fire_delay"]);
        templates[entry.first] = t;
    }
}
