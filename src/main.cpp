#include <sp2/engine.h>
#include <sp2/window.h>
#include <sp2/logging.h>
#include <sp2/io/directoryResourceProvider.h>
#include <sp2/graphics/gui/scene.h>
#include <sp2/graphics/gui/theme.h>
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

#include "playerPawn.h"
#include "lightSource.h"
#include "enemies/basicEnemy.h"
#include "darknessOverlayRenderer.h"
#include "entrance.h"
#include "mapScene.h"


sp::P<sp::Window> window;

Controls controls0{0};
Controls controls1{1};
sp::io::Keybinding escape_key{"exit", "Escape"};


class GameManager : public sp::Updatable
{
public:
    GameManager()
    {
        scene = new MapScene("MAIN");

        map_position = sp::Vector2i(7, 7);
        map_name = "zelda1/overworld";

        scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");

        player = new PlayerPawn(scene->getRoot(), controls0);
        (new LightSource(player))->radius = 3.5;
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
            scene->unloadMap(MapScene::Transition::None);
            player->setPosition(target_position);
            if (map_position.x == -1)
                scene->loadMap(map_name + ".json");
            else
                scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");
        }
        else if (player->getPosition2D().x < 0 && map_position.x != -1)
        {
            map_position.x -= 1;
            scene->unloadMap(MapScene::Transition::Left);
            player->setPosition(player->getPosition2D() + sp::Vector2d(scene->getMapData()->size.x, 0));
            scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");
        }
        else if (player->getPosition2D().x > scene->getMapData()->size.x && map_position.x != -1)
        {
            map_position.x += 1;
            scene->unloadMap(MapScene::Transition::Right);
            player->setPosition(player->getPosition2D() + sp::Vector2d(-scene->getMapData()->size.x, 0));
            scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");
        }
        else if (player->getPosition2D().y < 0 && map_position.x != -1)
        {
            map_position.y += 1;
            scene->unloadMap(MapScene::Transition::Down);
            player->setPosition(player->getPosition2D() + sp::Vector2d(0, scene->getMapData()->size.y));
            scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");
        }
        else if (player->getPosition2D().y > scene->getMapData()->size.y && map_position.x != -1)
        {
            map_position.y -= 1;
            scene->unloadMap(MapScene::Transition::Up);
            player->setPosition(player->getPosition2D() + sp::Vector2d(0, -scene->getMapData()->size.y));
            scene->loadMap(map_name + "/" + sp::string(map_position.x) + "-" + sp::string(map_position.y) + ".json");
        }
    }

private:
    sp::P<MapScene> scene;
    sp::P<PlayerPawn> player;
    sp::P<DarknessRenderPass> darkness_render_pass;
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

    //Create a window to render on, and our engine.
    window = new sp::Window(4.0/3.0);
#ifndef DEBUG
    window->setFullScreen(true);
    window->hideCursor();
#endif

    sp::gui::Theme::loadTheme("default", "gui/theme/basic.theme.txt");
    new sp::gui::Scene(sp::Vector2d(640, 480), sp::gui::Scene::Direction::Horizontal);

    sp::P<sp::SceneGraphicsLayer> scene_layer = new sp::SceneGraphicsLayer(1);
    scene_layer->addRenderPass(new sp::BasicNodeRenderPass());
    scene_layer->addRenderPass(new DarknessRenderPass());
#ifdef DEBUG
    scene_layer->addRenderPass(new sp::CollisionRenderPass());
#endif
    window->addLayer(scene_layer);

    new GameManager();

    engine->run();

    return 0;
}
