#include "renderer.h"
#include "render_settings.h"
#include "types.h"
#include <utility>
#include <vulkan/vulkan_core.h>

namespace lina { namespace renderer {
    template <GraphicsBackend T>
        void lrenderer<T>::begin_draw()
        {
            mmanger.begin_draw();
        }
    template <GraphicsBackend T>
        void lrenderer<T>::set_view(f32 x0, f32 y0, f32 width, f32 height)
        {
            mmanger.set_view(x0, y0, width, height);
        }
    template <GraphicsBackend T>
        void lrenderer<T>::end_draw()
        {
            mmanger.end_draw();
        }
    template <GraphicsBackend T>
        void lrenderer<T>::bind(u32 idx)
        {
            mcurr_pipeline = idx;
            mmanger.bind(idx);
        }
    template <GraphicsBackend T>
        void lrenderer<T>::bind_shader(u32 idx)
        {
            mmanger.bind_shader(idx);
        }
    template <GraphicsBackend T>
        void lrenderer<T>::render()
        {
            auto vertId = mshader_mappings[mcurr_pipeline].first;
            PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT = 
                reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetDeviceProcAddr(
                            mmanger.mvkdevice, "vkCmdSetPolygonModeEXT"));
            if (vertId != -1)
            {
                vkCmdSetPrimitiveTopology(mmanger.mvk_command_buffer,
                        (VkPrimitiveTopology)msettings.sprimitive);
                vkCmdSetPolygonModeEXT(mmanger.mvk_command_buffer,
                        (VkPolygonMode)msettings.sdraw);
                //auto vertId = mshader_mappings[mcurr_pipeline].second;
                auto vb = mvertex_buffers[vertId];
                auto ib = mindex_buffers[vertId];
                VkBuffer vbs[] = {vb.get_buffer()};
                VkBuffer ib_buf = ib.get_buffer();
                VkDeviceSize offsets[] = {0};

                vkCmdBindVertexBuffers(mmanger.mvk_command_buffer, 0,  1, vbs, offsets);

                vkCmdBindIndexBuffer(mmanger.mvk_command_buffer,
                        ib_buf, 0, VK_INDEX_TYPE_UINT32);

                vkCmdDrawIndexed(
                        mmanger.mvk_command_buffer,
                        ib.get_count(), 1, 0, 0, 0);
            }
            else 
            {
                vkCmdSetPolygonModeEXT(mmanger.mvk_command_buffer,
                        (VkPolygonMode)draw_mode::solid);
                vkCmdDraw(
                        mmanger.mvk_command_buffer,
                        3,
                        1,
                        0,
                        0);
            }
        }
    template <GraphicsBackend T>
    u32 lrenderer<T>::add_shader(const shader::shader& s)
    {
        mshaders.push_back(s);
        return mshaders.size() - 1;
    }
    template <GraphicsBackend T>
    u32 lrenderer<T>::add_mapping(u32 vidx, u32 sidx)
    {
        mshader_mappings.push_back(std::make_pair(vidx, sidx));
        return mshader_mappings.size() - 1;
    }

    template <GraphicsBackend T>
        void lrenderer<T>::submit_scene()
        {
            if (m_unadded_pipeline == 0 && mshader_mappings.size() > 0)
            {
                mmanger.create_default_render_pass();
            }
            for (int i = m_unadded_pipeline; i < mshader_mappings.size(); i++)
            {
                const auto& [vidx, sidx] = mshader_mappings[i];
                if (vidx != -1)
                {
                    mmanger.create_graphics_pipeline(&mvertex_buffers[vidx], &mshaders[sidx], vidx, sidx);
                }
                else 
                {
                    mmanger.create_graphics_pipeline(nullptr, &mshaders[sidx], vidx, sidx);
                }
                m_unadded_pipeline++;
            }
        }

    template <GraphicsBackend T>
        void lrenderer<T>::load_default_shaders()
        {
            /*
               shader::shader shader;
               mmanger.create_default_render_pass();
               shader.add_program("src/defaults/compiled_shaders/default.vert.spv", backend::ShaderStage::Vertex);
               shader.add_program("src/defaults/compiled_shaders/default.frag.spv", backend::ShaderStage::Fragment);
               mmanger.create_shader(shader);
               mmanger.create_default_graphics_pipeline();
               */
        }
    template class lrenderer<GraphicsBackend::Vulkan>;
}}
