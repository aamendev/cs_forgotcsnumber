#ifndef PARA_CURVE_H
#define PARA_CURVE_H
#include "types.h"
#include "vector3d.h"
namespace lina { namespace math{ 
    template <class T>
    class parametric_curve
    {
        public:
            virtual T eval(f64 t) = 0;
            virtual T grad(f64 t) = 0;
            inline f64 next_t(f64 t, f64 smax, f64 smin = 0.)
            {
                return t + std::max(smax / grad(t).norm(), smin / grad(t).norm());
            }
    };
}}
#endif
