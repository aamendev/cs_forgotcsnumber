#ifndef COMMON_PARAMETRIC_CURVES_H
#define COMMON_PARAMETRIC_CURVES_H
#include "parametric_curve.h"
#include "vector2d.h"
namespace lina { namespace math{ namespace parametric{
    class circle : public parametric_curve<vector2d<f64>>
    {
        public:
            f64 r;
            circle(f64 r): r(r){}
        public:
            virtual vector2d<f64> eval(f64 t) override;
            virtual vector2d<f64> grad(f64 t) override;
    };
    class descartes_follium : public parametric_curve<vector2d<f64>>
    {
        public:
            f64 a;
            descartes_follium(f64 a): a(a) {}
        public:
            virtual vector2d<f64> eval(f64 t) override;
            virtual vector2d<f64> grad(f64 t) override;
    };
}}}
#endif
