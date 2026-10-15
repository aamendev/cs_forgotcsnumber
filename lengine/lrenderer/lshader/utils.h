#ifndef LSHADER_UTILS_H
#define LSHADER_UTILS_H
#include "types.h"
#include <string>
#include <array>

namespace lina { namespace shader { namespace utils{
    inline constexpr int g_max_stage_count = 2;
    inline std::array<std::string, g_max_stage_count> g_shader_idx_name{"vert", "frag"};
    inline std::string stage_to_str(ShaderStage st)
    {
        switch (st)
        {
            case ShaderStage::Vertex:
            {
                return "vert";
            }
            break;
            case ShaderStage::Fragment:
            {
                return "frag";
            }
            break;
            default:{return "";}
        }
    }
    inline int stage_to_index(ShaderStage st)
    {
        switch (st)
        {
            case ShaderStage::Vertex:
            {
                return 0;
            }
            break;
            case ShaderStage::Fragment:
            {
                return 1;
            }
            break;
            default:{return -1;}
        }
    }
}}}
#endif
