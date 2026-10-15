#include "editor.h"
#include <filesystem>
#include <functional>
#include <memory>
#include "types.h"
namespace lina { namespace engine{
    using wsettings = typename lina::renderer::backend::window<GraphicsBackend::Vulkan>::settings;
    editor::editor(renderer::lrenderer<GraphicsBackend::Vulkan>* rend): 
        m_close(false),
        m_curr_layer(0), 
        m_layers({}), 
        m_window(nullptr),
        m_renderer(rend)
    {
//        std::filesystem::current_path(ENGINE_ROOT);
    }
    void editor::init(renderer::backend::window<GraphicsBackend::Vulkan>* window)
    {
        m_window = window;
        m_renderer->init(m_window);
        for (int i = 0; i < m_layers.size(); i++)
        {
            //if (i == m_curr_layer || m_layers[i]->init_always == true)
            {
                m_layers[i]->init();
            }
        }
    }
    void editor::run()
    {
        while (!m_close)
        {
            handle_events();
            auto [w, h] = m_renderer->get_dimensions();
            m_renderer->begin_draw();
            for (auto l : m_layers)
            {
                m_renderer->set_view(
                        l->m_x0 * w, 
                        l->m_y0 * h, 
                        l->m_width * w, 
                        l->m_height * h);
                l->run();
            }
            m_renderer->end_draw();
            //m_layers[m_curr_layer]->run();
        }
        for (auto* i : m_layers)
            delete i;
    }
    void editor::handle_events()
    {
        category_sub<events::key_press>(
            (std::function<void(events::key_press&)>)std::bind(
                &editor::on_key_down,
                this, 
                std::placeholders::_1));


        category_sub<events::window_close>(
            (std::function<void(events::window_close&)>)std::bind(
                &editor::close,
                this,
                std::placeholders::_1));

        for (auto& l : m_layers)
        {
        category_sub<events::key_press>(
                (std::function<void(events::key_press&)>)std::bind(
                    &core::layer::on_key_down,
                    l, 
                    std::placeholders::_1));
        category_sub<events::window_resize>(
                (std::function<void(events::window_resize&)>)std::bind(
                    &core::layer::on_resize,
                    l, 
                    std::placeholders::_1));
        category_sub<events::mouse_button_pressed>(
                (std::function<void(events::mouse_button_pressed&)>)std::bind(
                    &core::layer::on_mouse_click,
                    l, 
                    std::placeholders::_1));
        category_sub<events::mouse_move>(
                (std::function<void(events::mouse_move&)>)std::bind(
                    &core::layer::on_mouse_move,
                    l, 
                    std::placeholders::_1));
        category_sub<events::mouse_button_released>(
                (std::function<void(events::mouse_button_released&)>)std::bind(
                    &core::layer::on_mouse_release,
                    l, 
                    std::placeholders::_1));
        }

        auto& events = m_window->get_events();
        for (auto* e : events)
        {
            e->handle();
            delete e;
        }
        m_window->clear_events();
    }
    void editor::close(events::window_close& e)
    {
        (void)e;
        m_close = true;
    }
    void editor::on_key_down(events::key_press& e)
    {
        switch (e.key())
        {
            case lina::input::key_code::W:
            {
                m_renderer->set_wireframe();
            }
            break;
            case lina::input::key_code::S:
            {
                m_renderer->set_solid();
            }
            break;
            case lina::input::key_code::P:
            {
                m_renderer->set_point();
            }
            break;
            default:{}
        }
    }
    
}}
