#ifndef UI_LAYER_H
#define UI_LAYER_H
#include "event/mouse_event.h"
#include "lmath/vector2d.h"
#include "src/core/layer.h"
#include "lrenderer/renderer.h"
#include "lmesh/mesh.h"
#include "lmath/all.h"
#include "lmath/transformations.h"
#include "src/cameras/perspective.h"
#include "lmath/vector3d.h"
#include "lmath/vector4d.h"
#include "lengine/lfonts/font.h"
#include "lrenderer/lshader/shader_emission.h"
#include "types.h"
namespace lina { namespace layers { 
    class ui_component 
    {
        public:
            virtual bool within(f32 x, f32 y) = 0;
            virtual void form_data() = 0;
        public:
            inline const std::vector<f32>& get_data(){return m_data;}
            std::function<void()> act = [](){};
            int injection_mode = -1;
        public:
            std::vector<f32> m_data;
    };
    struct theme
    {
        math::vector4d<f32> bg_col = {32./255., 31./255., 49./255, 0.};
        math::vector4d<f32> component_col = {43./255, 42./255, 60./255, 1.};
        math::vector4d<f32> shadow_col = {0., 0., 0., 1.};
        math::vector4d<f32> click_col = {255./255, 186./255, 222./255, 1. };
        math::vector4d<f32> highlight_col = {255./255 *.5, 186./255 *.5, 222./255 * .5, 1. };
        math::vector4d<f32> fg_col = {0., 0., 0., 1.};
    };
    struct hover_info 
    {
        math::vector2d<f32> mouse_pos;
        int is_clicked = 0;
        int _pad;
    };
    class ui : public core::layer
    {
        public:
            ui(renderer::lrenderer<GraphicsBackend::Vulkan>* r, f32 x0, f32 y0, f32 width, f32 height);
            virtual void run() override;
            virtual void init() override;
            virtual void on_resize(events::window_resize& e) override;
            virtual void on_mouse_click(events::mouse_button_pressed& e) override;
            virtual void on_mouse_release(events::mouse_button_released& e) override;
            virtual void on_mouse_move(events::mouse_move& e) override;
            //virtual void on_key_down(events::key_press& e) override;
        private:
            renderer::lrenderer<GraphicsBackend::Vulkan>* m_renderer;
            u32 sidx;
            u32 pidx;
            hover_info m_hover_info;
            math::vector4d<f32> m_res;
            std::vector<ui_component*> m_components;
            std::vector<f64> m_font_glyphs;
            std::pair<u64, u64> m_font_texture_dimensions;
            theme m_theme;
            f32* mresize_var = nullptr;

            bool mresize_x = true;
            lina::font::font_reader font_reader;
    };

    class rect : public ui_component
    {
        public:
            rect() {injection_mode = 2;};
            rect(f32 x, f32 y, f32 w, f32 h): origin(x, y), dimensions(w, h){injection_mode = 2;}
            inline virtual bool within(f32 x, f32 y) override 
            {
                if (
                        ((x - origin.x- radius) <= dimensions.x &&
                        (x - origin.x - radius) >= 0)
                    &&
                        ((y - origin.y - radius) <= dimensions.y &&
                        (y - origin.y - radius) >= 0))
                {
                    return true;
                }
                return false;
            }
            inline virtual void form_data() override
            {
                m_data.reserve(4);
                m_data.push_back(origin.x + dimensions.x/2 + radius);
                m_data.push_back(origin.y + dimensions.y/2 + radius);
                m_data.push_back(dimensions.x/2 - radius);
                m_data.push_back(dimensions.y/2 - radius);
            }
        public:
            math::vector2d<f32> origin;
            math::vector2d<f32> dimensions;
            float radius = 0.04f;
    };
}}
#endif
