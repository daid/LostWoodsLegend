#include <sp2/engine.h>
#include <sp2/window.h>
#include <sp2/logging.h>
#include <sp2/io/directoryResourceProvider.h>
#include <sp2/io/zipResourceProvider.h>
#include <sp2/graphics/gui/scene.h>
#include <sp2/graphics/gui/theme.h>
#include <sp2/graphics/gui/loader.h>
#include <sp2/graphics/scene/graphicslayer.h>
#include <sp2/graphics/scene/basicnoderenderpass.h>
#include <sp2/graphics/scene/collisionrenderpass.h>
#include <sp2/graphics/textureManager.h>
#include <sp2/graphics/spriteAnimation.h>
#include <sp2/graphics/meshdata.h>
#include <sp2/graphics/opengl.h>
#include <sp2/collision/simple2d/shape.h>
#include <sp2/scene/scene.h>
#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/updatable.h>

#include <json11/json11.hpp>

#include "playerInfo.h"
#include "playerPawn.h"
#include "lightSource.h"
#include "enemies/basicEnemy.h"
#include "darknessOverlayRenderer.h"
#include "entrance.h"
#include "mapScene.h"
#include "equipment.h"


sp::P<sp::Window> window;

Controls controls0{0};
Controls controls1{1};
sp::io::Keybinding escape_key{"exit", "Escape"};


class GameManager : public sp::Updatable
{
public:
    GameManager(sp::P<sp::SceneGraphicsLayer> scene_layer)
    {
        scene_render_pass = new sp::BasicNodeRenderPass();
        darkness_render_pass = new DarknessRenderPass();
        gui_render_pass = new sp::BasicNodeRenderPass();
        scene_layer->addRenderPass(scene_render_pass);
        scene_layer->addRenderPass(darkness_render_pass);
        scene_layer->addRenderPass(gui_render_pass);
#ifdef DEBUG
        scene_layer->addRenderPass(new sp::CollisionRenderPass());
#endif
        gui_scene = new sp::gui::Scene(sp::Vector2d(320, 240), sp::gui::Scene::Direction::Horizontal);
        scene = new MapScene("MAIN");
        scene_render_pass->addCamera(scene->getCamera());
        darkness_render_pass->addCamera(scene->getCamera());
        gui_render_pass->addCamera(gui_scene->getCamera());
        gui = sp::gui::Loader::load("gui/hud.gui", "HUD");

        //TODO: This is hardcoded data and should be loaded from some kind of world/game file.
        map_position = sp::Vector2i(7, 3);
        map_name = "zelda1/overworld";

        scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");

        player_info.addItem("SWORD");
        player = new PlayerPawn(scene->getRoot(), player_info, controls0);
    }

    virtual void onUpdate(float delta) override
    {
        if (player->getWarpTarget())
        {
            sp::P<Entrance> entrance = player->getWarpTarget();
            map_name = entrance->getMapName();
            map_position.x = -1;
            int idx = map_name.rfind("/");
            if (idx > 0)
            {
                sp::string part = map_name.substr(idx + 1);
                idx = part.find("-");
                if (idx > 0 && part[0] >= '0' && part[0] <= '9')
                {
                    map_name = map_name.substr(0, -part.length() - 1);
                    map_position.x = sp::stringutil::convert::toInt(part);
                    map_position.y = sp::stringutil::convert::toInt(part.substr(idx + 1));
                }
            }
            sp::Vector2d target_position = entrance->getTargetPosition();
            if (entrance->getDungeonName() != "")
            {
                player_info.current_dungeon = entrance->getDungeonName();
                if (player_info.current_dungeon == "NONE")
                    player_info.current_dungeon = "";
            }
            loadNextMap(MapScene::Transition::None, target_position);
        }
        else if (player->getPosition2D().x < scene->getMapData()->play_area.position.x && map_position.x != -1)
        {
            map_position.x -= 1;
            loadNextMap(MapScene::Transition::Left, player->getPosition2D() + sp::Vector2d(scene->getMapData()->play_area.size.x, 0));
        }
        else if (player->getPosition2D().x > scene->getMapData()->play_area.position.x + scene->getMapData()->play_area.size.x && map_position.x != -1)
        {
            map_position.x += 1;
            loadNextMap(MapScene::Transition::Right, player->getPosition2D() + sp::Vector2d(-scene->getMapData()->play_area.size.x, 0));
        }
        else if (player->getPosition2D().y < scene->getMapData()->play_area.position.y && map_position.x != -1)
        {
            map_position.y += 1;
            loadNextMap(MapScene::Transition::Down, player->getPosition2D() + sp::Vector2d(0, scene->getMapData()->play_area.size.y));
        }
        else if (player->getPosition2D().y > scene->getMapData()->play_area.position.y + scene->getMapData()->play_area.size.y && map_position.x != -1)
        {
            map_position.y -= 1;
            loadNextMap(MapScene::Transition::Up, player->getPosition2D() + sp::Vector2d(0, -scene->getMapData()->play_area.size.y));
        }
        updateGUI();
    }

private:
    void loadNextMap(MapScene::Transition transition, sp::Vector2d player_position)
    {
        scene->unloadMap(transition);
        player->setPosition(player_position);
        if (map_position.x == -1)
            scene->loadMap(map_name + ".json");
        else
            scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");
        darkness_render_pass->enabled = scene->getMapData()->darkness;
    }

    void updateGUI()
    {
        for(int n=0; n<PlayerInfo::active_item_count; n++)
        {
            sp::P<sp::gui::Widget> w = gui->getWidgetWithID("ITEM" + sp::string(n));
            if (!w)
                continue;
            sp::P<sp::gui::Widget> icon = w->getWidgetWithID("ICON");
            sp::P<sp::gui::Widget> label = w->getWidgetWithID("LABEL");
            if (!player_info.active_items[n].equipment)
            {
                icon->hide();
                label->hide();
            }
            else
            {
                icon->show();
                icon->setAttribute("image", player_info.active_items[n].equipment->sprite);
                icon->setAttribute("uv", sp::string(player_info.active_items[n].equipment->sprite_uv.position.x) + "," + sp::string(player_info.active_items[n].equipment->sprite_uv.position.y) + "," + sp::string(player_info.active_items[n].equipment->sprite_uv.size.x) + "," + sp::string(player_info.active_items[n].equipment->sprite_uv.size.y));
                if (player_info.active_items[n].ammo)
                    label->show();
                else
                    label->hide();
            }
        }
    }

    PlayerInfo player_info;
    sp::P<MapScene> scene;
    sp::P<sp::gui::Scene> gui_scene;
    sp::P<PlayerPawn> player;
    sp::P<sp::BasicNodeRenderPass> scene_render_pass;
    sp::P<DarknessRenderPass> darkness_render_pass;
    sp::P<sp::BasicNodeRenderPass> gui_render_pass;
    sp::P<sp::gui::Widget> gui;
    sp::Vector2i map_position;
    sp::string map_name;
};

int main(int argc, char** argv)
{
    sp::P<sp::Engine> engine = new sp::Engine();

    //Create resource providers, so we can load things.
    new sp::io::DirectoryResourceProvider("resources");
    
    //Disable or enable smooth filtering by default, enabling it gives nice smooth looks, but disabling it gives a more pixel art look.
    sp::texture_manager.setDefaultSmoothFiltering(false);
    
    BasicEnemy::loadEnemyTemplates();
    loadEquipmentList();

    //Create a window to render on, and our engine.
    window = new sp::Window(4.0/3.0);
#ifndef DEBUG
    window->setFullScreen(true);
    window->hideCursor();
#endif

    sp::gui::Theme::loadTheme("default", "gui/theme/basic.theme.txt");

    sp::P<sp::SceneGraphicsLayer> scene_layer = new sp::SceneGraphicsLayer(1);
    window->addLayer(scene_layer);

    new GameManager(scene_layer);

    engine->run();

    return 0;
}
