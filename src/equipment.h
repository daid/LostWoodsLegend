#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <sp2/string.h>
#include <sp2/math/rect.h>


class Equipment
{
public:
    sp::string id;
    sp::string name;
    sp::string sprite;
    sp::Rect2d sprite_uv;
    
    virtual~Equipment() {}
};

class AmmoEquipment;
class UsableEquipment : public Equipment
{
public:
    std::vector<const AmmoEquipment*> ammo_types;
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
template<typename T> const T* findEquipment(sp::string id) { return dynamic_cast<const T*>(findEquipment<Equipment>(id)); }
template<> const Equipment* findEquipment<Equipment>(sp::string id);

#endif//EQUIPMENT_H
