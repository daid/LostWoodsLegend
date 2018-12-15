#ifndef ENEMIES_BASIC_ENEMY_H
#define ENEMIES_BASIC_ENEMY_H

#include "enemy.h"
#include "../direction.h"

/* BasicEnemy
    This Enemy randomly wanders the map, and optionally fires projectiles.
    Quite a few enemies follow this basic pattern and thus can be made with this class:
    * Armos (no projectiles)
    * Ghini? (no projectiles, TODO: need to check behaviour really matches BasicEnemy)
    * Lynel (TODO: no delay after firing)
    * Moblin
    * Octorok
    * Darknut (no projectiles, TODO: cannot be hit from front)
    * Gibdo? (no projectiles, TODO: need to check behaviour really matches BasicEnemy)
    * Stalfos (no projectiles)

*/
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
        int fire_delay;
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

public:
    static void loadEnemyTemplates();
    
    static std::map<sp::string, Template> templates;
};


#endif//ENEMIES_BASIC_ENEMY_H