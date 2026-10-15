#ifndef SHADER_H
#define SHADER_H
#include <unordered_map>
#include <vector>
#include "types.h"
#include <string>
#include "lcpp_utils/lstring.h"
#include "shader_map.h"
namespace lina { namespace shader { 
    
    class shader 
    {
        public:
            i32 parse_and_load(std::string&& src, std::string&& out_folder = "");
            bool load_compiled_program(i32 idx, const char* src, ShaderStage st);
            i32 add_compiled_program(const char* src, ShaderStage st);
            void add_uniform(Uniform&& u);
            void add_texture(Texture&& t);
            void add_ps(PushConstant&& ps);
            void combine_uniforms();
            inline u32 get_binding_count() const;
            inline u32 get_uniform_count() const;
            inline u32 get_static_count() const;
            inline u32 get_dynamic_count() const;
            inline u32 get_ps_count() const;
            const inline std::vector<Program>& get_programs() const;
            const inline std::vector<PushConstant>& get_ps() const;
            const inline std::vector<Uniform>& get_uniforms() const;
            void parse_file(const char*);
        private:
            std::vector<Program> mprograms;
            std::vector<PushConstant> m_push_constants;
            std::vector<Uniform> m_uniforms;
            std::vector<Uniform> m_static_uniforms;
            std::vector<Uniform> m_dynamic_uniforms;
            std::vector<Texture> m_textures;
            std::unordered_map<std::string, u32> m_type_size_map;
    };
    inline u32 shader::get_binding_count()const {return m_static_uniforms.size() + m_dynamic_uniforms.size();}
    inline u32 shader::get_uniform_count()const {return m_static_uniforms.size() + m_dynamic_uniforms.size();}
    inline u32 shader::get_ps_count() const {return m_push_constants.size();};
    const inline std::vector<Program>& shader::get_programs() const {return mprograms;}
    const inline std::vector<PushConstant>& shader::get_ps()const {return m_push_constants;}
    const inline std::vector<Uniform>& shader::get_uniforms() const {return m_uniforms;};
    inline u32 shader::get_static_count() const {return m_static_uniforms.size();};
    inline u32 shader::get_dynamic_count() const {return m_dynamic_uniforms.size();};
}}
#endif

