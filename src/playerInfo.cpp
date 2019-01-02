#include "playerInfo.h"


bool PlayerInfo::addItem(sp::string id)
{
    const UsableEquipment* usable = findEquipment<UsableEquipment>(id);
    if (usable)
    {
        const AmmoEquipment* ammo = nullptr;
        for (auto ammo_type : usable->ammo_types)
        {
            if (hasAmmo(ammo_type))
            {
                ammo = ammo_type;
                break;
            }
        }

        //Check if we already have this item.
        for (int n = 0; n < active_item_count; n++)
            if (active_items[n].equipment == usable)
                return false;
        for(auto& entry : equipment)
            if (entry.equipment == usable)
                return false;

        //First try to add the item to an empty active slot, so we can directly use it.
        for(int n=0; n<active_item_count; n++)
        {
            if (active_items[n].equipment == nullptr)
            {
                active_items[n].equipment = usable;
                active_items[n].ammo = ammo;
                return true;
            }
        }

        //Else store it in the equipment list.
        for(auto& entry : equipment)
        {
            if (!entry.equipment)
            {
                entry.equipment = usable;
                entry.ammo = ammo;
                return true;
            }
        }
        equipment.emplace_back();
        equipment.back().equipment = usable;
        equipment.back().ammo = ammo;
        return true;
    }
    const AmmoEquipment* ammo = findEquipment<AmmoEquipment>(id);
    if (ammo)
    {
        if (hasAmmo(ammo))
            return false;
        this->ammo.emplace_back();
        this->ammo.back().equipment = ammo;
        this->ammo.back().capacity = ammo->default_capacity;
        this->ammo.back().amount = ammo->default_capacity;
        
        //Check if we can assign this ammo to any usable item that has no ammo assigned to it.
        for(int n=0; n<active_item_count; n++)
        {
            if (active_items[n].equipment && active_items[n].equipment->ammo_types.size() > 0 && !active_items[n].ammo)
            {
                for (auto a : active_items[n].equipment->ammo_types)
                    if (a == ammo)
                        active_items[n].ammo = ammo;
            }
        }
        for(auto& entry : equipment)
        {
            if (entry.equipment && entry.equipment->ammo_types.size() > 0 && !entry.ammo)
            {
                for (auto a : entry.equipment->ammo_types)
                    if (a == ammo)
                        entry.ammo = ammo;
            }
        }
        return true;
    }
    const UniqueEquipment* unique = findEquipment<UniqueEquipment>(id);
    if (unique)
    {
        if (hasUnique(unique))
            return false;
        if (unique->dungeon_specific)
        {
            if (current_dungeon == "")
                return false;
            dungeon_uniques[current_dungeon].push_back(unique);
        }
        else
        {
            uniques.push_back(unique);
        }
        return true;
    }
    const CollectableEquipment* collectable = findEquipment<CollectableEquipment>(id);
    if (collectable)
    {
        if (collectable->dungeon_specific)
        {
            if (current_dungeon == "")
                return false;
            for(auto& c : dungeon_collectables[current_dungeon])
            {
                if (c.equipment == collectable)
                {
                    c.amount += 1;
                    return true;
                }
            }

            dungeon_collectables[current_dungeon].emplace_back();
            dungeon_collectables[current_dungeon].back().equipment = collectable;
            dungeon_collectables[current_dungeon].back().amount = 1;
        }
        else
        {
            for(auto& c : collectables)
            {
                if (c.equipment == collectable)
                {
                    c.amount += 1;
                    return true;
                }
            }

            collectables.emplace_back();
            collectables.back().equipment = collectable;
            collectables.back().amount = 1;
        }
        return true;
    }
    return false;
}

int PlayerInfo::addAmmo(sp::string id, int amount)
{
    const AmmoEquipment* ammo = findEquipment<AmmoEquipment>(id);
    if (!ammo)
        return 0;
    for (auto a : this->ammo)
    {
        if (a.equipment == ammo)
        {
            int add = std::min(amount, a.capacity - a.amount);
            a.amount += add;
            return add;
        }
    }
    return 0;
}

bool PlayerInfo::hasAmmo(const AmmoEquipment* ammo)
{
    for (auto a : this->ammo)
        if (a.equipment == ammo)
            return true;
    return false;
}

bool PlayerInfo::hasUnique(const UniqueEquipment* unique)
{
    if (unique->dungeon_specific)
    {
        if (current_dungeon == "")
            return false;
        for (auto u : dungeon_uniques[current_dungeon])
            if (u == unique)
                return true;
        return false;
    }
    for (auto u : uniques)
        if (u == unique)
            return true;
    return false;
}
