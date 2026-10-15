#ifndef WINDOW_H
#define WINDOW_H
#include <tuple>
#include <vulkan/vulkan_core.h>
#include <types.h>
#include "event/all_events.h"
namespace lina { namespace renderer { namespace backend { 
    template <GraphicsBackend T>
    struct SurfaceArgs;
    
    template<>
    struct SurfaceArgs<GraphicsBackend::Vulkan>
    {
        using ArgType = std::tuple<VkInstance*, VkSurfaceKHR*>;
    };
    template<>
    struct SurfaceArgs<GraphicsBackend::OpenGl>
    {
        using ArgType = std::tuple<>;
    };

    template <GraphicsBackend T>
        class window
        {
            public:
                struct settings
                {
                    std::string name = "Lina is pretty!";
                    f64 width = 0.;
                    f64 height = 0.;
                    b8 resizable = false;
                    b8 resized = false;
                    std::vector<event*> events = {};
                    void* platform_data = nullptr;
                };
                virtual ~window() = default;
                virtual b8 create(settings& s) = 0;
                virtual void update() = 0;
                virtual void create_surface(typename SurfaceArgs<T>::ArgType data) = 0;
                virtual b8 is_closed() const = 0;
                virtual void register_callbacks() = 0;
                virtual const char** vulkan_extensions(u64* count);
                virtual inline std::string get_name() const;
                virtual inline f64 get_width() const;
                virtual inline f64 get_height() const;
                virtual inline b8 is_resized() const;
                virtual const inline std::vector<event*>& get_events() const;
                virtual inline void set_resized(bool val);
                inline void clear_events();
            public:
                settings msettings;
        };
    template <GraphicsBackend T>
        const char** window<T>::vulkan_extensions(u64* count) {*count = 0; return {};}
    template <GraphicsBackend T>
        void window<T>::clear_events() {msettings.events.clear();}
    template <GraphicsBackend T>
        std::string window<T>::get_name() const {return std::string(msettings.name);};
    template <GraphicsBackend T>
        f64 window<T>::get_width() const {return msettings.width;}
    template <GraphicsBackend T>
        f64 window<T>::get_height() const {return msettings.height;}
    template <GraphicsBackend T>
        b8 window<T>::is_resized() const {return msettings.resized;};
    template <GraphicsBackend T>
        const inline
        std::vector<event*>& window<T>::get_events() const {return msettings.events;}
    template <GraphicsBackend T>
        void window<T>::set_resized(bool val) {msettings.resized = val;}
}}}
#endif
