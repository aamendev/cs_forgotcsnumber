#ifndef BACKEND_MANAGER_H
#define BACKEND_MANAGER_H
#include "types.h"
#include "resource_manager.h"
#include "lshader/shader.h"
#include "window.h"
#include <stack>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "backend_settings.h"
#include <set>
namespace lina { namespace renderer { namespace backend {
    namespace buffers 
    {
        class vertex; }
    class manager 
    {
        struct vk_physical_device 
        {
            VkPhysicalDeviceProperties properties; VkPhysicalDeviceMemoryProperties memory_properties;
        };
        struct vk_queues 
        {
            VkQueue present_queue;
            VkQueue graphics_queue;
        };
        struct vk_swap_chain_details
        {
            std::vector<VkPresentModeKHR> present_modes;
            std::vector<VkSurfaceFormatKHR> surface_formats;
            VkSurfaceCapabilitiesKHR capabilities;
            VkSurfaceFormatKHR chosen_format;
            VkPresentModeKHR chosen_present_mode;
        };
        struct vk_sync_object
        {
            VkSemaphore simage_available;
            VkSemaphore srender_finished;
            VkFence finflight;
        };
        struct vk_render_pass_data 
        {
            i32 subpass_count;

        };
        struct vk_image
        {
            VkImage img;
            VkImageView view;
            VkDeviceMemory memory;
        };
        public:
        friend class buffer;
        manager() : mwindows(){}
        b8 init(window<GraphicsBackend::Vulkan>* win);
        inline void push_window(window<GraphicsBackend::Vulkan>* win);
        inline void pop_window();
        inline b8 request_render_pass() {return false;};
        b8 create_default_graphics_pipeline();
        b8 create_graphics_pipeline(buffers::vertex* vb, shader::shader* shader, i32 vertex_idx, i32 shader_idx);
        void create_surface();
        void update_ps(i64 shader_idx, i64 ps_id, void* data);
        void update_uniform_buffer(i64 shader_idx, i64 uniform_id, void* data);
        void begin_draw();
        void set_view(f32 x0 = 0, f32 y0 = 0, f32 width = -1, f32 height = -1);
        void bind(u32 idx = 0);
        void bind_shader(u32 idx = 0);
        void end_draw();
        b8 create_default_render_pass();
        void render();
        inline std::pair<f32, f32> get_dimensions();
        private:
        b8 create_device();
        b8 create_vulkan_instance();
        b8 create_swap_chain();
        b8 create_sync_objects();
        b8 create_frame_buffers(u32 idx = 0);
        b8 create_command_structures();
        void clear_swap_chain();
        b8 recreate_swap_chain();
        b8 create_shader(shader::shader* sh);
        VkGraphicsPipelineCreateInfo default_pipeline_settings();
        VkDescriptorSetLayout create_ds_layout(shader::shader* s);
        VkDescriptorPool create_dpool(shader::shader* s);
        void add_dsets(shader::shader* s, i32 shader_idx);

        public:
        VkDevice mvkdevice;
        vk_physical_device mpd;
        VkCommandBuffer mvk_command_buffer;
        VkCommandPool mvk_command_pool;
        vk_queues mqueues;
        private:
        std::stack<window<GraphicsBackend::Vulkan>*> mwindows;
        VkInstance mvkinstance;
        VkSurfaceKHR mvksurface;
        vk_swap_chain_details mswap_details;
        i32 mvk_present_idx = -1;
        i32 mvk_graphics_idx = -1;
        std::vector<VkImage> mvk_swap_images;
        std::vector<VkImageView> mvk_swap_image_views;
        VkSwapchainKHR mvk_swapchain;
        vk_sync_object msync;
        std::vector<VkRenderPass> mvk_render_passes;
        std::vector<VkFramebuffer> mvk_frame_buffers;
        VkExtent2D mvk_extent;

        std::vector<VkShaderModule> mvk_shader_modules;
        std::vector<VkPipelineLayout> mvk_pipeline_layout;
        std::vector<VkPipeline> mvk_pipeline;
        u32 mcurr_img_idx;
        u32 mcurr_render_pass = 0;
        u32 mcurr_pipeline;
        std::vector<vk_image> mcol_images;
        std::vector<vk_image> mdepth_images;
        resource_manager m_resource_manager;

    };
    inline std::pair<f32, f32> manager::get_dimensions()
    {
        return {mwindows.top()->get_width(), mwindows.top()->get_height()};
    }
    inline void manager::push_window(window<GraphicsBackend::Vulkan>* win)
    {
        mwindows.push(win);
    }
    inline void manager::pop_window()
    {
        mwindows.pop();
    }
}}}
#endif
