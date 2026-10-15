#include "backend_manager.h"
#include "backend_settings.h"
#include "buffers/uniform_buffer.h"
#include "buffers/vertex_buffer.h"
#include "resource_manager.h"
#include "buffers/vertex_buffer.h"
#include "window.h"
#include <climits>
#include <iostream>
#include <tuple>
#include <vulkan/vulkan_core.h>

namespace lina { namespace renderer { namespace backend {
    b8 manager::init(window<GraphicsBackend::Vulkan>* w)
    {
        mcurr_render_pass = 0;
        mcurr_pipeline = 0;
        mwindows.push(w);
        if (create_vulkan_instance())
        {
            std::tuple<VkInstance*, VkSurfaceKHR*> args = {&mvkinstance, &mvksurface};
            mwindows.top()->create_surface(args);
            b8 success = 
                create_device() && 
                create_swap_chain() && 
                create_sync_objects() && 
                create_command_structures();
            return true;
        }
        return false;
    }
    void manager::set_view(f32 x0, f32 y0, f32 width, f32 height)
    {
        width = mvk_extent.width * (width < 0) + width * !(width < 0);
        height = mvk_extent.height * (height < 0) + height * !(height < 0);
        VkViewport view_port = 
        {
            .x = x0,
            .y = y0,
            .width = width,
            .height = height,
            .minDepth = 0.01f,
            .maxDepth = 1.0f
        };
        vkCmdSetViewport(mvk_command_buffer, 
                0,
                1,
                &view_port);
    }
    void manager::begin_draw()
    {
        vkWaitForFences(
                mvkdevice, 1, 
                &msync.finflight, VK_TRUE, UINT64_MAX);
        vkResetFences(mvkdevice, 1, &msync.finflight);
        u32 image_idx;
        VkResult res = vkAcquireNextImageKHR(
                mvkdevice,
                mvk_swapchain,
                UINT64_MAX,
                msync.simage_available, VK_NULL_HANDLE,
                &image_idx
                );
        if (res == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate_swap_chain();
            return;
        }
        mcurr_img_idx = image_idx;
        VkCommandBufferBeginInfo begin_info = 
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        vkResetCommandBuffer(mvk_command_buffer, 0);
        vkBeginCommandBuffer(mvk_command_buffer, &begin_info);
        
        VkRect2D scissor = 
        {
            .offset = {0, 0},
            .extent = mvk_extent
        };
        vkCmdSetScissor(mvk_command_buffer,
                0,
                1,
                &scissor);
        VkClearValue clear_values[3];
        clear_values[0].color = {{0., 0., 0., 1.}};
        clear_values[1].color = {{0., 0., 0., 1.}};
        clear_values[2].depthStencil = {1.0, 0};
        VkRenderPassBeginInfo pass_begin_info = 
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = mvk_render_passes[0],
            .framebuffer = mvk_frame_buffers[mcurr_img_idx],
            .renderArea = {.offset = {0, 0}, .extent = mvk_extent},
            .clearValueCount = 3,
            .pClearValues = clear_values,
        };
        vkCmdBeginRenderPass(
                mvk_command_buffer, 
                &pass_begin_info, 
                VK_SUBPASS_CONTENTS_INLINE);

    }
    void manager::end_draw()
    {
        // submission
        
        vkCmdEndRenderPass(mvk_command_buffer);
        vkEndCommandBuffer(mvk_command_buffer);

        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSemaphore wait_semaphores[] = {msync.simage_available};
        VkSemaphore signal_semaphores[] = {msync.srender_finished};

        VkSubmitInfo sub_info = 
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = wait_semaphores,
            .pWaitDstStageMask = wait_stages,

            .commandBufferCount = 1,
            .pCommandBuffers = &mvk_command_buffer,

            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signal_semaphores,
        };
        vkQueueSubmit(
                mqueues.graphics_queue, 1,
                &sub_info, msync.finflight);

        // presentation
        VkPresentInfoKHR present_info = 
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signal_semaphores,
            .swapchainCount = 1,
            .pSwapchains = &mvk_swapchain,
            .pImageIndices = &mcurr_img_idx,
        };

        VkResult res = vkQueuePresentKHR(mqueues.present_queue, &present_info);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || mwindows.top()->is_resized())
        {
            mwindows.top()->set_resized(false);
            recreate_swap_chain();
        }
        mwindows.top()->update();
    }
    b8 manager::create_vulkan_instance()
    {
        VkApplicationInfo application_info = 
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Lina Engine",
            .applicationVersion = VK_MAKE_VERSION(1, 3, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3,
        };
        u64 extension_count = 0;
        const char** extensions = mwindows.top()->vulkan_extensions(&extension_count);
        VkInstanceCreateInfo inf = 
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = (u32)enabled_layers.size(),
            .ppEnabledLayerNames = enabled_layers.data(),
            .enabledExtensionCount = (u32)extension_count,
            .ppEnabledExtensionNames = extensions,
        };
        VkResult res;
        if ((res = vkCreateInstance(&inf, nullptr, &mvkinstance) 
            )!= VK_SUCCESS) 
        {
            return false;
        }
        return true;
    }
    b8 manager::create_device()
    {
        std::vector<VkPhysicalDevice> physical_devices;
        u32 physical_device_count = 0;
        vkEnumeratePhysicalDevices(
                mvkinstance,
                &physical_device_count ,
                nullptr
                );
        physical_devices.resize(physical_device_count);
        if(vkEnumeratePhysicalDevices(
                    mvkinstance,
                    &physical_device_count,
                    &(physical_devices[0])) != VK_SUCCESS)
        {
            return false;
        }

        // enumereate physicalDevices and find one suitable for graphics
        i32 index = -1;
        u32 format_count = 0;
        u32 present_count = 0;
        for (auto& d : physical_devices)
        {
            index++;
            mvk_graphics_idx = -1;
            mvk_present_idx = -1;
            format_count = 0;
            present_count = 0;
            // check extensions:
            u32 extensions_count;
            vkEnumerateDeviceExtensionProperties(d,
                    nullptr, 
                    &extensions_count,
                    nullptr);
            std::vector<VkExtensionProperties> vkexts(extensions_count);
            vkEnumerateDeviceExtensionProperties(d,
                    nullptr,
                    &extensions_count,
                    vkexts.data());


            std::set<std::string> required_extensions_set(
                    grequired_extensions.begin(), grequired_extensions.end()
                    );

            b8 extension_found = false;
            for (const auto& e : vkexts)
            {
                required_extensions_set.erase(e.extensionName);
                if (required_extensions_set.empty()) 
                {
                    extension_found = true;
                    break;
                }
            }
            if (!extension_found) continue;

            // swap chain support

            vkGetPhysicalDeviceSurfaceFormatsKHR(
                    d,
                    mvksurface,
                    &format_count,
                    nullptr);
            if (format_count == 0) continue;

            vkGetPhysicalDeviceSurfacePresentModesKHR(
                    d,
                    mvksurface,
                    &present_count,
                    nullptr);

            if (present_count == 0) continue;

            // get queue family
            u32 queue_family_properties_count;
            vkGetPhysicalDeviceQueueFamilyProperties(d, &queue_family_properties_count, nullptr);
            std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_properties_count);
            vkGetPhysicalDeviceQueueFamilyProperties(
                    d,
                    &queue_family_properties_count,
                    queue_family_properties.data());

            // check queue family supports graphics
            i32 family_idx = 0;
            b8 supports_graphics = false;
            for (auto& f : queue_family_properties)
            {
                if (f.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    mvk_graphics_idx = family_idx;
                }
                VkBool32 present_support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(
                        d, family_idx, mvksurface,
                        &present_support);
                if (present_support)
                {
                    mvk_present_idx = family_idx;
                }
                if (mvk_present_idx > -1 && mvk_graphics_idx > -1)
                {
                    supports_graphics = true;
                    break;
                }
                family_idx++;
            }
            if (!supports_graphics) continue;
            VkPhysicalDeviceVulkan12Features vulkan12Features{};
            vulkan12Features.sType =
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

            VkPhysicalDeviceFeatures2 features2{};
            features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features2.pNext = &vulkan12Features;

            vkGetPhysicalDeviceFeatures2(d, &features2);

            if (!vulkan12Features.uniformBufferStandardLayout) 
            {
                continue;
            }
            break;
        }
        // get physical device properties

        if (index >= physical_device_count) return false;
        auto& physical_device = physical_devices[index];

        // get properties of physical device
        vkGetPhysicalDeviceProperties(physical_device, &mpd.properties);
        vkGetPhysicalDeviceMemoryProperties(physical_device, &mpd.memory_properties);

        // prepate for swap chain creation
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                physical_device,
                mvksurface,
                &mswap_details.capabilities);


        mswap_details.surface_formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
                physical_device,
                mvksurface,
                &format_count,
                mswap_details.surface_formats.data());

        mswap_details.present_modes.resize(present_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
                physical_device,
                mvksurface,
                &present_count,
                mswap_details.present_modes.data());

        // logical device creation
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<u32> unique_queue_idxs = 
        {
            (u32)mvk_present_idx,
            (u32)mvk_graphics_idx
        };

        f32 priority = 1.0f;
        for (auto idx : unique_queue_idxs)
        {
            VkDeviceQueueCreateInfo info = 
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = idx,
                .queueCount = 1,
                .pQueuePriorities = &priority,
            };
            queue_create_infos.push_back(info);
        }
        VkPhysicalDeviceFeatures features = 
        {
            .depthClamp = VK_TRUE,
            .fillModeNonSolid = VK_TRUE,
            .samplerAnisotropy = VK_TRUE,
        };
        VkPhysicalDeviceUniformBufferStandardLayoutFeaturesKHR ubsl{};
        ubsl.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES_KHR;
        ubsl.uniformBufferStandardLayout = VK_TRUE;
        VkDeviceCreateInfo device_info = 
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &ubsl,
            .queueCreateInfoCount = (u32)queue_create_infos.size(),
            .pQueueCreateInfos = queue_create_infos.data(),
            .enabledExtensionCount = (u32)grequired_extensions.size(),
            .ppEnabledExtensionNames = grequired_extensions.data(),
            .pEnabledFeatures = &features,
        };

        if (vkCreateDevice(
                    physical_device, 
                    &device_info,
                    nullptr,
                    &mvkdevice
                    ) != VK_SUCCESS)
        {
            std::cerr << "Err creating device;";
            return false;
        }
        vkGetDeviceQueue(
                mvkdevice,
                (u32)mvk_graphics_idx,
                0,
                &(mqueues.graphics_queue)
                );
        vkGetDeviceQueue(
                mvkdevice,
                (u32)mvk_present_idx,
                0,
                &(mqueues.present_queue)
                );

        return true;
    }
    b8 manager::create_swap_chain()
    {
        // set format
        VkSurfaceFormatKHR swap_format = mswap_details.surface_formats[0];
        for (const auto& format : mswap_details.surface_formats)
        {
            if (format.format == VK_FORMAT_R8G8B8A8_SRGB 
                    && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                swap_format = format;
                break;
            }
        }
        mswap_details.chosen_format = swap_format;

        // set present mode
        VkPresentModeKHR swap_present_mode = VK_PRESENT_MODE_FIFO_KHR;
        for (const auto& present : mswap_details.present_modes)
        {
            if (present == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                swap_present_mode = present;
                break;
            }
        }
        mswap_details.chosen_present_mode = swap_present_mode;

        // choose extent (full screen for now)
        i32 w = mwindows.top()->get_width();
        i32 h = mwindows.top()->get_height();
        VkExtent2D swap_extent = {.width = (u32)w, .height = (u32)h};

        mvk_extent = swap_extent;


        b8 choose_max = 
            (mswap_details.capabilities.maxImageCount > 0) && 
            (mswap_details.capabilities.minImageCount > mswap_details.capabilities.maxImageCount);
        u32 img_count = 
            (mswap_details.capabilities.minImageCount + 1) * !choose_max +
            (mswap_details.capabilities.maxImageCount) * choose_max;
        u32 queue_family_indices[] = {
            (u32)mvk_graphics_idx,
            (u32)mvk_present_idx
        };

        VkSwapchainCreateInfoKHR create_info = 
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = mvksurface,
            .minImageCount = img_count,
            .imageFormat = swap_format.format,
            .imageColorSpace = swap_format.colorSpace,
            .imageExtent = swap_extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = mswap_details.capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = swap_present_mode,
            .clipped = VK_TRUE,
        };
        if (mvk_graphics_idx != mvk_present_idx)
        {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = &queue_family_indices[0];
        }
        if (vkCreateSwapchainKHR(
                    mvkdevice,&create_info, nullptr,
                    &mvk_swapchain) != VK_SUCCESS)
        {
            return false;
        }

        // creating images and image views
        u32 swap_chain_images_count;
        vkGetSwapchainImagesKHR(mvkdevice, mvk_swapchain,
                &swap_chain_images_count, nullptr);
        mvk_swap_images.resize(swap_chain_images_count);
        vkGetSwapchainImagesKHR(mvkdevice, mvk_swapchain,
                &swap_chain_images_count, mvk_swap_images.data());

        mvk_swap_image_views.resize(swap_chain_images_count);

        for (u32 i = 0; i < swap_chain_images_count; i++)
        {
            VkImageViewCreateInfo iv_create_info = 
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = mvk_swap_images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = mswap_details.chosen_format.format,
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_R,
                    .g = VK_COMPONENT_SWIZZLE_G,
                    .b = VK_COMPONENT_SWIZZLE_B,
                    .a = VK_COMPONENT_SWIZZLE_A,
                },
                .subresourceRange = 
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };
            if (vkCreateImageView(mvkdevice, &iv_create_info,
                        nullptr, &mvk_swap_image_views[i]) != VK_SUCCESS)
            {
                mvk_swap_images.clear();
                mvk_swap_image_views.clear();
                return false;
            }
        }

        return true;
    }
    b8 manager::create_sync_objects()
    {
        VkSemaphoreCreateInfo sem_info =
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        if ((vkCreateSemaphore(mvkdevice, &sem_info, 
                        nullptr,&msync.simage_available) != VK_SUCCESS))
        {
            return false;
        }
        if ((vkCreateSemaphore(mvkdevice, &sem_info, 
                        nullptr,&msync.srender_finished) != VK_SUCCESS))
        {
            return false;
        }

        VkFenceCreateInfo fence_info =
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        if (vkCreateFence(mvkdevice, &fence_info,
                    nullptr, & msync.finflight) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }
    b8 manager::create_default_render_pass()
    {
        VkAttachmentDescription colour_attachment = 
        {
            .format = mswap_details.chosen_format.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };
        VkAttachmentReference colour_ref = 
        {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };


        VkSubpassDescription subpass_desc = 
        {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colour_ref,
        };
        VkAttachmentDescription attachments[2] = {colour_attachment};
        VkRenderPassCreateInfo render_pass_info = 
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &attachments[0],
            .subpassCount = 1,
            .pSubpasses = &subpass_desc,
        };

        mvk_render_passes.resize(1);
        if(vkCreateRenderPass(mvkdevice, &render_pass_info,
                    nullptr, &mvk_render_passes[0]) != VK_SUCCESS)
        {
            return false;
        }
        return create_frame_buffers();
    }
    b8 manager::create_frame_buffers(u32 idx)
    {
        mvk_frame_buffers.resize(mvk_swap_image_views.size());
        for (i32 i = 0; i < mvk_frame_buffers.size(); i++)
        {
            VkImageView img_views[1] = {mvk_swap_image_views[i]};
            VkFramebufferCreateInfo create_info = 
            {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = mvk_render_passes[idx],
                .attachmentCount = 1,
                .pAttachments = &img_views[0],
                .width = mvk_extent.width,
                .height = mvk_extent.height,
                .layers = 1,
            };
            if (vkCreateFramebuffer(mvkdevice, &create_info,
                        nullptr, &mvk_frame_buffers[i]) != VK_SUCCESS) 
            {
                return false;
            }
        }
        return true;

    }
    b8 manager::create_command_structures()
    {
        VkCommandPoolCreateInfo cp_create = 
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = (u32)mvk_graphics_idx,
        };
        if (vkCreateCommandPool(mvkdevice, &cp_create,
                    nullptr, &mvk_command_pool) != VK_SUCCESS)
        {
            return false;
        }
        VkCommandBufferAllocateInfo cb_alloc =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = mvk_command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        if (vkAllocateCommandBuffers(mvkdevice, &cb_alloc,
                    &mvk_command_buffer) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }
    b8 manager::create_shader(shader::shader* sh)
    {
        const auto& programs = sh->get_programs();
        auto old_sz = mvk_shader_modules.size();
        mvk_shader_modules.resize(programs.size() + old_sz);
        for (i32 i = 0 ; i < programs.size(); i++)
        {
            VkShaderModuleCreateInfo create_info= {};
            create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.codeSize = programs[i].program.size();
            create_info.pCode =(const u32*)(programs[i].program.data());
            vkCreateShaderModule(mvkdevice, &create_info,
                    nullptr, &mvk_shader_modules[i + old_sz]);
        }
        return true;
    }
    VkDescriptorSetLayout manager::create_ds_layout(shader::shader* s)
    {
        auto bindingsCount = s->get_binding_count();
        auto uniformCount = s->get_uniform_count();
        const auto& uniformRef = s->get_uniforms();
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(bindingsCount);

        VkDescriptorSetLayout setLayout;

        for (auto& ur : uniformRef)
        {
            VkDescriptorSetLayoutBinding uboLayoutBinding
            {
                .binding = ur.binding,
                    .descriptorType = (VkDescriptorType)ur.type, 
                    .descriptorCount = 1,
                    .stageFlags = (VkShaderStageFlags)ur.stage, 
                    .pImmutableSamplers = nullptr
            };
            bindings.push_back(uboLayoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = static_cast<u32>(bindings.size()),
                .pBindings = bindings.data()
        };

        if(vkCreateDescriptorSetLayout(
                    mvkdevice,
                    &layoutInfo,
                    nullptr,
                    &setLayout) != VK_SUCCESS)
            std::cerr<<"No layout created!\n";
        return setLayout;
    }
    VkDescriptorPool manager::create_dpool(shader::shader* shader)
    {
        auto binding_count = shader->get_binding_count();
        auto& uniforms =shader->get_uniforms();

        std::vector<VkDescriptorPoolSize> pool_sizes;
        pool_sizes.reserve(binding_count);

        VkDescriptorPool vkpool;

        u32 u = 0;
        if ((u = shader->get_static_count()) > 0)
        {
            pool_sizes.push_back(
                    {
                    .type =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = u 
                    });
        }

        if ((u = shader->get_dynamic_count()) > 0)
        {
            pool_sizes.push_back(
                    {
                    .type =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                    .descriptorCount = u 
                    });
        }

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = 1,
                .poolSizeCount = static_cast<u32>(pool_sizes.size()),
                .pPoolSizes = pool_sizes.data(),
        };

        if(vkCreateDescriptorPool(
                    mvkdevice,
                    &poolInfo,
                    nullptr,
                    &vkpool) != VK_SUCCESS)
            std::cerr<<"Error creating set\n";
        return vkpool;
    }
    void manager::add_dsets(shader::shader* shader, i32 shader_idx)
    {
        auto* layout = m_resource_manager.get_dset_layout_pointer(shader_idx);
        auto dpool = m_resource_manager.get_dpool_pointer(shader_idx)[0];

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = dpool,
                .descriptorSetCount = 1,
                .pSetLayouts = layout
        };
        m_resource_manager.add_dsets(allocInfo, this);

        std::vector<VkWriteDescriptorSet> descriptorWrite;
        auto uniform_count = shader->get_uniform_count();
        descriptorWrite.resize(uniform_count);

        std::vector<VkDescriptorBufferInfo> buff_infos;
        buff_infos.resize(uniform_count);

        auto& refUniform = shader->get_uniforms();

        auto** upointers = m_resource_manager.get_ub_pointer(shader_idx);
        auto* dsetpointers = m_resource_manager.get_dset_pointer(shader_idx);
        for (int i = 0; i < refUniform.size(); i++)
        {
            buff_infos[i] =  
            {
                .buffer = upointers[i]->get_buffer(),
                .offset = 0,
                .range = upointers[i]->get_size()
            };

            descriptorWrite[i] = 
            (VkWriteDescriptorSet){
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = dsetpointers[0],
                .dstBinding = refUniform[i].binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = (VkDescriptorType)refUniform[i].type,
                .pImageInfo = nullptr,
                .pBufferInfo = &buff_infos[i],
                .pTexelBufferView = nullptr,
            };
        }

        vkUpdateDescriptorSets(
               mvkdevice ,
                static_cast<u32>(descriptorWrite.size()),
                descriptorWrite.data(),
                0,
                nullptr);
    }
    b8 manager::create_graphics_pipeline(buffers::vertex* vb, shader::shader* shader, i32 vertex_idx, i32 shader_idx)
    {
        //auto* ppipeline = m_resource_manager.get_pipeline_pointer(shader_idx);
        const auto& mod = shader->get_programs();
        auto count = mod.size();
        i32 offset = mvk_shader_modules.size();
        auto* pstages = m_resource_manager.get_shader_info_pointer(shader_idx);
        if (pstages == nullptr && shader->get_programs().size() > 0)
        {
            auto module_offset = mvk_shader_modules.size();
            create_shader(shader);
            for (const auto& p : mod)
            {
                VkPipelineShaderStageCreateInfo shader_info
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .stage = (VkShaderStageFlagBits)p.stage,
                        .module = mvk_shader_modules[module_offset++],
                        .pName = "main"
                };
                m_resource_manager.add_shader_stage_info(shader_info);
            }
            pstages = m_resource_manager.get_shader_info_pointer(shader_idx);
        }

        auto* psranges = m_resource_manager.get_ps_range_pointer(shader_idx);
        const auto& refPs = shader->get_ps();
        if (psranges == nullptr && shader->get_ps_count() > 0)
        {
            for (const auto& rps : refPs)
            {
                VkPushConstantRange psRange =
                {
                    .stageFlags = (VkShaderStageFlags)rps.stage,
                    .offset = rps.offset,
                    .size = rps.size,
                };
                m_resource_manager.add_shader_ps(psRange);
            }
           psranges = m_resource_manager.get_ps_range_pointer(shader_idx);
        }

        auto* puniforms = m_resource_manager.get_ub_pointer(shader_idx);
        if (puniforms == nullptr && shader->get_uniform_count() > 0)
        {
            shader->combine_uniforms();
            auto& uniforms = shader->get_uniforms();
            for (int j = 0; j < uniforms.size(); j++)
            {
                buffers::uniform* new_buf = new buffers::uniform();
                new_buf->init(this);
                new_buf->construct(uniforms[j].size, uniforms[j].count);
                m_resource_manager.add_ub(new_buf);
            }
            puniforms = m_resource_manager.get_ub_pointer(shader_idx);
        }


        auto* dpool = m_resource_manager.get_dpool_pointer(shader_idx);
        if (dpool == nullptr)
        {
            auto dpool = create_dpool(shader);
            m_resource_manager.add_dpool(dpool);
        }
        auto* pdset_layout = m_resource_manager.get_dset_layout_pointer(shader_idx);
        if (pdset_layout == nullptr)
        {
            auto setlayout = create_ds_layout(shader);
            m_resource_manager.add_dset_layout(setlayout);
            pdset_layout = m_resource_manager.get_dset_layout_pointer(shader_idx);
        }

        auto* pdset = m_resource_manager.get_dset_pointer(shader_idx);
        if (pdset == nullptr)
        {
            add_dsets(shader, shader_idx);
            pdset = m_resource_manager.get_dset_pointer(shader_idx);
        }
        VkPipelineVertexInputStateCreateInfo* pvertex_input = nullptr;
        if (vertex_idx != -1 && vb != nullptr)
        {
            auto* ppvertex_input = m_resource_manager.get_vertex_info_pointer(vertex_idx);
            if (ppvertex_input == nullptr)
            {
                auto* attributeDescriptions = vb->get_attribute_descriptions().data();
                u32 sz = vb->get_attribute_descriptions().size();
                auto* bindingDescription = new VkVertexInputBindingDescription;
                *bindingDescription = vb->get_binding_description();
                VkPipelineVertexInputStateCreateInfo* vinput_info = new VkPipelineVertexInputStateCreateInfo;
                vinput_info->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vinput_info->vertexBindingDescriptionCount = 1;
                vinput_info->pVertexBindingDescriptions = bindingDescription;
                vinput_info->vertexAttributeDescriptionCount = sz;
                vinput_info->pVertexAttributeDescriptions = attributeDescriptions;
                vinput_info->flags = 0;

                m_resource_manager.add_vertex_input_info(vinput_info);

                ppvertex_input = m_resource_manager.get_vertex_info_pointer(vertex_idx);
            }
            pvertex_input = *ppvertex_input;
        }


        auto* ppipeline_layout = m_resource_manager.get_pipeline_layout_pointer(shader_idx);
        if (ppipeline_layout == nullptr)
        {
            VkPipelineLayout layout;
            VkPipelineLayoutCreateInfo pipeline_layout_info{};
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.pSetLayouts = pdset_layout;
            pipeline_layout_info.setLayoutCount = 1;
            pipeline_layout_info.pushConstantRangeCount = shader->get_ps_count();
            pipeline_layout_info.pPushConstantRanges = psranges;

            vkCreatePipelineLayout(mvkdevice, &pipeline_layout_info,
                    nullptr, &layout);
            m_resource_manager.add_pipeline_layout(layout);

            ppipeline_layout = m_resource_manager.get_pipeline_layout_pointer(shader_idx);
        }

        

        VkPipelineDepthStencilStateCreateInfo depth_stencil
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_TRUE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState color_blend_attachment
        {
            .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = 
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT 
                    | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        VkPipelineColorBlendStateCreateInfo color_blending =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f},
        };
        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
            VK_DYNAMIC_STATE_POLYGON_MODE_EXT,
        };
        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();

        VkGraphicsPipelineCreateInfo info = 
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

            .pInputAssemblyState = &input_assembly,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depth_stencil,
            .pColorBlendState = &color_blending,
            .pDynamicState = &dynamic_state,

            .renderPass = mvk_render_passes[mcurr_render_pass],
            .subpass = 0,

            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };
        info.stageCount = count;
        info.pStages = pstages;
        if (vertex_idx != -1)
        {
            std::cerr << "FOUND from: " << vertex_idx << ", " << shader_idx << '\n';
            info.pVertexInputState = pvertex_input;
        }
        info.layout = *ppipeline_layout;

        auto* _ = m_resource_manager.get_pipeline_pointer(INT_MAX);
        VkPipeline pipeline;
        vkCreateGraphicsPipelines(
                mvkdevice,
                VK_NULL_HANDLE,
                1,
                &info,
                nullptr,
                &pipeline
                );
        m_resource_manager.add_pipeline(pipeline);
        return true;
    }
    void manager::bind_shader(u32 idx)
    {
        auto* pdset = m_resource_manager.get_dset_pointer(idx);
        auto playout = *m_resource_manager.get_pipeline_layout_pointer(idx);
        vkCmdBindDescriptorSets(
                mvk_command_buffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                playout,
                0,
                1,
                pdset,
                0, 
                nullptr
                );
    }
    void manager::bind(u32 idx)
    {
        auto* ppipeline = m_resource_manager.get_pipeline_pointer(idx);
        vkCmdBindPipeline(
                mvk_command_buffer, 
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                *ppipeline);
        mcurr_pipeline = idx;
    }

    
    b8 manager::create_default_graphics_pipeline()
    {
        mvk_pipeline.emplace_back();
        mvk_pipeline_layout.emplace_back();
        VkPipelineShaderStageCreateInfo vert_shader_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = mvk_shader_modules[0],
                .pName = "main"
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = 0,
                .pVertexBindingDescriptions = nullptr,
                .vertexAttributeDescriptionCount = 0,
                .pVertexAttributeDescriptions = nullptr,
        };

        VkPipelineShaderStageCreateInfo frag_shader_info
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = mvk_shader_modules[1],
                .pName = "main"
        };
        VkPipelineShaderStageCreateInfo shader_stages[] = 
        {
            vert_shader_info, 
            frag_shader_info
        };

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        vkCreatePipelineLayout(mvkdevice, &pipeline_layout_info,
                nullptr, &mvk_pipeline_layout[0]);

        auto info = default_pipeline_settings();
        info.stageCount = 2;
        info.pVertexInputState = &vertex_input_info;
        info.layout = mvk_pipeline_layout[0];
        info.renderPass = mvk_render_passes[0];
        info.subpass = 0;
        vkCreateGraphicsPipelines(
                mvkdevice,VK_NULL_HANDLE,1,
                &info, nullptr,
                &mvk_pipeline[0]
                );
        for (auto module : mvk_shader_modules)
        {
            vkDestroyShaderModule(mvkdevice, module, nullptr);
        }
        return true;
    }
    VkGraphicsPipelineCreateInfo manager::default_pipeline_settings()
    {
        VkPipelineDepthStencilStateCreateInfo depth_stencil
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_TRUE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState color_blend_attachment
        {
            .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = 
                    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT 
                    | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        VkPipelineColorBlendStateCreateInfo color_blending =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f},
        };
        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
        };
        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates = dynamic_states.data();

        VkGraphicsPipelineCreateInfo pipeline_info = 
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

            .pInputAssemblyState = &input_assembly,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depth_stencil,
            .pColorBlendState = &color_blending,
            .pDynamicState = &dynamic_state,

            .renderPass = mvk_render_passes[mcurr_render_pass],
            .subpass = 0,

            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        return pipeline_info;
    }
    void manager::clear_swap_chain()
    {
        for (i32 i = 0; i < mvk_frame_buffers.size(); i++)
        {
            vkDestroyFramebuffer(mvkdevice, mvk_frame_buffers[i],
                    nullptr);
            vkDestroyImageView(mvkdevice, 
                    mvk_swap_image_views[i], nullptr);
        }
        vkDestroySwapchainKHR(
                mvkdevice, mvk_swapchain,
                nullptr
                );
    }
    b8 manager::recreate_swap_chain()
    {
        vkDeviceWaitIdle(mvkdevice);
        clear_swap_chain();
        return create_swap_chain() && create_frame_buffers(mcurr_render_pass);
    }
    void manager::update_ps(i64 shader_idx, i64 ps_id, void* data)
    {
        auto ps = *(m_resource_manager.get_ps_range_pointer(shader_idx) + ps_id);
        auto layout = *(m_resource_manager.get_pipeline_layout_pointer(shader_idx));
        vkCmdPushConstants(
                mvk_command_buffer,
                layout,
                ps.stageFlags,
                ps.offset,
                ps.size,
                data
                );
    }
    void manager::update_uniform_buffer(i64 shader_idx, i64 uniform_id, void* data)
    {
        auto ps = *(m_resource_manager.get_ub_pointer(shader_idx) + uniform_id);
        ps->write_data(data);
    }

}}}
