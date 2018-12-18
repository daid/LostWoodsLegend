#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <sp2/string.h>


class Equipment
{
public:
    sp::string id;
    sp::string name;
    sp::string sprite;
};

class AmmoEquipment;
class UsableEquipment : public Equipment
{
public:
    std::vector<AmmoEquipment*> ammo_types;
};

class AmmoEquipment : public Equipment
{
public:
    int default_capacity;
};

class UniqueEquipment : public Equipment
{
public:
    bool dungeon_specific;
};

class CollectableEquipment : public Equipment
{
public:
    bool dungeon_specific;
};

void loadEquipmentList();
Equipment* findEquipment(sp::string id);

#endif//EQUIPMENT_H
