#ifndef SIMPLE_LAYER_H
#define SIMPLE_LAYER_H
#include "src/core/layer.h"
#include "lrenderer/renderer.h"
#include "lmesh/mesh.h"
#include "lmath/all.h"
#include "lmath/transformations.h"
#include "src/cameras/perspective.h"
#include "lmath/vector3d.h"
#include "lmath/vector4d.h"
#include "lrenderer/lshader/shader_emission.h"
#include "types.h"
namespace lina { namespace layers { 
    class simple : public core::layer
    {
        public:
            struct LocalPush 
            {
                math::mat4<f32> model;
                math::mat4<f32> view;
            };
            simple(renderer::lrenderer<GraphicsBackend::Vulkan>* r, f32 x0, f32 y0, f32 width, f32 height);
            virtual void run() override;
            virtual void init() override;
            virtual void on_key_down(events::key_press& e) override;
        private:
            void fill_verts(std::vector<f32>& data);
            void fill_inds(std::vector<u32>& inds);
        private:
            renderer::lrenderer<GraphicsBackend::Vulkan>* m_renderer;
            mesh::mesh<f32> m_sphere;
            cameras::perspective m_cam;
            LocalPush m_push;
            math::vector3d<f32> m_col;
            u32 sidx;
            u32 vidx;
            u32 pidx;
    };
}}
#endif
