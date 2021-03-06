#ifndef COLLISION_BITS_H
#define COLLISION_BITS_H

//This file defines the different filter masks for different types of objects in collision handling.
namespace CollisionCategory
{
    static constexpr int other = 1;
    static constexpr int player = 2;
    static constexpr int enemy = 3;
    static constexpr int player_projectile = 4;
    static constexpr int enemy_projectile = 5;

    static constexpr int walls = 8;
    static constexpr int water = 9;
    static constexpr int level_edge = 10;
};

#endif//COLLISION_BITS_H
