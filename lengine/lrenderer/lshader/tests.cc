#include "tests.h"
#include "lcpp_utils/lstring.h"
#include "shader.h"
#include "shader_emission.h"
#include "shader_map.h"
#include "types.h"
#include <system_error>
namespace lina { namespace shader { namespace tests {
    void parsing()
    {
        BEGIN_TEST("parsing");
        shader sh;
        sh.parse_file("./tests_shaders/parse1.vert");
        sh.combine_uniforms();
        auto un = sh.get_uniforms();
        auto pss = sh.get_ps();
        assert(un.size() == 0);
        assert(pss.size() == 1);
        auto ps1 = pss[0];
        assert(ps1.size == (4 * 4 * sizeof(f32) * 2));
        assert(ps1.offset == 0);
        assert(ps1.stage == ShaderStage::Vertex);

        shader sh2;
        sh2.parse_file("./tests_shaders/parse_offset.vert");
        sh2.combine_uniforms();
        un = sh2.get_uniforms();
        pss = sh2.get_ps();
        assert(un.size() == 0);
        assert(pss.size() == 1);
        ps1 = pss[0];
        assert(ps1.size == (4 * 4 * sizeof(f32) * 2));
        assert(ps1.offset == 16);
        assert(ps1.stage == ShaderStage::Vertex);


        shader sh3;
        sh3.parse_file("./tests_shaders/parse_spaces.vert");
        sh3.combine_uniforms();
        un = sh3.get_uniforms();
        pss = sh3.get_ps();
        assert(un.size() == 0);
        assert(pss.size() == 1);
        ps1 = pss[0];
        assert(ps1.size == (4 * 4 * sizeof(f32) * 2));
        assert(ps1.offset == 0);
        assert(ps1.stage == ShaderStage::Vertex);

        shader sh4;
        sh4.parse_file("./tests_shaders/parse_uniforms.frag");
        sh4.combine_uniforms();
        un = sh4.get_uniforms();
        pss = sh4.get_ps();
        assert(un.size() == 1);
        assert(pss.size() == 1);
        ps1 = pss[0];
        auto un1 = un[0];
        assert(ps1.size == (3 * sizeof(float)));
        assert(ps1.offset == 0);
        assert(ps1.stage == ShaderStage::Fragment);

        assert(un1.stage == ShaderStage::Fragment);
        assert(un1.binding == 5);
        assert(un1.count == 1);
        assert(un1.type == UniformType::Static);
        END_TEST("parsing");
    }
    void parsing_and_compiling()
    {
        BEGIN_TEST("parsing and compiling");
        shader sh;
        sh.parse_and_load("./tests_shaders/parse1.vert", "compiled_shaders");
        sh.combine_uniforms();
        auto un = sh.get_uniforms();
        auto pss = sh.get_ps();
        assert(un.size() == 0);
        assert(pss.size() == 1);
        auto ps1 = pss[0];
        assert(ps1.size == (4 * 4 * sizeof(f32) * 2));
        assert(ps1.offset == 0);
        assert(ps1.stage == ShaderStage::Vertex);

        shader sh2;
        sh2.parse_and_load("./tests_shaders/parse_offset.vert", "compiled_shaders");
        sh2.combine_uniforms();
        un = sh2.get_uniforms();
        pss = sh2.get_ps();
        assert(un.size() == 0);
        assert(pss.size() == 1);
        ps1 = pss[0];
        assert(ps1.size == (4 * 4 * sizeof(f32) * 2));
        assert(ps1.offset == 16);
        assert(ps1.stage == ShaderStage::Vertex);


        shader sh3;
        sh3.parse_and_load("./tests_shaders/parse_spaces.vert");
        sh3.combine_uniforms();
        un = sh3.get_uniforms();
        pss = sh3.get_ps();
        assert(un.size() == 0);
        assert(pss.size() == 1);
        ps1 = pss[0];
        assert(ps1.size == (4 * 4 * sizeof(f32) * 2));
        assert(ps1.offset == 0);
        assert(ps1.stage == ShaderStage::Vertex);

        shader sh4;
        sh4.parse_and_load("./tests_shaders/parse_uniforms.frag");
        sh4.combine_uniforms();
        un = sh4.get_uniforms();
        pss = sh4.get_ps();
        assert(un.size() == 1);
        assert(pss.size() == 1);
        ps1 = pss[0];
        auto un1 = un[0];
        assert(ps1.size == (3 * sizeof(float)));
        assert(ps1.offset == 0);
        assert(ps1.stage == ShaderStage::Fragment);

        assert(un1.stage == ShaderStage::Fragment);
        assert(un1.binding == 5);
        assert(un1.count == 1);
        assert(un1.type == UniformType::Static);
        END_TEST("parsing and compiling");
    }
    void shader_emission()
    {
        BEGIN_TEST("emission");
        emission em;
        em.write_shader_to_file("default", "./test_out");
        std::string vertex_shader = cpp_utils::string::load_file_to_string("test_out/default.vert");
        std::string fragment_shader = cpp_utils::string::load_file_to_string("test_out/default.frag");

        assert(vertex_shader == 
                "#version 450\n"
                "layout(location=0)in vec4 lshader_in_position;\n"
                "layout(push_constant) uniform SHADER_MODEL_VIEW\n"
                "{\n"
                "    mat4 model;\n"
                "    mat4 view;\n"
                "} shader_model_view;\n"
                "layout(binding=0) uniform SHADER_PROJECTION\n"
                "{\n"
                "    mat4 proj;\n"
                "} shader_proj;\n"
                "\nvoid main()\n{\n"
                "gl_Position = (shader_proj.proj * shader_model_view.view * shader_model_view.model) * lshader_in_position;\n"
                "\n}"
              );
        assert(fragment_shader == 
                "#version 450\n"
                "layout(location=0) out vec4 frag_col;\n"
                "\nvoid main()\n{\n"
                "frag_col = vec4(255./255., 186./255., 222./255., 1.);\n"
                "\n}"
              );
        emission em2;
        em2.add_component_from_map("colour");
        em2.write_shader_to_file("colour", "./test_out");
        vertex_shader = cpp_utils::string::load_file_to_string("test_out/colour.vert");
        fragment_shader = cpp_utils::string::load_file_to_string("test_out/colour.frag");

        assert(vertex_shader == 
                "#version 450\n"
                "layout(location=0)in vec4 lshader_in_position;\n"
                "layout(push_constant) uniform SHADER_MODEL_VIEW\n"
                "{\n"
                "    mat4 model;\n"
                "    mat4 view;\n"
                "} shader_model_view;\n"
                "layout(binding=0) uniform SHADER_PROJECTION\n"
                "{\n"
                "    mat4 proj;\n"
                "} shader_proj;\n"
                "\nvoid main()\n{\n"
                "gl_Position = (shader_proj.proj * shader_model_view.view * shader_model_view.model) * lshader_in_position;\n"
                "\n}"
              );
        assert(fragment_shader == 
                "#version 450\n"
                "layout(location=0) out vec4 frag_col;\n"
                "layout(binding=1) uniform LSHADER_COL\n"
                "{\n"
                "vec4 col;\n"
                "\n} lshader_col;"
                "\nvoid main()\n{\n"
                "frag_col = vec4(255./255., 186./255., 222./255., 1.);\n"
                "frag_col = lshader_col.col;\n"
                "\n}"
              );
        END_TEST("emission");
    }
    void shader_injection()
    {
        BEGIN_TEST("injection");
        emission em;
        g_component_code["draw_stuff"].before_main = cpp_utils::string::load_file_to_string("tests_shaders/draw_stuff_before_main.frag");
        g_component_code["draw_stuff"].in_main = cpp_utils::string::load_file_to_string("tests_shaders/draw_stuff_main.frag");
        g_component_code["draw_stuff"].stage = ShaderStage::Fragment;

        std::cout <<em.add_component_from_map("draw_stuff");
        std::cout <<em.set_injection_mode("draw_stuff", 0, "drawing stuff....");
        std::cout << em.set_injection_mode("draw_stuff", 1, "draw_circle(in_uv, vec2(data[0], data[1]), data[2]);");
        std::cout << em.set_injection_mode("draw_stuff", 2, "draw_box(in_uv, vec2(data[0], data[1]), vec2(data[2], data[3]));");

        em.set_global_array_name("draw_stuff", "data");

        em.inject("draw_stuff", 2);
        em.inject("draw_stuff", 1);
        em.inject("draw_stuff", 0, {.name="DRAW"});
        em.submit_injections("draw_stuff");
        em.write_shader_to_file("draw_stuff", "./test_out");
        auto fragment_shader = cpp_utils::string::load_file_to_string("test_out/draw_stuff.frag");

        assert(fragment_shader == 
                "#version 450\n"
                "layout(location=0) out vec4 frag_col;\n"
                "float sdCircle(vec2 p, float r, vec2 offset)\n"
                "{\n"
                "    return length(p-offset)-r;\n"
                "}\n"
                "float sdBox(vec2 p, vec2 b)\n"
                "{\n"
                "    vec2 d = abs(p)-b;\n"
                "    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);\n"
                "}\n"
                "vec4 draw_circle(vec2 p, vec2 center, float r)\n"
                "{\n"
                "    float v = sdCircle(p, r, center);\n"
                "    drawing stuff....\n"
                "\n"
                "}\n"
                "vec4 draw_box(vec2 p, vec2 center, float r)\n"
                "{\n"
                "    float v = sdBox(p - center, r);\n"
                "    drawing stuff....\n"
                "\n"
                "}\n"
                "\n"
                "layout(binding=1) uniform draw_stuff_LSHADER_GLOBAL_ARR\n"
                "{\n"
                "float[7] data;\n"
                "};\n"
                "\n"
                "void main()\n"
                "{\n"
                "frag_col = vec4(255./255., 186./255., 222./255., 1.);\n"
                "vec2 in_uv = (2.0*fragCoord-iResolution.xy)/iResolution.y;\n"
                "vec4 col;\n"
                "draw_box(in_uv, vec2(data[0+0], data[1+0]), vec2(data[2+0], data[3+0]));\n"
                "draw_circle(in_uv, vec2(data[0+4], data[1+4]), data[2+4]);\n"
                "\n"
                "fragColor = col;\n"
                "\n"
                "}");
        END_TEST("injection");
    }
    void all()
    {
        parsing();
        parsing_and_compiling();
        shader_emission();
        shader_injection();
    }
}}}
