#ifndef ENEMIES_BASIC_ENEMY_H
#define ENEMIES_BASIC_ENEMY_H

#include "enemy.h"
#include "../direction.h"

#include <sp2/script/environment.h>
#include <sp2/script/coroutine.h>


/* BasicEnemy
    This Enemy randomly wanders the map, and optionally fires projectiles.
    Quite a few enemies follow this basic pattern and thus can be made with this class:
    * Armos (no projectiles)
    * Ghini? (no projectiles, TODO: need to check behaviour really matches BasicEnemy, moves diagonaly)
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
        bool wall_collision;
        int hp;
        int hit_player_damage;

        std::map<sp::string, sp::string> properties;
    };

    BasicEnemy(sp::P<sp::Node> parent, const Template& enemy_template);

    virtual void onFixedUpdate() override;

    virtual bool onTakeDamage(int amount, sp::P<PlayerPawn> source) override;
    virtual void onCollision(sp::CollisionInfo& info) override;

    virtual void onRegisterScriptBindings(sp::ScriptBindingClass& bindings) override;

    void setPosition2D(sp::Vector2d position);
    void playAnimation(sp::string animation);
    void createProjectile(sp::Vector2d velocity, sp::string sprite_name);
private:
    sp::P<sp::script::Environment> script_environment;
    sp::script::CoroutinePtr update_coroutine;

    int hp;
    int hurt_counter;

    const Template& enemy_template;

public:
    static void loadEnemyTemplates();
    
    static std::map<sp::string, Template> templates;
};


#endif//ENEMIES_BASIC_ENEMY_H