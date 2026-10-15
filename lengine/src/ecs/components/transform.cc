#include "transform.h"

namespace lina { namespace ecs {  namespace components{
    void transform::compute_matrix(bool compute_inv)
    {
        m_tarns_matrix = 
            math::transformations::translate_matrix<f32>(mpos) * 
            math::transformations::rot_matrix<f32>(math::vector3d<f32>(1, 0, 0), mrot.x) *  
            math::transformations::rot_matrix<f32>(math::vector3d<f32>(0, 1, 0), mrot.y) *  
            math::transformations::rot_matrix<f32>(math::vector3d<f32>(0, 0, 1), mrot.z) *  
            math::transformations::scale_matrix<f32>(mscale);
        if (compute_inv)[[likely]]
        {
            m_inv_matrix = m_tarns_matrix.get_inverse();
        }
    }
    math::mat4<f32> transform::get_transformation_matrix()
    {
        if (!m_computed)
        {
            compute_matrix();
        }
        return m_tarns_matrix;
    }
    math::mat4<f32> transform::get_inv_transformation_matrix()
    {
        if (!m_computed)
        {
            compute_matrix();
        }
        return m_inv_matrix;
    }
}}}
