#ifndef IMPLICIT_CURVE_H
#define IMPLICIT_CURVE_H
#include "types.h"
#include "vector3d.h"
namespace lina { namespace math{ 
    template <class T>
    class implicit_curve
    {
        public:
            virtual f64 eval(const T& at) = 0;
            virtual T grad(const T& at) = 0;
    };
}}
#endif
