#include "resource_manager.h"
#include "window.h"
#include "backend_manager.h"
namespace lina { namespace renderer { namespace backend {
    void resource_manager::add_ub(buffers::uniform* buf)
    {
        m_shader_resrouces.ub.push_back(buf);
    }
    VkPipelineShaderStageCreateInfo* resource_manager::get_shader_info_pointer(i64 shader_idx)
    {
        if (shader_idx >= m_shader_offsets.size())
        {
            m_shader_offsets.push_back((shader_offset){.stage_info = (i64)m_shader_resrouces.shader_stage_infos.size()});
            return nullptr;
        }
        else if (m_shader_offsets[shader_idx].stage_info == -1)
        {
            m_shader_offsets[shader_idx].stage_info = m_shader_resrouces.shader_stage_infos.size();
            return nullptr;
        }
        return &m_shader_resrouces.shader_stage_infos[m_shader_offsets[shader_idx].stage_info];
    }
    VkPushConstantRange* resource_manager::get_ps_range_pointer(i64 shader_idx)
    {
        if (shader_idx >= m_shader_offsets.size())
        {
            m_shader_offsets.push_back((shader_offset){.ps_range = (i64)m_shader_resrouces.ps_ranges.size()});
            return nullptr;
        }
        else if (m_shader_offsets[shader_idx].ps_range == -1)
        {
            m_shader_offsets[shader_idx].ps_range = m_shader_resrouces.ps_ranges.size();
            return nullptr;
        }
        return &m_shader_resrouces.ps_ranges[m_shader_offsets[shader_idx].ps_range];
    }
    VkPipelineVertexInputStateCreateInfo** resource_manager::get_vertex_info_pointer(i64 vertex_idx)
    {
        if (vertex_idx >= m_vertex_offsets.size())
        {
            m_vertex_offsets.push_back(m_vert_input_infos.size());
            return nullptr;
        }
        return &m_vert_input_infos[m_vertex_offsets[vertex_idx]];
    }
    VkDescriptorSetLayout* resource_manager::get_dset_layout_pointer(i64 shader_idx)
    {
        if (shader_idx >= m_shader_offsets.size())
        {
            m_shader_offsets.push_back((shader_offset){.dset_layout = (i64)m_shader_resrouces.dset_layout.size()});
            return nullptr;
        }
        else if (m_shader_offsets[shader_idx].dset_layout == -1 || m_shader_offsets[shader_idx].dset_layout >= m_shader_resrouces.dset_layout.size())
        {
            m_shader_offsets[shader_idx].dset_layout = m_shader_resrouces.dset_layout.size();
            return nullptr;
        }
        return &m_shader_resrouces.dset_layout[m_shader_offsets[shader_idx].dset_layout];
    }
    VkDescriptorSet* resource_manager::get_dset_pointer(i64 shader_idx)
    {
        if (shader_idx >= m_shader_offsets.size())
        {
            m_shader_offsets.push_back((shader_offset){.dset = (i64)m_shader_resrouces.dset.size()});
            return nullptr;
        }
        else if (m_shader_offsets[shader_idx].dset == -1 || m_shader_offsets[shader_idx].dset >= m_shader_resrouces.dset.size())
        {
            m_shader_offsets[shader_idx].dset = m_shader_resrouces.dset.size();
            return nullptr;
        }
        return &m_shader_resrouces.dset[m_shader_offsets[shader_idx].dset];
    }
    VkDescriptorPool* resource_manager::get_dpool_pointer(i64 shader_idx)
    {
        if (shader_idx >= m_shader_offsets.size())
        {
            m_shader_offsets.push_back((shader_offset){.dpool = (i64)m_shader_resrouces.dpool.size()});
            return nullptr;
        }
        else if (m_shader_offsets[shader_idx].dpool == -1 || m_shader_offsets[shader_idx].dpool >= m_shader_resrouces.dpool.size())
        {
            m_shader_offsets[shader_idx].dpool = m_shader_resrouces.dpool.size();
            return nullptr;
        }
        return &m_shader_resrouces.dpool[m_shader_offsets[shader_idx].dpool];
    }

    buffers::uniform** resource_manager::get_ub_pointer(i64 shader_idx)
    {
        if (shader_idx >= m_shader_offsets.size())
        {
            m_shader_offsets.push_back((shader_offset){.ub = (i64)m_shader_resrouces.ub.size()});
            return nullptr;
        }
        else if (m_shader_offsets[shader_idx].ub == -1 || m_shader_offsets[shader_idx].ub >= m_shader_resrouces.ub.size())
        {
            m_shader_offsets[shader_idx].ub = m_shader_resrouces.ub.size();
            return nullptr;
        }
        return &m_shader_resrouces.ub[m_shader_offsets[shader_idx].ub];
    }
    VkPipelineLayout* resource_manager::get_pipeline_layout_pointer(i64 shader_idx)
    {
        if (shader_idx >= m_shader_offsets.size())
        {
            m_shader_offsets.push_back((shader_offset){.pipeline_layout = (i64)m_shader_resrouces.pipeline_layout.size()});
            return nullptr;
        }
        else if (m_shader_offsets[shader_idx].pipeline_layout == -1 || m_shader_offsets[shader_idx].pipeline_layout >= m_shader_resrouces.pipeline_layout.size())
        {
            m_shader_offsets[shader_idx].pipeline_layout = m_shader_resrouces.pipeline_layout.size();
            return nullptr;
        }
        return &m_shader_resrouces.pipeline_layout[m_shader_offsets[shader_idx].pipeline_layout];
    }
    VkPipeline* resource_manager::get_pipeline_pointer(i64 pipeline_idx)
    {
        if (pipeline_idx >= m_pipeline_offsets.size())
        {
            m_pipeline_offsets.push_back(m_pipeline_resources.pipeline.size());
            return nullptr;
        }
        return &m_pipeline_resources.pipeline[m_pipeline_offsets[pipeline_idx]];
    }

    void resource_manager::add_dsets(VkDescriptorSetAllocateInfo& dset_info, manager* mang)
    {
        auto oldsz = m_shader_resrouces.dset.size();
        auto new_sz = oldsz +  dset_info.descriptorSetCount;
        m_shader_resrouces.dset.resize(new_sz);
        if(vkAllocateDescriptorSets(
                   mang->mvkdevice, 
                    &dset_info,
                    &m_shader_resrouces.dset[oldsz]) != VK_SUCCESS)
            std::cerr<<"Failed to allocate sets\n";
    }
}}}
