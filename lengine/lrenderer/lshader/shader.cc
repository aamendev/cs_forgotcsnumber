#include "shader.h"
#include "lcpp_utils/lstring.h"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include "ExpressionEvaluator/ExpressionEvaluator.h"
#include "ExpressionEvaluator/utils.h"
#include "lcpp_utils/all.h"
namespace lina { namespace shader { 
    i32 shader::parse_and_load(std::string&& src, std::string&& out_folder)
    {
        parse_file(src.c_str());
        auto last_dir = src.find_last_of('/');
        std::string folder(src.begin(), src.begin() + last_dir);
        std::string curr_file = "";
        std::string new_path = "";
        if (last_dir != std::string::npos)
        {
            curr_file = std::string(src.begin() + (last_dir + 1), src.end());
            if (!out_folder.empty())
            {
                std::filesystem::create_directory(folder+  "/" + out_folder);
            }
            new_path = folder + "/" + out_folder + "/" + curr_file + ".spv";
        }
        else 
        {
            curr_file = std::string(src.begin(), src.end());
            if (!out_folder.empty())
            {
                std::filesystem::create_directory(out_folder);
            }
            new_path = out_folder + "/" + curr_file + ".spv";
        }
        std::system(("glslang -V " + src +  " -o " +new_path).c_str());

        auto stuff = cpp_utils::string::split(src, '.');
        auto ext = stuff[stuff.size() - 1];
        bool vert = ext == "vert";
        bool frag = ext == "frag";
        ShaderStage stage = 
            (ShaderStage)((u32)ShaderStage::Vertex * (vert) + 
                    (u32)ShaderStage::Fragment * (frag) + 
                    (u32)ShaderStage::None * (!vert && !frag));
        return add_compiled_program(new_path.c_str(), stage);
    }
    i32 shader::add_compiled_program(const char* src, ShaderStage st)
    {
        auto idx = mprograms.size();
        mprograms.emplace_back();
        if (load_compiled_program(idx, src, st))
        {
            return idx;
        }
        return -1;
    }
    void shader::parse_file(const char* path)
    {
        auto name = std::string(path);
        auto stuff = cpp_utils::string::split(name, '.');
        auto last = stuff[stuff.size() - 1];
        bool vert = last == "vert";
        bool frag = last == "frag";
        ShaderStage stage = 
            (ShaderStage)((u32)ShaderStage::Vertex * (vert) + 
            (u32)ShaderStage::Fragment * (frag) + 
            (u32)ShaderStage::None * (!vert && !frag));
        auto content = cpp_utils::string::load_file_to_string(std::move(path));
        content = cpp_utils::string::trim_all_space(content);
        std::vector<char> delim =  {';', '{'};
        auto lines = cpp_utils::string::split(content, delim);
        ParseMode mode = ParseMode::None;
        u32 curr_size = 0;
        u32 binding = 0;
        u32 offset = 0;
        bool first_entry = true;
        for (const auto& line : lines)
        {
            auto str = cpp_utils::string::trim_all_space(line);
            if (str.empty())
            {
                continue;
            }
            switch (mode)
            {
                case ParseMode::None: 
                    {
                        if (str.contains("uniform"))
                        {
                            auto layout_it = std::search(str.begin(), str.end(), std::begin("layout"), std::end("layout") -1);
                            auto para_it = layout_it;
                            bool correct = true;
                            if (layout_it != str.end())
                            {
                                para_it = std::find(layout_it, str.end(), '(');
                                auto new_str = std::string(layout_it + 6, para_it);
                                for (auto c : new_str)
                                {
                                    if (c != ' ' && c != '\n' && c != '\t')
                                    {
                                        correct = false;
                                        break;
                                    }
                                }
                            }
                            else 
                            {
                                correct = false;
                            }
                            if (correct)
                            {
                                if (str.contains("push_constant"))
                                {
                                    curr_size = 0;
                                    offset = 0;
                                    mode = ParseMode::ps;
                                    first_entry = true;
                                }
                                else 
                                {
                                    auto it = std::search(str.begin(), str.end(), std::begin("binding"), std::end("binding")-1);
                                    if (it != str.end())
                                    {
                                        it = std::find(it, str.end(), '=');
                                        auto it2 = std::find(it, str.end(), ')');
                                        if (it != str.end())
                                        {
                                            curr_size = 0;
                                            std::string binding_str = std::string(it + 1, it2);
                                            ExpressionSpecs<int, char> s = 
                                            {
                                                .sOperators = Common::Algebra::operators<int>,
                                                .sConvertionFunc = Common::Algebra::getValFromChar<int>,
                                            };
                                            ExpressionEvaluator<int, char> ee(s);
                                            binding = ee.evaluate(binding_str);
                                            mode = ParseMode::uniform;
                                            first_entry = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                case ParseMode::ps:
                    {
                        if (str.contains('}'))[[unlikely]]
                        {
                            mode = ParseMode::None;
                            m_push_constants.push_back(
                                    {
                                    .stage = stage,
                                    .offset = offset,
                                    .size = curr_size,
                                    }
                                    );
                        }
                        else 
                        {
                            std::string curr_str = str;
                            if (first_entry)
                            {
                                auto it2 = std::search(str.begin(), str.end(), std::begin("layout"), std::end("layout")-1);
                                if (it2 != str.end())
                                {
                                    it2 = std::find(it2, str.end(), '(');
                                    auto end_it = std::find(it2, str.end(), ')');
                                    if (it2 != str.end() && end_it != str.end())
                                    {
                                        auto it = std::search(it2, str.end(), std::begin("offset"), std::end("offset")-1);
                                        if (it != str.end())
                                        {
                                            it = std::find(it, str.end(), '=');
                                            if (it != str.end())
                                            {
                                                std::string offset_str = std::string(it + 1, str.end());
                                                offset_str = cpp_utils::string::trim_all_space(offset_str);
                                                offset = std::stoi(offset_str);
                                            }
                                        }
                                        curr_str = std::string(end_it + 1, str.end());
                                    }
                                }
                                first_entry = false;
                            }
                            auto sub_str = cpp_utils::string::split_all_space(cpp_utils::string::trim_all_space(curr_str));
                            for (const auto& word : sub_str)
                            {
                                if (g_shader_type_size.contains(word))
                                {
                                    curr_size += g_shader_type_size.at(word);
                                    break;
                                }
                            }
                        }
                    }
                    break;
                case ParseMode::uniform:
                    {
                        if (str.contains('}'))[[unlikely]]
                        {
                            mode = ParseMode::None;
                            m_static_uniforms.push_back(
                                    {
                                    .binding = binding,
                                    .stage = stage,
                                    .type = UniformType::Static,
                                    .size = curr_size,
                                    .count = 1,
                                    }
                                    );
                        }
                        else 
                        {
                            first_entry = false;
                            auto sub_str = cpp_utils::string::split_all_space(str);
                            for (const auto& word : sub_str)
                            {
                                auto iter = std::find_if_not(
                                        word.begin(), 
                                        word.end(),
                                        [](char c)
                                        {
                                        return 
                                        (c <= 'z' && c >= 'a') || 
                                        (c <= 'Z' && c >= 'A') || 
                                        (c <= '9' && c >= '0');
                                        });
                                auto search = std::string(
                                        word.begin(),
                                        iter);
                                if (g_shader_type_size.contains(search))
                                {
                                    int mul = 1;
                                    if (iter != word.end())
                                    {
                                        iter = std::find_if(iter, word.end(),
                                                [](char c)
                                                {
                                                return (c <= '9' && c >= '0');
                                                });
                                        if (iter != word.end())
                                        {
                                            mul = std::stoi(std::string(iter, word.end()));
                                        }
                                    }
                                    curr_size += mul * g_shader_type_size.at(search);
                                    break;
                                }
                            }
                        }
                    }
            }
        }
    }
    bool shader::load_compiled_program(i32 idx, const char* src, ShaderStage st)
    {
        if (idx > mprograms.size()-1) 
        {
            return false;
        }
        std::ifstream file(src, std::ios::ate | std::ios::binary);
        if (!file)
        {
            return false;
        }
        u32 file_size = (u32) file.tellg();
        mprograms[idx].program.resize(file_size);
        file.seekg(0);
        file.read(mprograms[idx].program.data(), file_size);
        file.close();
        mprograms[idx].stage = st;
        return true;
    }
    void shader::combine_uniforms()
    {
        m_uniforms.clear();
        m_uniforms.reserve(m_static_uniforms.size() + m_dynamic_uniforms.size());
        for (auto& u : m_static_uniforms)
        {
            m_uniforms.push_back(u);
        }
        for (auto& u : m_dynamic_uniforms)
        {
            m_uniforms.push_back(u);
        }
    }
    void shader::add_uniform(Uniform&& u)
    {
        switch(u.type)
        {
            case UniformType::Static:
                {
                    m_static_uniforms.push_back(u);
                }
                break;
            case UniformType::Dynamic:
                {
                    m_dynamic_uniforms.push_back(u);
                }
        };
    }
    void shader::add_ps(PushConstant&& ps)
    {
        m_push_constants.push_back(ps);
    }
}}

