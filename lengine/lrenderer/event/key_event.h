#ifndef KEY_EVENT_H
#define KEY_EVENT_H
#include "event.h"
#include "key_codes.h"
namespace lina{ namespace events{
    class key_press : public event
    {
        public:
            key_press(input::key_code keycode, b8 is_hold = false, b8 is_shift = false, b8 is_caps = false): 
                mkey_code(keycode), m_is_hold(is_hold), m_shift(is_shift), m_caps(is_caps){}
            b8 is_hold() const {return m_is_hold;}
            b8 is_upper() const {return m_shift ^ m_caps;}
            input::key_code key() const {return mkey_code;}
            virtual type get_type() const override {return get_static_type();}
            static type get_static_type() {return type::key_down;}
        private:
            input::key_code mkey_code;
            b8 m_is_hold;
            b8 m_shift;
            b8 m_caps;
    };

    class key_release : public event
    {
        public:
            key_release(input::key_code keycode, b8 is_shift = false, b8 is_caps = false): 
                mkey_code(keycode), m_shift(is_shift), m_caps(is_caps){}
            input::key_code key() const {return mkey_code;}

            virtual type get_type() const override {return get_static_type();}
            static type get_static_type() {return type::key_up;}
        private:
            input::key_code mkey_code;
            b8 m_shift;
            b8 m_caps;
    };
}}
#endif

