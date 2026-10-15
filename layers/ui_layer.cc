#include "ui_layer.h"
#include "event/event.h"
#include "event/mouse_event.h"
#include "event/window_event.h"
#include "lcpp_utils/lstring.h"
#include "lmath/vector2d.h"
#include "shader_map.h"
#include "types.h"
#include <string>
#include <vector>
namespace lina { namespace layers { 
    ui::ui(renderer::lrenderer<GraphicsBackend::Vulkan>* r, f32 x0, f32 y0, f32 width, f32 height):m_renderer(r)
    {
        m_x0 = x0;
        m_y0 = y0;
        m_width = width;
        m_height = height;
    }
    void ui::init()
    {
        auto [w, h] = m_renderer->get_dimensions();
        m_res = {m_x0 * w, m_y0 * h, m_width * w, m_height * h};

        { // load buttons (replace with file soon)
            float pad = 0.1;
            auto* but1 = new rect(-1., -1.0, 0.2, 0.1);
            but1->act = [&](){
                m_renderer->set_solid();
            };
            auto* but2 = new rect(-1. + pad + 0.2, -1., 0.2, 0.1);
            but2->act = [&](){
                m_renderer->set_wireframe();
            };
            auto* but3 = new rect(-1. + 2*(pad + 0.2), -1., 0.2, 0.1);
            but3->act = [&](){
                m_renderer->set_point();
            };
            m_components.push_back(
                    but1
                    );
            m_components.push_back(
                    but2
                    );
            m_components.push_back(
                    but3
                    );
        }
        shader::emission em;
        { // should potentially be visual editor
            shader::g_component_code["ui"] = 
            {
                cpp_utils::string::load_file_to_string("assets/shaders/ui_before.frag"),
                cpp_utils::string::load_file_to_string("assets/shaders/ui_main.frag"),
                shader::ShaderStage::Fragment
            };
            em.set_global_array_name("ui", "data");
            em.add_component_from_map("FULL_SCREEN");
            em.add_component_from_map("ui");
            em.set_global_array_name("ui", "data");
            em.set_injection_mode("ui", 0, "op_union(");
            em.set_injection_mode("ui", 1, "sd_box(in_uv, vec2(data[0], data[1]), vec2(data[2], data[3]))");
            em.set_injection_mode("ui", 2, "op_round(sd_box(in_uv, vec2(data[0], data[1]), vec2(data[2], data[3])), 0.04)");
        }

        { // add injection
            em.set_inject_component("ui");
            em.inject("v = ");
            for (int i = 0; i < m_components.size() - 1; i++)
            {
                em.inject(0);
            }
            em.inject(m_components[0]->injection_mode);
            for (int i = 1; i < m_components.size(); i++)
            {
                em.inject(",");
                em.inject(m_components[i]->injection_mode);
                em.inject(", idx, " + std::to_string(i));
                em.inject(")");
            }
            em.inject(";");
            em.submit_injections("ui");
            em.write_shader_to_file("ui", "assets/shaders");
        }

        shader::shader sh;
        sh.parse_and_load("assets/shaders/ui.vert", "compiled_shaders");
        sh.parse_and_load("assets/shaders/ui.frag", "compiled_shaders");
        sidx = m_renderer->add_shader(sh);

        pidx = m_renderer->add_mapping(-1, sidx);
        m_renderer->submit_scene();

        { // submit data
            std::vector<f32>data;
            for (auto c : m_components)
            {
                c->form_data();
                data.insert(data.end(), c->m_data.begin(), c->m_data.end());
            }
            m_renderer->update_mesh_uniform(sidx, 0, (void*)&m_theme);
            m_renderer->update_mesh_uniform(sidx, 1, &m_res);
            m_renderer->update_mesh_uniform(sidx, 2, data.data());
        }
    }
    void ui::run()
    {
        m_renderer->bind(pidx);
        m_renderer->bind_shader(sidx);
        m_renderer->update_mesh_constants(sidx,
                0, 
                &m_hover_info);
        m_renderer->render();
    }
    void ui::on_resize(events::window_resize& e)
    {
        auto w = e.get_width();
        auto h = e.get_height();
        m_res = {m_x0 * w, m_y0 * h, m_width * w, m_height * h};
        m_renderer->update_mesh_uniform(sidx, 1, &m_res);
    }
    void ui::on_mouse_click(events::mouse_button_pressed& e)
    {
        if (m_hover_info.mouse_pos.x > -2 && m_hover_info.mouse_pos.y > -2)
        {
            if (std::abs(m_hover_info.mouse_pos.x + 1.) < 0.05)
            {
                mresize_var = &m_x0;
            }
            else if (std::abs(m_hover_info.mouse_pos.x - 1.) < 0.05)
            {
                mresize_var = &m_width;
            }

            else if (std::abs(m_hover_info.mouse_pos.y + 1.) < 0.05)
            {
                mresize_var = &m_y0;
            }
            else if (std::abs(m_hover_info.mouse_pos.y - 1.) < 0.05)
            {
                mresize_var = &m_height;
            }
            int i = 0;
            for (auto c : m_components)
            {
                if (c->within(m_hover_info.mouse_pos.x, m_hover_info.mouse_pos.y))
                {
                    if (m_hover_info.is_clicked == 0)
                    {
                        m_hover_info.is_clicked = 1;
                    }
                    c->act();
                    e.set_handled();
                }
                i++;
            }

        }
    }
    void ui::on_mouse_release(events::mouse_button_released& e)
    {
        m_hover_info.is_clicked = 0;
        mresize_var = nullptr;
    }
    void ui::on_mouse_move(events::mouse_move& e)
    {
        if (
                (e.x() - m_res.x) >= 0 && (e.x() - m_res.x) <= m_res.z
                &&
                (e.y() - m_res.y) >= 0 && (e.y() - m_res.y) <= m_res.w
                )
        {
            m_hover_info.mouse_pos.x = (2*(e.x() - m_res.x) - m_res.z)/m_res.z;
            m_hover_info.mouse_pos.y = (2*(e.y() - m_res.y) - m_res.w)/m_res.w;
            std::cerr << m_hover_info.mouse_pos;
        }
        else 
        {
            m_hover_info.mouse_pos.x = -2;
            m_hover_info.mouse_pos.y = -2;
        }
    }
}}
