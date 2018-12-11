#ifndef DARKNESS_OVERLAY_RENDERER_H
#define DARKNESS_OVERLAY_RENDERER_H

#include <sp2/graphics/scene/basicnoderenderpass.h>

class DarknessRenderPass : public sp::BasicNodeRenderPass
{
public:
    bool enabled = false;
    
    DarknessRenderPass();
    virtual void render(sp::RenderQueue& queue) override;

protected:
    virtual void addNodeToRenderQueue(sp::RenderQueue& queue, sp::Node* node) override;

private:
    float circle_radius_scale;
    sp::RenderData circle_render_data;
    sp::RenderData overlay_render_data;
};

#endif//DARKNESS_OVERLAY_RENDERER_H
