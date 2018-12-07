#ifndef DIRECTION_H
#define DIRECTION_H

#include <sp2/math/vector2.h>

class Direction
{
public:
    enum Value {
        Up, Down, Left, Right
    } value;
    
    operator Value() const
    {
        return value;
    }
    
    void operator=(const Value value)
    {
        this->value = value;
    }
    
    void update(sp::Vector2d movement)
    {
        if (movement.x < 0)
        {
            if (movement.y <= movement.x && value == Down)
                value = Down;
            else if (movement.y >= -movement.x && value == Up)
                value = Up;
            else
                value = Left;
        }
        else if (movement.x > 0)
        {
            if (movement.y <= -movement.x && value == Down)
                value = Down;
            else if (movement.y >= movement.x && value == Up)
                value = Up;
            else
                value = Right;
        }
        else if (movement.y < 0)
        {
            value = Down;
        }
        else if (movement.y > 0)
        {
            value = Up;
        }
    }
};

#endif//DIRECTION_H