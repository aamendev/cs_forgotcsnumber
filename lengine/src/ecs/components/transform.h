#ifndef ECS_TRANSFORM_H
#define ECS_TRANSFORM_H
#include <lmath/all.h>
#include <lmath/transformations.h>
namespace lina { namespace ecs {  namespace components{
    class transform
    {
        public: 
            transform() :m_computed(false), mscale(math::vector3d<f32>(1., 1., 1.)){}

            inline void set_pos(math::vector3d<f32>&& p);
            inline void set_rot(math::vector3d<f32>&& p);
            inline void set_scale(math::vector3d<f32>&& p);
            inline const math::vector3d<f32>& get_pos() const;
            inline const math::vector3d<f32>& get_rot() const;
            inline const math::vector3d<f32>& get_scale() const;
        public:
            inline void move(math::vector3d<f32>&& inc);
            inline void rotate(math::vector3d<f32>&& inc);
            inline void expand(math::vector3d<f32>&& inc);
            void compute_matrix(bool compute_inv = true);
            math::mat4<f32> get_transformation_matrix();
            math::mat4<f32> get_inv_transformation_matrix();
        private:
            math::vector3d<f32> mpos;
            math::vector3d<f32> mrot;
            math::vector3d<f32> mscale;
            math::mat4<f32> m_tarns_matrix;
            math::mat4<f32> m_inv_matrix;
            bool m_computed;
    };
    inline void transform::move(math::vector3d<f32>&& inc) {mpos += inc; m_computed = false;}
    inline void transform::rotate(math::vector3d<f32>&& inc) {mrot += inc; m_computed = false;}
    inline void transform::expand(math::vector3d<f32>&& inc) {mscale = mscale * inc; m_computed = false;}
    inline void transform::set_pos(math::vector3d<f32>&& v) {mpos = v; m_computed = false;}
    inline void transform::set_rot(math::vector3d<f32>&& v) {mrot = v; m_computed = false;}
    inline void transform::set_scale(math::vector3d<f32>&& v) {mscale = v; m_computed = false;}
    inline const math::vector3d<f32>& transform::get_pos() const
    {
        return mpos;
    }
    inline const math::vector3d<f32>& transform::get_rot() const
    {
        return mrot;
    }
    inline const math::vector3d<f32>& transform::get_scale() const
    {
        return mscale;
    }
}}}
#endif
