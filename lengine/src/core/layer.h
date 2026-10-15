#ifndef LAYER_H
#define LAYER_H
#include "lrenderer/event/all_events.h"
namespace lina { namespace core {
    class layer
    {
        public:
            virtual ~layer() = default;
            virtual void run() = 0;
            virtual void init() = 0;
            virtual void on_key_down(events::key_press& e){};
            virtual void on_mouse_move(events::mouse_move& m){};
            virtual void on_mouse_click(events::mouse_button_pressed& m){};
            virtual void on_mouse_release(events::mouse_button_released& m){};
            virtual void on_key_up(events::key_release& e){};
            virtual void on_resize(events::window_resize& e){};
        public:
            b8 init_always = false;
            f32 m_width = -1;
            f32 m_height = -1;
            f32 m_x0 = 0;
            f32 m_y0 = 0;
    };
}}
#endif

