#ifndef ECS_MATERIAL_H
#define ECS_MATERIAL_H
#include <lmath/vector4d.h.h>
namespace lina { namespace ecs { namespace components{
    class material 
    {
        public:
            material(): ambient(math::vector4d<f32>(1.f, 1.f, 1.f, 1.f)){}
        public:
            math::vector4d<f32> ambient;
    };
}}}
#endif
