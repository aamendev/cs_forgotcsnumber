#ifndef LSHADER_MAP_H
#define LSHADER_MAP_H
#include <unordered_map>
#include <string>
#include "types.h"
#include "lcpp_utils/lstring.h"
namespace lina { namespace shader {
    struct shader_code
    {
        std::string before_main;
        std::string in_main;
        ShaderStage stage;
    };
    inline std::unordered_map<std::string, u32> g_shader_type_size
    {
        {"mat4", 4 * 4 * sizeof(float)},
            {"mat3", 3 * 3 * sizeof(float)},
            {"mat2", 2 * 2 * sizeof(float)},
            {"float", sizeof(float)},
            {"vec2", 2 * sizeof(float)},
            {"vec3", 3 * sizeof(float)},
            {"vec4", 4 * sizeof(float)},
            {"ivec2", 2 * sizeof(int)},
            {"ivec3", 3 * sizeof(int)},
            {"ivec4", 4 * sizeof(int)},
            {"uvec2", 2 * sizeof(unsigned int)},
            {"uvec3", 3 * sizeof(unsigned int)},
            {"uvec4", 4 * sizeof(unsigned int)},
            {"dvec4", 2 * 4 * sizeof(float)},
            {"dvec3", 2 * 3 * sizeof(float)},
            {"dvec2", 2 * 2 * sizeof(float)},
            {"double", 2 * sizeof(float)},
            {"dmat4", 2 * 4 * 4 * sizeof(float)},
            {"dmat3", 2 * 3 * 3 * sizeof(float)},
            {"dmat2", 2 * 2 * 2 * sizeof(float)},
            {"double", sizeof(double)},
            {"int", sizeof(int)},
            {"uint", sizeof(unsigned int)},
            {"bool", sizeof(bool)},
    };
    inline std::unordered_map<std::string, shader_code> g_component_code
    {
        {
            "FULL_SCREEN",
                {
                    "#version 450\n"
                        "vec2 positions[3] = vec2[](\n"
                        "    vec2(-1.0, -1.0),\n"
                        "    vec2( 3.0, -1.0),\n"
                        "    vec2(-1.0,  3.0)\n"
                        ");\n",
                    "gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);",
                    ShaderStage::Vertex
                }
        },
            {
                "DEFAULT_VERTEX",
                {
                    "#version 450\n"
                        "layout(location={IN_LOC})in vec4 lshader_in_position;\n"
                        "layout(push_constant) uniform SHADER_MODEL_VIEW\n"
                        "{\n"
                        "    mat4 model;\n"
                        "    mat4 view;\n"
                        "} shader_model_view;\n"
                        "layout(binding={BINDING}) uniform SHADER_PROJECTION\n"
                        "{\n"
                        "    mat4 proj;\n"
                        "} shader_proj;\n"
                        ,
                    "gl_Position = (shader_proj.proj * shader_model_view.view * shader_model_view.model) * lshader_in_position;\n",
                    ShaderStage::Vertex
                }
            },
            {
                "DEFAULT_FRAG",
                {
                    "#version 450\n"
                        "layout(location={OUT_LOC}) out vec4 frag_col;\n",
                    "frag_col = vec4(255./255., 186./255., 222./255., 1.);\n",
                    ShaderStage::Fragment
                }
            },
            {
                "colour", 
                {
                    "layout(binding={BINDING}) uniform LSHADER_COL\n"
                        "{\n"
                        "vec4 col;\n"
                        "\n} lshader_col;",
                    "frag_col = lshader_col.col;\n",
                    ShaderStage::Fragment
                }
            },
            {
                "phong_frag",
                {
                    cpp_utils::string::load_file_to_string("./utility_shaders/phong_before_main.frag"),
                    cpp_utils::string::load_file_to_string("./utility_shaders/phong_main.frag"),
                    ShaderStage::Fragment
                },
            },
            {
                "phong_vert",
                {
                    cpp_utils::string::load_file_to_string("./utility_shaders/phong_before_main.vert"),
                    cpp_utils::string::load_file_to_string("./utility_shaders/phong_main.vert"),
                    ShaderStage::Vertex
                }
            }
    };
}}
#endif
