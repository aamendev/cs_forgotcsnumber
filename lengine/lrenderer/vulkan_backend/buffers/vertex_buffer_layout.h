#ifndef BACKEND_VERTEX_BUFFER_LAYOUT_H
#define BACKEND_VERTEX_BUFFER_LAYOUT_H
#include "types.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
namespace lina{ namespace renderer{ namespace backend {
    enum class Format
    {
        Float1 = VK_FORMAT_R32_SFLOAT,
        Float2 = VK_FORMAT_R32G32_SFLOAT,
        Float3 = VK_FORMAT_R32G32B32_SFLOAT,
        Float4 = VK_FORMAT_R32G32B32A32_SFLOAT,
        Double1 = VK_FORMAT_R64_SFLOAT,
        Double2 = VK_FORMAT_R64G64_SFLOAT,
        Double3 = VK_FORMAT_R64G64B64_SFLOAT,
        Double4 = VK_FORMAT_R64G64B64A64_SFLOAT,

        Uint1 = VK_FORMAT_R32_UINT,
        Uint2 = VK_FORMAT_R32G32_UINT,
        Uint3 = VK_FORMAT_R32G32B32_UINT
    };
    struct vbelement
    {
        Format format;
        u32 location;
        static inline u32 get_size(Format format)
        {
            switch(format)
            {
                case Format::Float1:
                case Format::Float2:
                case Format::Float3:
                case Format::Float4:
                case Format::Uint1:
                case Format::Uint2:
                case Format::Uint3:
                    return 4;
                    break;
                case Format::Double1:
                case Format::Double2:
                case Format::Double3:
                case Format::Double4:
                    return 8;
                    break;
                default:
                    return 1;
            }
        }
        static inline u32 get_count(Format format)
        {
            switch(format)
            {
            case Format::Float1:
            case Format::Uint1:
            case Format::Double1:
                return 1;
                break;
            case Format::Float2:
            case Format::Uint2:
            case Format::Double2:
                return 2;
                break;
            case Format::Float3:
            case Format::Uint3:
            case Format::Double3:
                return 3;
                break;
            case Format::Float4:
            case Format::Double4:
                return 4;
                break;
            default:
                return 1;
            }
        }
    };
    class vblayout
    {
        public:
            vblayout() : m_stride(0) {}

            void push(Format format, u32 location)
            {
                m_elements.push_back({format, location});
                m_stride += vbelement::get_count(format)
                    * vbelement::get_size(format);
            }

            /* Getters */
            inline std::vector<vbelement> get_elements()
            {
                return m_elements;
            }
            inline u32 get_stride()
            {
                return m_stride;
            }
        private:
            std::vector<vbelement> m_elements;
            u32 m_stride;
    };
}}}
#endif
