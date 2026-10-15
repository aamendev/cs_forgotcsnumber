#ifndef EVENT_H
#define EVENT_H
#include <functional>
#include <string>
#include "../types.h"
namespace lina { 
    namespace events {
    enum class type : u16
    {
        none = 0,
        window_close, window_resize, window_focus, window_move,
        app_tick, app_update, app_render,
        key_down, key_up,
        mouse_down, mouse_up, mouse_move, mouse_scroll
    };
    }

    class event
    {
        public:
            virtual ~event() = default;
            virtual events::type get_type() const {return events::type::none;};
            template<typename ...Args, typename ...f_args>
            inline void subscribe(std::function<void(Args...)>f, f_args&&... args) 
            {
                std::function<void()> f2 = std::bind(f, args...);
                subscribers.push_back(f2);
            }
            inline u32 remaining_subs() const {return subscribers.size();}
            inline b8 handle()
            {
                for (auto f : subscribers)
                {
                    if (mhandled) break;
                    f();
                }
                subscribers.clear();
                return true;
            }
            inline void set_handled(bool h = true){mhandled = h;}
        protected:
            bool mhandled = false;
            std::vector<std::function<void()>> subscribers;
    };
}
#endif


