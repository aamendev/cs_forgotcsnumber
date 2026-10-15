#ifndef RENDERER_H
#define RENDERER_H
#include "lshader/shader.h"
#include "types.h"
#include "vulkan_backend/buffers/vertex_buffer.h"
#include "vulkan_backend/buffers/index_buffer.h"
#include "render_settings.h"
#include <vector>
namespace lina { namespace renderer {
    template <GraphicsBackend gbackend>
        class lrenderer
        {
            public: 
                inline b8 init(backend::window<gbackend>* w) {return mmanger.init(w);}
                void begin_draw();
                inline void push_window(backend::window<GraphicsBackend::Vulkan>* win);
                inline void pop_window();
                void set_view(f32 x0 = 0, f32 y0 = 0, f32 width = -1, f32 height = -1);
                inline std::pair<f32, f32> get_dimensions();
                void end_draw();
                void bind(u32 idx = 0);
                void bind_shader(u32 idx = 0);
                void load_default_shaders();
                u32 add_shader(const shader::shader& s);
                template<typename T>
                    u32 add_mesh(
                            backend::vblayout& layout, const std::vector<T>& verts, const std::vector<u32>&ind);
                template<typename T>
                    void update_mesh(u32 idx, const std::vector<T>& verts, const std::vector<u32>& ind);
                inline void update_mesh_constants(u32 material_id, u32 constant_id, void* data);
                inline void update_mesh_uniform(u32 material_id, u32 uniform_id, void* data);
                u32 add_mapping(u32 vidx, u32 sidx);
                inline void set_mesh_shader(i32 mesh_id, i32 shader_id);
                void submit_scene();
                void render();
                inline void set_wireframe();
                inline void set_solid();
                inline void set_point();
            private:
                backend::manager mmanger;
                std::vector<backend::buffers::vertex> mvertex_buffers;
                std::vector<backend::buffers::index> mindex_buffers;
                std::vector<shader::shader> mshaders;
                std::vector<std::pair<i32, i32>> mshader_mappings;
                u32 mcurr_pipeline = 0;
                u32 m_unadded_pipeline = 0;
                renderer_settings msettings;
        };
    template <GraphicsBackend gbackend>
        std::pair<f32, f32> lrenderer<gbackend>::get_dimensions()
        {
            return mmanger.get_dimensions();
        }
    template <GraphicsBackend gbackend>
        void lrenderer<gbackend>::set_wireframe() {msettings.sdraw = draw_mode::wireframe;}
    template <GraphicsBackend gbackend>
        void lrenderer<gbackend>::set_solid() {msettings.sdraw = draw_mode::solid;}
    template <GraphicsBackend gbackend>
        void lrenderer<gbackend>::set_point() {msettings.sdraw = draw_mode::point;}

    template <GraphicsBackend gbackend>
        template<typename T>
        u32 lrenderer<gbackend>::add_mesh(
                backend::vblayout& layout, const std::vector<T>& verts, const std::vector<u32>&indices)
        {
            backend::buffers::vertex vb;
            vb.init(&mmanger, layout);
            vb.construct(&verts[0],
                    verts.size() * sizeof(verts[0]));
            mvertex_buffers.push_back(vb);

            backend::buffers::index ib;
            ib.init(&mmanger);
            ib.construct(&indices[0],
                    indices.size() * sizeof(indices[0]));
            mindex_buffers.push_back(ib);

            return mindex_buffers.size() - 1;
        }
    template <GraphicsBackend gbackend>
        template<typename T>
        void lrenderer<gbackend>::update_mesh(
                u32 idx, const std::vector<T>& verts, const std::vector<u32>& indices)
        {
            backend::buffers::vertex vb;
            mvertex_buffers[idx].construct(&verts[0],
                    verts.size() * sizeof(verts[0]));
            mindex_buffers[idx].construct(&indices[0],
                    indices.size() * sizeof(indices[0]));
        }
    template <GraphicsBackend gbackend>
        inline void lrenderer<gbackend>::update_mesh_constants(u32 material_id, u32 constant_id, void* data)
        {
            mmanger.update_ps(material_id, constant_id, data);
        }
    template <GraphicsBackend gbackend>
        inline void lrenderer<gbackend>::update_mesh_uniform(u32 material_id, u32 uniform_id, void* data)
        {
            mmanger.update_uniform_buffer(material_id, uniform_id, data);
        }

    template <GraphicsBackend gbackend>
        inline void lrenderer<gbackend>::push_window(backend::window<GraphicsBackend::Vulkan>* win)
        {
            mmanger.push_window(win);
        }
    template <GraphicsBackend gbackend>
        inline void lrenderer<gbackend>::pop_window()
        {
            mmanger.pop_window();
        }
}}
#endif
