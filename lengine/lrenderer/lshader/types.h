#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

namespace lina { namespace shader {
enum class ParseMode 
    {
        None,
        ps,
        uniform,
    };
    enum class ShaderStage
    {
        None = 0,
        Vertex =  0x00000001,
        Fragment = 0x00000010,
        All = 0x7FFFFFFF,
    };
    struct PushConstant
    {
        ShaderStage stage;   
        u32 offset;
        u32 size;
    };
    struct Program 
    {
        ShaderStage stage;
        std::vector<char> program;
    };
    enum class UniformType
    {
        Static = 6,
        Dynamic = 8,
    };
    struct Uniform
    {
        std::string name = "";
        u32 binding;
        ShaderStage stage;
        UniformType type = UniformType::Static;
        u32 size;
        u32 count = 1;
    };
    struct Texture
    {
        u32 width;
        u32 height;
        void* data;
        u8 stride = 4;
    };
    enum class ErrType
    {
        None = 0,
        File_Not_Found,
        Directory_Create_Fail,
        Component_Not_Present,
        Draw_Mode_Not_Present,
    };

    struct Err 
    {
        ErrType err_type = ErrType::None;
        std::string content = "";
    };

    inline std::ostream& operator<<(std::ostream& os, Err e)
    {
        os << e.content << '\n';
        return os;
    }
}}
#endif


