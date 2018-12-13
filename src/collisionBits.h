#ifndef COLLISION_BITS_H
#define COLLISION_BITS_H

//This file defines the different filter masks for different types of objects in collision handling.
namespace CollisionCategory
{
    static constexpr int other = 1;
    static constexpr int player = 2;
    static constexpr int enemy = 3;

    static constexpr int walls = 8;
    static constexpr int level_edge = 9;
};

#endif//COLLISION_BITS_H
