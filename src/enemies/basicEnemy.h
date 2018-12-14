#ifndef ENEMIES_BASIC_ENEMY_H
#define ENEMIES_BASIC_ENEMY_H

#include "enemy.h"
#include "../direction.h"


class BasicEnemy : public Enemy
{
public:
    class Template
    {
    public:
        sp::string sprite;
        sp::Rect2d collision_rect;
        int hp;
        int hit_player_damage;
        double walk_speed;

        sp::string projectile_sprite;
        sp::Rect2d projectile_collision_rect;
        double projectile_speed;
    };

    BasicEnemy(sp::P<sp::Node> parent, const Template& enemy_template);

    virtual void onFixedUpdate() override;

    virtual bool onTakeDamage(int amount, sp::P<PlayerPawn> source) override;
    virtual void onCollision(sp::CollisionInfo& info) override;
private:
    enum class State
    {
        Walk,
        Attack
    };

    int hp;
    int hurt_counter;

    State state;
    int state_delay;
    Direction walk_direction;
    int walk_distance;

    const Template& enemy_template;
};


#endif//ENEMIES_BASIC_ENEMY_H