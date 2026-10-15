#include "perspective.h"
namespace lina  { namespace cameras{

    void perspective::compute_matrix()
    {
        math::vector3d<f32> n = m_target.normalized();
        math::vector3d<f32> u = m_up.cross(n).normalized();
        math::vector3d<f32> v = n.cross(u);
        m_camera_matrix.set_col(0, math::conversions::extend(std::move(u), -m_pos.x));
        m_camera_matrix.set_col(1, math::conversions::extend(std::move(v), -m_pos.y));
        m_camera_matrix.set_col(2, math::conversions::extend(std::move(n), -m_pos.z));
        m_camera_matrix.set_col(3, math::vector4d<f32>(0, 0, 0, 1));
    }
}}
