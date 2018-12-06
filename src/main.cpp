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
#include <sp2/collision/2d/circle.h>
#include <sp2/scene/scene.h>
#include <sp2/scene/node.h>
#include <sp2/scene/camera.h>
#include <sp2/scene/tilemap.h>
#include <sp2/io/keybinding.h>

#include <json11/json11.hpp>

#include "playerPawn.h"

sp::P<sp::Window> window;

Controls controls0{0};
sp::io::Keybinding escape_key{"exit", "Escape"};


int main(int argc, char** argv)
{
    sp::P<sp::Engine> engine = new sp::Engine();

    //Create resource providers, so we can load things.
    new sp::io::DirectoryResourceProvider("resources");

    //Disable or enable smooth filtering by default, enabling it gives nice smooth looks, but disabling it gives a more pixel art look.
    sp::texture_manager.setDefaultSmoothFiltering(false);

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
#ifdef DEBUG
    scene_layer->addRenderPass(new sp::CollisionRenderPass());
#endif
    window->addLayer(scene_layer);

    //TODO: Create your own scene(s) here and populate them with nodes.
    sp::P<sp::Scene> scene = new sp::Scene("MAIN", 0);
    sp::P<sp::Node> node = new PlayerPawn(scene->getRoot(), controls0);

    sp::P<sp::Camera> camera = new sp::Camera(scene->getRoot());
    camera->setOrtographic(8.0);
    camera->setPosition(sp::Vector2d(8, 4));
    scene->setDefaultCamera(camera);
    
    {
        sp::P<sp::Tilemap> tilemap = new sp::Tilemap(scene->getRoot(), "zelda1/overworld/tilemap.png", 1.0, 11);

        std::string err;
        json11::Json json = json11::Json::parse(sp::io::ResourceProvider::get("zelda1/overworld/7-7.json")->readAll(), err);
        int w = json["width"].int_value();
        int h = json["height"].int_value();
        for(int y=0; y<h; y++)
        {
            for(int x=0; x<w; x++)
            {
                tilemap->setTile(x, y, json["layers"][0]["data"][x + (h - 1 - y) * w].number_value() - 1);
            }
        }
    }
    
    engine->run();

    return 0;
}
