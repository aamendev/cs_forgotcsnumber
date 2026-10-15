#ifndef BACKEND_SETTINGS_H
#define BACKEND_SETTINGS_H
#include "types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan_core.h>
namespace lina { namespace renderer { namespace backend {
    inline const std::vector<const char*> grequired_extensions = 
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_EXT_extended_dynamic_state3",
    };
    inline const std::vector<const char*> enabled_layers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };
}}}
#endif
