#ifndef BACKEND_RESOURCE_MANAGER_H
#define BACKEND_RESOURCE_MANAGER_H
#include <vector>
#include "types.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <iostream>
namespace lina { namespace renderer { namespace backend {
    class manager;
    namespace buffers 
    {
        class uniform;
    }
    struct shader_offset
    {
        i64 stage_info= -1;
        i64 ps_range = -1;
        i64 ub = -1;
        i64 dset_layout = -1;
        i64 dpool = -1;
        i64 dset = -1;
        i64 pipeline_layout = -1;
    };
    struct vk_shader_resources
    {
        std::vector<VkPipelineShaderStageCreateInfo> shader_stage_infos;
        std::vector<VkPushConstantRange> ps_ranges;
        std::vector<buffers::uniform*> ub;
        std::vector<VkDescriptorSetLayout> dset_layout;
        std::vector<VkDescriptorPool> dpool;
        std::vector<VkDescriptorSet> dset;
        std::vector<VkPipelineLayout> pipeline_layout;
    };
    struct vk_pipeline_resources 
    {
        std::vector<VkPipeline> pipeline;
    };
    class resource_manager
    {
        public:
            VkPipelineShaderStageCreateInfo* get_shader_info_pointer(i64 shader_idx);
            VkPushConstantRange* get_ps_range_pointer(i64 shader_idx);
            VkPipelineVertexInputStateCreateInfo** get_vertex_info_pointer(i64 shader_idx);
            VkDescriptorSetLayout* get_dset_layout_pointer(i64 shader_idx);
            VkDescriptorPool* get_dpool_pointer(i64 shader_idx);
            VkDescriptorSet* get_dset_pointer(i64 shader_idx);
            buffers::uniform** get_ub_pointer(i64 shader_idx);
            VkPipelineLayout* get_pipeline_layout_pointer(i64 shader_idx);
            VkPipeline* get_pipeline_pointer(i64 shader_idx);
            inline void add_shader_ps(VkPushConstantRange& ps);
            inline void add_shader_stage_info(VkPipelineShaderStageCreateInfo& info);
            inline void add_vertex_input_info(VkPipelineVertexInputStateCreateInfo* info);
            void add_ub(buffers::uniform* buffer);
            inline void add_dset_layout(VkDescriptorSetLayout& layout);
            inline void add_dpool(VkDescriptorPool& pool);
            void add_dsets(VkDescriptorSetAllocateInfo& dset, manager* m);
            inline void add_pipeline_layout(VkPipelineLayout& layout);
            inline void add_pipeline(VkPipeline& layout);
        private:
            std::vector<i64> m_vertex_offsets;
            std::vector<VkPipelineVertexInputStateCreateInfo*> m_vert_input_infos;
            std::vector<shader_offset> m_shader_offsets;
            vk_shader_resources m_shader_resrouces;
            vk_pipeline_resources m_pipeline_resources;
            std::vector<i64> m_pipeline_offsets;
    };
    inline void resource_manager::add_shader_ps(VkPushConstantRange& ps)
    {
        m_shader_resrouces.ps_ranges.push_back(ps);
    }
    inline void resource_manager::add_shader_stage_info(VkPipelineShaderStageCreateInfo& info)
    {
        m_shader_resrouces.shader_stage_infos.push_back(info);
    }
    inline void resource_manager::add_vertex_input_info(VkPipelineVertexInputStateCreateInfo* info)
    {
        m_vert_input_infos.push_back(info);
    }
    inline void resource_manager::add_dset_layout(VkDescriptorSetLayout& layout)
    {
        m_shader_resrouces.dset_layout.push_back(layout);
    }
    inline void resource_manager::add_dpool(VkDescriptorPool& pool)
    {
        m_shader_resrouces.dpool.push_back(pool);
    }
    inline void resource_manager::add_pipeline_layout(VkPipelineLayout& layout)
    {
        m_shader_resrouces.pipeline_layout.push_back(layout);
    }
    inline void resource_manager::add_pipeline(VkPipeline& pline)
    {
        m_pipeline_resources.pipeline.push_back(pline);
    }
    
}}}
#endif
