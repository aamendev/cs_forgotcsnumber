#ifndef EDITOR_H
#define EDITOR_H
#include "lrenderer/renderer.h"
#include "lrenderer/vulkan_backend/window.h"
#include "../core/layer.h"
#include "types.h"
#include <memory>
namespace lina { namespace engine {
    class editor 
    {
        public:
            editor(renderer::lrenderer<GraphicsBackend::Vulkan>* m_renderer);
            void init(renderer::backend::window<GraphicsBackend::Vulkan>* window);
            void run();
            inline void add_layer(core::layer* l);

            template <typename EventType, typename Callback>  
                void category_sub(Callback f);
        private:
            inline void static set_close(b8& close) {close = false;}

            void handle_events();
            void close(events::window_close& e);
            void on_key_down(events::key_press& e);
        private:
            renderer::lrenderer<GraphicsBackend::Vulkan>* m_renderer;
            b8 m_close;
            std::vector<core::layer*> m_layers;
            i32 m_curr_layer;
            renderer::backend::window<GraphicsBackend::Vulkan>* m_window;
    };
    inline void editor::add_layer(core::layer* l) {m_layers.push_back(l);}

    template <typename EventType, typename Callback>  
        void editor::category_sub(Callback f)
        {
            auto& events = m_window->get_events();
            for (int i = 0; i < events.size(); i++)
            {
                auto* e = events[i];
                if (e->get_type() == EventType::get_static_type())
                {
                    e->subscribe(f, static_cast<EventType&>(*e));
                }
            }
        }
}}
#endif
