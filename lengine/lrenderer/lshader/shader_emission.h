#ifndef SHADER_EMISSION_H
#define SHADER_EMISSION_H
#include "shader.h"
#include "shader_map.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include "utils.h"
#define GLOBAL_REPLACE_OFFSET (2)
namespace lina { namespace shader {
    struct replace_label 
    {
        int idx = -1;
        std::string&& name = "INJECT";
    };
    struct injection_data
    {
            std::unordered_map<std::string, std::vector<std::string>> injection_code;
            std::unordered_map<std::string, std::unordered_map<std::string, std::string>> injection_area;
            std::unordered_map<std::string, int> global_offset;
            std::unordered_map<std::string, std::string> global_array_names;
            std::string curr_inject_component = "NONE";
            inline void clear(std::string&& comp)
            {
                injection_code.erase(comp);
                injection_area.erase(comp);
                global_offset.erase(comp);
            }
            inline void clear()
            {
                injection_area.clear();
                injection_code.clear();
                global_offset.clear();
                curr_inject_component = "NONE";
            }
    };
    class emission
    {
        public:
            emission();
            Err add_component_from_map(std::string&& component);
            Err add_component_inplace(ShaderStage shader_stage, std::string&& content, bool before_main);
            Err write_shader_to_file(std::string&& file_name, std::string&& out_folder);



            Err set_injection_mode(std::string&& comp, int idx, std::string&& val);
            Err inject(std::string&& comp, int inject_mode, replace_label&& replace_label = {});
            Err inject(std::string&& comp, std::string&& inject_string, replace_label&& replace_label = {});
            Err submit_injections(std::string&& comp);

            // useful for repeated applications of above three functions
            inline void set_inject_component(std::string&& component);
            inline Err set_injection_mode(int idx, std::string&& val);
            inline Err inject(int inject_mode, replace_label&& replace_label = {});
            inline Err inject(std::string&& inject_str, replace_label&& replace_label = {});
            inline Err submit_injections();
            //
            inline void clear_injections(std::string&& comp);
            inline void clear_injections();

            inline void set_global_array_name(std::string&& comp, std::string&& val);
        public:
            std::unordered_set<std::string> m_present_components;
            std::array<std::pair<std::string, std::string>, utils::g_max_stage_count> m_content;
            std::array<std::array<int, 2>, utils::g_max_stage_count> m_local_replace_variables;
            std::array<int, 2> m_global_replace_variables;
            injection_data m_injection_data;
    };
    inline void emission::set_global_array_name(std::string&& comp, std::string&& val)
    {
        m_injection_data.global_array_names[comp] = val;
    }
    inline void emission::set_inject_component(std::string&& component)
    {
        m_injection_data.curr_inject_component = component;
    }
    inline Err emission::set_injection_mode(int idx, std::string&& val)
    {
        return set_injection_mode(std::move(m_injection_data.curr_inject_component),idx, std::move(val));
    }
    inline Err emission::inject(int inject_mode, replace_label&& replace_lable)
    {
        return inject(std::move(m_injection_data.curr_inject_component), inject_mode, std::move(replace_lable));
    }
    inline Err emission::inject(std::string&& inject_str, replace_label&& replace_label)
    {
        return inject(std::move(m_injection_data.curr_inject_component),
                std::move(inject_str), std::move(replace_label));
    }
    inline Err emission::submit_injections()
    {
        return submit_injections(std::move(m_injection_data.curr_inject_component));
    }
    inline void emission::clear_injections(std::string&& comp)
    {
        m_injection_data.clear(std::move(comp));
    }
    inline void emission::clear_injections()
    {
        m_injection_data.clear();
    }
}}
#endif
