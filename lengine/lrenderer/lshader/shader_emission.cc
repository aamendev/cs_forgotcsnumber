#include "shader_emission.h"
#include "lcpp_utils/lstring.h"
#include "shader_map.h"
#include "types.h"
#include "utils.h"
#include <filesystem>
#include <iostream>
#include <string>

namespace lina { namespace shader {
    emission::emission()
    {
        m_global_replace_variables = {0, 0};
        for (auto& p : m_global_replace_variables)
        {
           p = 0; 
        }
        for (auto& p : m_local_replace_variables)
        {
           p[0] = 0; 
           p[1] = 0; 
        }
    }
    Err emission::add_component_from_map(std::string&& component)
    {
        if (g_component_code.contains(component) && !m_present_components.contains(component))[[likely]]
        {
            m_present_components.insert(component);
            auto str = g_component_code[component];
            std::array<int, 4> incs = 
            {
                cpp_utils::string::count_occurences(str.before_main, "{BINDING}"),
                cpp_utils::string::count_occurences(str.before_main,"{OFFSET}"),
                cpp_utils::string::count_occurences(str.before_main,"{IN_LOC}"),
                cpp_utils::string::count_occurences(str.before_main,"{OUT_LOC}"),
            };
            auto before = cpp_utils::string::replace_all("{BINDING}", std::to_string(m_global_replace_variables[0]), str.before_main);
            cpp_utils::string::replace_all_inplace("{OFFSET}", std::to_string(m_global_replace_variables[1]), before);

            cpp_utils::string::replace_all_inplace("{IN_LOC}",
                    std::to_string(m_local_replace_variables[utils::stage_to_index(str.stage)][0]), 
                    before);
            cpp_utils::string::replace_all_inplace("{OUT_LOC}",
                    std::to_string(m_local_replace_variables[utils::stage_to_index(str.stage)][1]), before);

            m_content[utils::stage_to_index(str.stage)].first += before; 
            m_content[utils::stage_to_index(str.stage)].second += str.in_main; 

            for (int i = 0; i < m_local_replace_variables[utils::stage_to_index(str.stage)].size(); i++)
            {
                m_local_replace_variables[utils::stage_to_index(str.stage)][i] += incs[i+GLOBAL_REPLACE_OFFSET];
            }
            for (int i = 0; i < m_global_replace_variables.size(); i++)
            {
                m_global_replace_variables[i] += incs[i];
            }
            return Err{.content="success"};
        }
        return Err{ErrType::Component_Not_Present, "component not found in g_component_code, check name or add it to table"};
    }
    Err emission::add_component_inplace(ShaderStage shader_stage, std::string&& content, bool before_main)
    {
        return Err{.content = "function not provided yet"};
    }
    Err emission::write_shader_to_file(std::string&& file_name, std::string&& out_folder)
    {
        std::string main_part = out_folder + "/" + file_name;
        std::string path;
        bool has_path = file_name.contains('.');
        if (!out_folder.empty())
        {
            std::filesystem::create_directory(out_folder);
        }
        int i = 0;
        for (auto [before, after] : m_content)
        {
            path = main_part;
            if (!has_path)[[likely]]
            {
                path += ("." + utils::g_shader_idx_name[i++]);
            }
            std::string total_content = before + "\nvoid main()\n{\n" + after + "\n}";
            cpp_utils::string::write_string_to_file(std::move(total_content), std::move(path));
        }
        return Err{.content="success"};
    }
    Err emission::set_injection_mode(std::string&& comp, int idx, std::string&& val)
    {
        if (!m_present_components.contains(comp))[[unlikely]]
        {
            return Err{ErrType::Component_Not_Present, "component not added, consider calling add_component_from_map"};
        }
        if (idx >= m_injection_data.injection_code[comp].size())[[unlikely]]
        {
            m_injection_data.injection_code[comp].resize(idx + 1);
        }
        m_injection_data.injection_code[comp][idx] = val;
        return Err{.content="success"};
    }
    Err emission::inject(std::string&& comp, std::string&& inject_string, replace_label&& rep)
    {
        std::string label = rep.name;
        if (rep.idx != -1)
        {
            label += std::to_string(rep.idx);
        }
        int old_offset = m_injection_data.global_offset[comp];
        int new_offset = m_injection_data.global_offset[comp];
        m_injection_data.injection_area[comp][label] += cpp_utils::string::capture_and_replace(inject_string, 
                [](std::string s)
                {
                return (s.size() == 1 && s[0] == '[') || (s.size() > 1 && s[0] == '[' && s[s.size() - 1] != ']');
                }, 
                [old_offset, &new_offset](std::string::const_iterator begin, std::string::const_iterator end)mutable
                {
                new_offset++;
                std::string new_str(begin, end);
                auto last = new_str.back();
                new_str.pop_back();
                new_str.push_back('+');
                new_str += std::to_string(old_offset);
                new_str.push_back(last);
                return new_str;
                });
        m_injection_data.injection_area[comp][label].push_back('\n');
        m_injection_data.global_offset[comp] = new_offset;
        return Err{.content="success"};
    }
    Err emission::inject(std::string&& comp, int draw_mode, replace_label&& rep)
    {
        if (!m_present_components.contains(comp))[[unlikely]]
        {
            return Err{ErrType::Component_Not_Present, "component not added, consider calling add_component_from_map"};
        }
        if (!m_injection_data.injection_code.contains(comp))[[unlikely]]
        {
            return Err{.err_type=ErrType::Component_Not_Present, .content=
                "injection component has no injection code set or is wrong." 
                "either call set_injection_mode on it or call "
                "set_inject_component(comp) with desired component if not using inject(comp, draw_mode)"};
        }
        if (draw_mode >= m_injection_data.injection_code[comp].size())[[unlikely]]
        {
            return Err{.err_type=ErrType::Draw_Mode_Not_Present, 
                .content=
                    "draw mode " + std::to_string(draw_mode) +
                " is greater than provided injection code counts " + std::to_string(m_injection_data.injection_code.size()) + ".Consider adding them or biasing the draw_mode"};
        }
        auto& cg = m_injection_data.injection_code[comp][draw_mode];
        return inject(std::move(comp), std::move(cg), std::move(rep));
    }
    Err emission::submit_injections(std::string&& comp)
    {
        if (!m_present_components.contains(comp))[[unlikely]]
        {
            return Err{ErrType::Component_Not_Present, "component not added, consider calling add_component_from_injection_data.map"};
        }
        if (m_injection_data.injection_area[comp].empty())
        {
            return Err{ErrType::Component_Not_Present, "no injections were added to this component. consider calling inject"};
        }
        auto content_idx = utils::stage_to_index(g_component_code[comp].stage);
        if (m_injection_data.global_array_names.contains(comp))
        {
            m_content[content_idx].first += "\nlayout(std430, binding="+std::to_string(m_global_replace_variables[0])+") uniform " + comp + "_LSHADER_GLOBAL_ARR\n"
                "{\n"
                "float["+std::to_string(m_injection_data.global_offset[comp])+"] " + m_injection_data.global_array_names[comp] + ";\n"
                "};\n";
        }

        for (const auto& [replace_name, replace_code] : m_injection_data.injection_area[comp])
        {
            std::string rep_str = replace_code;
            auto rep_name = "{" +replace_name + "}";
            cpp_utils::string::replace_all_inplace(std::move(rep_name), std::move(rep_str), m_content[content_idx].second);
            cpp_utils::string::replace_all_inplace(std::move(rep_name), std::move(rep_str), m_content[content_idx].first);
        }
        return Err{.content = "success"};
    }
}}
