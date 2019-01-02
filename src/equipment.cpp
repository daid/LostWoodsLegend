#include "equipment.h"
#include <sp2/io/keyValueTreeLoader.h>
#include <sp2/stringutil/convert.h>

#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/meshdata.h>


static std::map<sp::string, Equipment*> equipment_map;

void loadEquipmentList()
{
    auto kv_tree = sp::io::KeyValueTreeLoader::load("zelda1/equipment.txt");
    auto entries = kv_tree->getFlattenNodesByIds();

    for(auto& entry : entries)
    {
        sp::string type = entry.second["type"];
        Equipment* e = nullptr;
        if (type == "usable")
        {
            e = new UsableEquipment();
        }
        else if (type == "ammo")
        {
            e = new AmmoEquipment();
            (static_cast<AmmoEquipment*>(e))->default_capacity = sp::stringutil::convert::toInt(entry.second["capacity"]);
        }
        else if (type == "unique")
        {
            e = new UniqueEquipment();
            (static_cast<UniqueEquipment*>(e))->dungeon_specific = sp::stringutil::convert::toBool(entry.second["dungeon"]);
        }
        else if (type == "collectable")
        {
            e = new CollectableEquipment();
            (static_cast<CollectableEquipment*>(e))->dungeon_specific = sp::stringutil::convert::toBool(entry.second["dungeon"]);
        }
        else
        {
            LOG(Warning, "Unknown equipment type:", type);
        }

        if (e)
        {
            e->id = entry.first;
            e->name = entry.second["name"];
            e->sprite = entry.second["sprite"];
            e->sprite_uv = sp::Rect2d(0, 0, 1, 1);
            auto uv = sp::stringutil::convert::toFloatArray(entry.second["sprite_uv"]);
            if (uv.size() == 4)
                e->sprite_uv = sp::Rect2d(uv[0], uv[1], uv[2], uv[3]);
            equipment_map[e->id] = e;
        }
    }

    //We need to link in the ammo after we created all equipment items. Else we might not have the ammo object created yet.
    for(auto& entry : entries)
    {
        if (entry.second.find("ammo") != entry.second.end())
        {
            UsableEquipment* usable = dynamic_cast<UsableEquipment*>(equipment_map[entry.first]);
            if (usable)
            {
                for(sp::string ammo_id : entry.second["ammo"].split(","))
                {
                    const AmmoEquipment* ammo = findEquipment<AmmoEquipment>(ammo_id.strip());
                    if (ammo)
                    {
                        usable->ammo_types.push_back(ammo);
                    }
                }
            }
        }
    }
}

template<> const Equipment* findEquipment<Equipment>(sp::string id)
{
    auto it = equipment_map.find(id);
    if (it == equipment_map.end())
        return nullptr;
    return it->second;
}

void Equipment::setIcon(sp::P<sp::Node> node) const
{
    node->render_data.type = sp::RenderData::Type::Normal;
    node->render_data.texture = sp::texture_manager.get(sprite);
    node->render_data.mesh = sp::MeshData::createQuad(sp::Vector2f(1, 1), sp::Vector2f(sprite_uv.position), sp::Vector2f(sprite_uv.position + sprite_uv.size));
    node->render_data.shader = sp::Shader::get("object.shader");
}
