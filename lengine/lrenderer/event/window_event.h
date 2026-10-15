#ifndef WINDOW_EVENT_H
#define WINDOW_EVENT_H
#include "event.h"
namespace lina{ namespace events{
    class window_resize : public event
    {
        public:
            window_resize(i32 width, i32 height) : m_width(width), m_height(height){}
            inline i32 get_width() const {return m_width;}
            inline i32 get_height() const {return m_height;}
            inline virtual type get_type() const override {return get_static_type();}
            inline static type get_static_type() {return type::window_resize;}
        private:
            i32 m_width;
            i32 m_height;
    };
    class window_close : public event
    {
        public:
            window_close() = default;
            inline static type get_static_type() {return type::window_close;}
            inline virtual type get_type() const override {return get_static_type();}
    };
}}
#endif

