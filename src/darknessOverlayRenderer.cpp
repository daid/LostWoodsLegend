#include "darknessOverlayRenderer.h"
#include "lightSource.h"
#include <sp2/graphics/meshdata.h>
#include <sp2/graphics/opengl.h>
#include <sp2/scene/scene.h>


DarknessRenderPass::DarknessRenderPass()
{
    circle_render_data.type = sp::RenderData::Type::Normal;
    circle_render_data.shader = sp::Shader::get("internal:color.shader");
    circle_render_data.mesh = sp::MeshData::createCircle(1, 32);
    
    overlay_render_data.type = sp::RenderData::Type::None;
    overlay_render_data.shader = sp::Shader::get("internal:color.shader");
    overlay_render_data.mesh = sp::MeshData::createQuad(sp::Vector2f(100, 100));
}

void DarknessRenderPass::render(sp::RenderQueue& queue)
{
    queue.add([]()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    });
    if (!enabled)
        return;

    //Setup rendering for light areas to the stencil buffer.
    queue.add([]()
    {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NEVER, 1, 0xFF);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
        glStencilMask(0xFF);
        glColorMask(false, false, false, false);
        glDisable(GL_DEPTH_TEST);
    });
    
    //Render light areas.
    circle_radius_scale = 1.0;//First render the partially lit areas.
    sp::BasicNodeRenderPass::render(queue);    
    queue.add([]()
    {
        glStencilFunc(GL_NEVER, 2, 0xFF);
    });
    circle_radius_scale = 0.7;//Then render the fully 100% lit areas.
    sp::BasicNodeRenderPass::render(queue);    

    //Setup rendering for the 100% dark areas, with a fullscreen overlay to stencil buffer value 0.
    queue.add([]()
    {
        glStencilFunc(GL_EQUAL, 0, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glColorMask(true, true, true, true);
    });
    
    queue.setCamera(sp::Scene::get("MAIN")->getCamera());
    overlay_render_data.color = sp::Color(0, 0, 0);
    queue.add(sp::Matrix4x4f::identity(), overlay_render_data);

    //Setup rendering for 50% dark, full screen overlay to stencil buffer value 1.
    queue.add([]()
    {
        glStencilFunc(GL_EQUAL, 1, 0xFF);
    });
    overlay_render_data.color = sp::Color(0, 0, 0, 0.5);
    queue.add(sp::Matrix4x4f::identity(), overlay_render_data);
    
    //Reset OpenGL state to normal.
    queue.add([]()
    {
        glDisable(GL_STENCIL_TEST);
        glColorMask(true, true, true, true);
        glEnable(GL_DEPTH_TEST);
    });
}

void DarknessRenderPass::addNodeToRenderQueue(sp::RenderQueue& queue, sp::Node* node)
{
    sp::P<LightSource> light = sp::P<sp::Node>(node);
    if (light)
    {
        float r = light->radius * circle_radius_scale;
        circle_render_data.scale = sp::Vector3f(r, r, 1);
        queue.add(light->getGlobalTransform(), circle_render_data);
    }
}
