#ifndef VULKAN_GLFW_WINDOW_H
#define VULKAN_GLFW_WINDOW_H
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "lengine/lrenderer/vulkan_backend/window.h"
#include "types.h"
namespace lina { namespace renderer { namespace backend { 
    class glfw_vulkan_window : public window<GraphicsBackend::Vulkan>
    {
        public:
            glfw_vulkan_window() = default;
            virtual  ~glfw_vulkan_window() override;
            virtual b8 create(settings& s) override;
            virtual void update() override;
            virtual inline void create_surface(typename SurfaceArgs<GraphicsBackend::Vulkan>::ArgType data) override;
            virtual inline b8 is_closed() const override;
            virtual inline const char** vulkan_extensions(u64* count) override;
            inline GLFWwindow* get_window() {return mwindow;}
            void register_callbacks() override;
            b8 init();
        public:
            GLFWwindow* mwindow;
    };
    inline void glfw_vulkan_window::create_surface(typename SurfaceArgs<GraphicsBackend::Vulkan>::ArgType data)
    {
        auto& [instance_ptr, surface_ptr] = data;
        glfwCreateWindowSurface(*instance_ptr, mwindow, nullptr,surface_ptr);
    }
    inline b8 glfw_vulkan_window::is_closed() const
    {
        return glfwWindowShouldClose(mwindow);
    }
    inline const char** glfw_vulkan_window::vulkan_extensions(u64* count)
    {
        return glfwGetRequiredInstanceExtensions((u32*)count);
    }
}}}
#endif
