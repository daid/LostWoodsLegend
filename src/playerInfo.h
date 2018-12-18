#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include "equipment.h"


/**
    The PlayerInfo object contains information about the player that indicates progress
    Mostly this relates to inventory.

    The player has:
    * money: very specific resource. Called Rupee in Zelda.
    * usable items: sword, wand, bow, bomb, etc...
    * constant upgrades that cannot be used: armor (damage reduction), flippers, 
    * one-off unusable items: trade items, dungeon open keys. Used in specific events.
    * unusable items with a count: collectables like secret sea shells. Used in specific events.
    * dungeon specific items: keys, boss/big-key, map, compass
    * ammo/usage storage for bombs/arrows

    Most complex is the usable items. Some usable items have a level (swords, shields) some have contents (bottle) and some have ammo.
    Ammo could potentially be used by multiple items (seeds in oracle games), and one usable item can have multiple ammo types (usually the bow)
    Ammo has a capacity that is player specific, as you can get capacity upgraded
    Also, you can have multiple of the same item with contents (bottles)
*/
class PlayerInfo
{
public:
    int money = 0;
    
    class Ammo
    {
    public:
        AmmoEquipment* equipment = nullptr;
        int amount = 0;
        int capacity = 0;
    };
    class Usable
    {
    public:
        UsableEquipment* equipment = nullptr;
        UsableEquipment* selected_ammo = nullptr;
    };
    class Collectable
    {
        CollectableEquipment* equipment = nullptr;
        int amount = 0;
    };

    Usable left_item;
    Usable right_item;
    std::vector<Usable> equipment;
    std::vector<Ammo> ammo;

    std::vector<UniqueEquipment*> uniques;
    std::vector<Collectable> collectables;

    //TODO: Dungeon specific items.
};

#endif//PLAYER_INFO_H
