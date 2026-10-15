#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H
#include "event.h"
#include "key_codes.h"

namespace lina{ namespace events{
    class mouse_move : public event
    {
        public:
            mouse_move(f32 x, f32 y): m_x(x), m_y(y){}
        public:
            inline f32 x() const {return m_x;}
            inline f32 y() const {return m_y;}
            inline std::pair<float, float> get() const {return {m_x, m_y};}
            inline virtual type get_type() const override {return get_static_type();}
            inline static type get_static_type() {return type::mouse_move;}
        private:
            f32 m_x;
            f32 m_y;
    };

    class mouse_scroll : public event
    {
        public:
            mouse_scroll(f32 x, f32 y): m_x(x), m_y(y){}
        public:
            inline f32 x() const {return m_x;}
            inline f32 y() const {return m_y;}
            inline std::pair<float, float> get() const {return {m_x, m_y};}
            inline virtual type get_type() const override {return get_static_type();}
            inline static type get_static_type() {return type::mouse_scroll;}
        private:
            f32 m_x;
            f32 m_y;
    };

    class mouse_button_pressed : public event
    {
        public:
            mouse_button_pressed(input::mouse_code button): m_button(button){}
            inline input::mouse_code button() const {return m_button;}
            inline virtual type get_type() const override {return get_static_type();}
            inline static type get_static_type() {return type::mouse_down;}
        private:
            input::mouse_code m_button;
    };

    class mouse_button_released : public event
    {
        public:
            mouse_button_released(input::mouse_code button): m_button(button){}
            inline input::mouse_code button() const {return m_button;}
            inline virtual type get_type() const override {return get_static_type();}
            inline static type get_static_type() {return type::mouse_up;}
        private:
            input::mouse_code m_button;
    };
}}
#endif

