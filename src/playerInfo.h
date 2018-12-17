#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

/**
    The PlayerInfo object contains information about the player that indicates progress
    Mostly this relates to inventory.
*/
class PlayerInfo
{
public:
    int money = 0;
    
    class Item
    {
    public:
        
        int use_count;
    };
};

#endif//PLAYER_INFO_H