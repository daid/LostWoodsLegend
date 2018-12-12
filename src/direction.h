#ifndef DIRECTION_H
#define DIRECTION_H

#include <sp2/math/vector2.h>
#include <sp2/random.h>


class Direction
{
public:
    enum Value {
        Up, Down, Left, Right
    } value = Down;
    
    operator Value() const
    {
        return value;
    }
    
    void operator=(const Value value)
    {
        this->value = value;
    }

    sp::Vector2d toVector()
    {
        switch (value)
        {
        case Value::Up:
            return sp::Vector2d(0, 1);
        case Value::Down:
            return sp::Vector2d(0, -1);
        case Value::Left:
            return sp::Vector2d(-1, 0);
        case Value::Right:
            return sp::Vector2d(1, 0);
        }
        return sp::Vector2d(0, 0);
    }

    double toAngle()
    {
        switch (value)
        {
        case Value::Up:
            return 0;
        case Value::Down:
            return 180;
        case Value::Left:
            return 90;
        case Value::Right:
            return 270;
        }
        return 0;
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
    
    static Value random()
    {
        return (Value)sp::irandom(0, 3);
    }
};

#endif//DIRECTION_H