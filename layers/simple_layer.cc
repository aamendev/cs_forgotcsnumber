#include "simple_layer.h"
#include "lmath/lossy_conversions.h"
#include "lmath/vector4d.h"
#include "lrenderer/lshader/shader_emission.h"
#include "lrenderer/event/key_codes.h"
#include <vector>
#include "lgraphics/color_space.h"
#include "types.h"
namespace lina { namespace layers { 
    simple::simple(renderer::lrenderer<GraphicsBackend::Vulkan>* r, f32 x0, f32 y0, f32 width, f32 height):m_renderer(r)
    {
        m_x0 = x0;
        m_y0 = y0;
        m_width = width;
        m_height = height;
    }
    void simple::init()
    {
        m_sphere.load("assets/meshes/sphere.obj");
        shader::emission em;
        em.add_component_from_map("DEFAULT_VERTEX");
        em.add_component_from_map("DEFAULT_FRAG");
        em.add_component_from_map("colour");
        em.write_shader_to_file("colour", "assets/shaders/");
        shader::shader sh;
        sh.parse_and_load("assets/shaders/colour.vert", "compiled_shaders");
        sh.parse_and_load("assets/shaders/colour.frag", "compiled_shaders");
        
        sidx = m_renderer->add_shader(sh);
        lina::renderer::backend::vblayout layout;
        layout.push(lina::renderer::backend::Format::Float3, 0);

        std::vector<u32> inds;
        fill_inds(inds);

        std::vector<f32> data;
        fill_verts(data);
        vidx = m_renderer->add_mesh(layout, data, inds);
        pidx = m_renderer->add_mapping(vidx, sidx);
        m_renderer->submit_scene();
        m_col = {0.55, 0.249939, 267};
        math::vector4d<f32> rgb_col = math::conversions::extend(graphics::colour_space::oklch_to_rgb(std::move(m_col)), 1.);
                m_renderer->update_mesh_uniform(sidx, 1, &rgb_col);
        m_renderer->update_mesh_uniform(sidx, 1, &rgb_col);
        auto proj = math::transformations::proj_matrix<f32>(90, 1.5); 
        m_renderer->update_mesh_uniform(sidx, 0, &proj);
    }
    void simple::run()
    {
        m_renderer->bind(pidx);
        m_renderer->bind_shader(sidx);
        auto trans = math::transformations::translate_matrix<f32>(math::vector3d<f32>(0,0, 2));
        auto view = m_cam.get_matrix();
        m_push.model = trans;
        m_push.view = view;
        m_renderer->update_mesh_constants(sidx, 0, &m_push);
        m_renderer->render();
    }
    void simple::fill_verts(std::vector<f32>& data)
    {
        data.clear();
        auto& pos = m_sphere.get_positions();
        data.resize(pos.size());
        for (int i = 0; i < pos.size()/3; i+=1)
        {
            data[3*i + 0] = pos[3 * i + 0];
            data[3*i + 1] = pos[3 * i + 1];
            data[3*i + 2] = pos[3 * i + 2];
        }
    }
    void simple::fill_inds(std::vector<u32>& inds)
    {
        inds = m_sphere.get_position_indices();
    }
    void simple::on_key_down(events::key_press& e)
    {
        switch (e.key())
        {
            case lina::input::key_code::Up:
            {
                m_sphere.subdivide(1);

                std::vector<u32> inds;
                fill_inds(inds);

                std::vector<f32> data;
                fill_verts(data);
                m_renderer->update_mesh(vidx, data, inds);
            }
            break;
            case lina::input::key_code::Down:
            {
                m_sphere.simplify(m_sphere.get_positions().size() / 6);
                std::vector<u32> inds;
                fill_inds(inds);

                std::vector<f32> data;
                fill_verts(data);
                m_renderer->update_mesh(vidx, data, inds);
            }
            break;
            case lina::input::key_code::R:
            {
                if (e.is_upper())
                {
                    m_col.x += 0.01;
                    if (m_col.x > 1.)
                    {
                        m_col.x = 0.;
                    }
                }
                else 
                {
                    m_col.x -= 0.01;
                    if (m_col.x < 0.)
                    {
                        m_col.x = 1.;
                    }
                }
                math::vector4d<f32> rgb_col = math::conversions::extend(graphics::colour_space::oklch_to_rgb(std::move(m_col)), 1.);
                m_renderer->update_mesh_uniform(sidx, 1, &rgb_col);
                std::cerr << m_col << '\n';
            }
            break;
            case lina::input::key_code::G:
            {
                auto max_chroma = graphics::colour_space::max_chroma(m_col.x, m_col.z);
                m_col.y = std::clamp(m_col.y, 0.f, max_chroma);
                std::cerr << max_chroma << '\n';
                if (e.is_upper())
                {
                    m_col.y += max_chroma/100.;
                    if (m_col.y > max_chroma)
                    {
                        m_col.y = 0.;
                    }
                }
                else 
                {
                    m_col.y -= max_chroma/100.;
                    if (m_col.y < 0.)
                    {
                        m_col.y = max_chroma;
                    }
                }
                math::vector4d<f32> rgb_col = math::conversions::extend(graphics::colour_space::oklch_to_rgb(std::move(m_col)), 1.);
                m_renderer->update_mesh_uniform(sidx, 1, &rgb_col);
                std::cerr << m_col << '\n';
            }
            break;
            case lina::input::key_code::B:
            {
                if (e.is_upper())
                {
                    m_col.z += 1.;
                    if (m_col.z > 360.)
                    {
                        m_col.z = 0.;
                    }
                }
                else 
                {
                    m_col.z -= 1.;
                    if (m_col.z < 0)
                    {
                        m_col.z = 360.;
                    }
                }
                math::vector4d<f32> rgb_col = math::conversions::extend(graphics::colour_space::oklch_to_rgb(std::move(m_col)), 1.);
                m_renderer->update_mesh_uniform(sidx, 1, &rgb_col);
                std::cerr << m_col << '\n';
            }
            break;
            default:{}
        }
    }
}}
