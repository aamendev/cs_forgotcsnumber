#ifndef PERSPECTIVE_CAMERA
#define PERSPECTIVE_CAMERA
#include <lmath/all.h>
#include <lmath/types.h>
#include <lmath/transformations.h>
#include <lmath/utils.h>
#include <lmath/lossy_conversions.h>
namespace lina { namespace cameras {
    class perspective 
    {
        public:
            perspective() : 
                m_sensitivity(1.f), 
                m_up(0., 1., 0.), 
                m_pos(0, 0, 0), 
                m_target(0, 0, 1),
                m_compute_martix(true)
        {}
            inline void set_pos(math::vector3d<f32>&& pos);
            inline void move(math::vector3d<f32>&& update);
            inline void set_up(math::vector3d<f32>&& up);
            inline void look_at(math::vector3d<f32>&& target);

            inline const math::vector3d<f32>& up();
            inline const math::vector3d<f32>& target();
            inline const math::vector3d<f32>& pos();
            inline f32 sensitivity();

            inline const math::mat4<f32>& get_matrix();
        private:
            void compute_matrix();
        private:
            math::vector3d<f32> m_pos;
            math::vector3d<f32> m_up;
            math::vector3d<f32> m_target;
            math::mat4<f32> m_camera_matrix;
            bool m_compute_martix;
            f32 m_sensitivity;
    };
    inline const math::mat4<f32>& perspective::get_matrix()
    {
        if (m_compute_martix)
        {
            compute_matrix();
            m_compute_martix = false;
        }
        return m_camera_matrix;
    }
    inline void perspective::set_pos(math::vector3d<f32>&& pos)
    {
        m_compute_martix = true;
        m_pos = pos;
    }
    inline void perspective::move(math::vector3d<f32>&& update)
    {
        m_compute_martix = true;
        m_pos += update;
    }
    inline const math::vector3d<f32>& perspective::up()
    {
        return m_up;
    }
    inline const math::vector3d<f32>& perspective::target()
    {
        return m_target;
    }
    inline const math::vector3d<f32>& perspective::pos()
    {
        return m_pos;
    }
    inline f32 perspective::sensitivity()
    {
        return m_sensitivity;
    }
}}
#endif
