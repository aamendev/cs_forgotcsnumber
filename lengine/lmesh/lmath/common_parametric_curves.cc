#include "common_parametric_curves.h"
#include "vector2d.h"
namespace lina { namespace math{ namespace parametric{

    vector2d<f64> circle::eval(f64 t)
    {
        return vector2d<f64>(std::cos(t), std::sin(t)) * r;
    }
    vector2d<f64> circle::grad(f64 t)
    {
        return vector2d<f64>(-std::sin(t), std::cos(t));
    }

    vector2d<f64> descartes_follium::eval(f64 t)
    {
        f64 den = (1 + t * t * t);
        return vector2d<f64>(3. * a * t / den, 3. * a * t * t / den);
    }
    vector2d<f64> descartes_follium::grad(f64 t)
    {
        f64 den = (1 + t * t * t);
        return vector2d<f64>(
                3 * a * (1 - 2 * t * t * t) / (den * den), 
                3 * a * (2 * t - t * t * t * t) / (den * den));
    }
}}}
