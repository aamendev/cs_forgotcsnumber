#include "mesh_colider.h"
#include <cfloat>
namespace lina { namespace ecs { namespace components { namespace colliders {
    bool mesh::check_collision(collider* c)
    {
        m_info = gjk::gjk_and_info((collider*)this, c);
        return m_info.collided;
    }
    math::vector3d<f32> mesh::furthest_point(const math::vector3d<f32>& d)
    {
        math::mat4<f32> transform = 
            math::transformations::rot_matrix<f32>(m_rotation) *
            math::transformations::scale_matrix<f32>(m_scale);

        math::vector4d<f32> temp = (math::vector4d<f32>){d.x, d.y, d.z, 1} * transform.get_transpose();
        math::vector3d<f32> new_dir = {temp.x, temp.y, temp.z};

        math::vector3d<f32> further_point = {0, 0, 0};
        math::vector3d<f32> current = {0, 0, 0};
        f32 max_distance = -FLT_MAX;

        for (int i = 0; i + 2 < m_vertices.size(); i+=3)
        {
            current = {m_vertices[i], m_vertices[i + 1], m_vertices[i + 2]};
            f32 dot_prod = current.dot(new_dir); 

            if (dot_prod > max_distance)
            {
                further_point = current;
                max_distance = dot_prod;
            }
        }
        auto ret =  (transform * further_point + m_center);
        return ret; 
    }
    std::pair<math::vector3d<f32>, math::vector3d<f32>> mesh::compute_bounding_box(u32 idx_begin, u32 idx_end)
    {
        math::mat4<f32> transform = 
            math::transformations::rot_matrix<f32>(m_rotation)*
            math::transformations::scale_matrix<f32>(m_scale);
        f32 min_x = FLT_MAX; 
        f32 min_y = FLT_MAX; 
        f32 min_z = FLT_MAX; 
        f32 max_x = -FLT_MAX; 
        f32 max_y = -FLT_MAX; 
        f32 max_z = -FLT_MAX; 
        math::vector3d<f32> point;
        for (int i = idx_begin; i < idx_end - 2; i+=3)
        {
            point = 
                (math::vector3d<f32>(m_vertices[i], m_vertices[i+1], m_vertices[i+2])) * transform + m_center;

            min_x = min_x * (min_x < point.x) + point.x * !(min_x < point.x);
            min_y = min_y * (min_y < point.y) + point.y * !(min_y < point.y);
            min_z = min_z * (min_z < point.z) + point.z * !(min_z < point.z);

            max_x = max_x * (max_x > point.x) + point.x * !(max_x > point.x);
            max_y = max_y * (max_y > point.y) + point.y * !(max_y > point.y);
            max_z = max_z * (max_z > point.z) + point.z * !(max_z > point.z);
        }
        auto bounding_box_extents_first = math::vector3d<f32>(min_x, min_y, min_z);

        auto bounding_box_extents_second = math::vector3d<f32>(max_x, max_y, max_z);

        return std::make_pair(bounding_box_extents_first, bounding_box_extents_second);
    }

    void mesh::compute_BVH()
    {
    }
    void mesh::compute_bounding_box()
    {
        m_bounding_box_extents = compute_bounding_box(0, m_vertices.size());
    }
}}}}
