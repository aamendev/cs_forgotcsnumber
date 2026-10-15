#ifndef GRAPHICS_FRONTEND_RENDERSETTINGS_H
#define GRAPHICS_FRONTEND_RENDERSETTINGS_H
#include <vulkan/vulkan_core.h>
namespace lina { namespace renderer {
    enum class draw_mode
    {
        solid = VK_POLYGON_MODE_FILL,
        wireframe = VK_POLYGON_MODE_LINE,
        point = VK_POLYGON_MODE_POINT,
    };
    enum class primitive_mode 
    {
        triangle_list = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };
    struct renderer_settings 
    {
        primitive_mode sprimitive = primitive_mode::triangle_list;
        draw_mode sdraw = draw_mode::solid;
    };
}}
#endif
