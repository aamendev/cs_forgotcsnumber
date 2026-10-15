#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include "vector2d.h"
#include <complex>
#include <cstdlib>
#include "types.h"
#define EPS (1e-7)
namespace lina { namespace math { namespace utils {
    template <typename T>
    inline T transfer_range(const T& v, T&& min1, T&& max1, T&& min2, T&& max2)
    {
        return ((max2 - min2) / (max1 - min1)) * (v - min1) + min2;
    }
    inline i16 f64_cmp(f64 f1, f64 f2, f64 eps = EPS)
    {
        return (f1 - f2 >= eps) - (f2 - f1 >= eps);
    }
    inline bool f64_eq(f64 f1, f64 f2, f64 eps = EPS)
    {
        return std::abs(f1 - f2) <= eps;
    }
    template <typename T>
    inline T operator-(const T& s)
    {
        return s * (T)-1;
    }
    template <typename T>
    inline T operator-(T&& s)
    {
        return s * (T)-1;
    }

    template <typename T>
    inline T operator-(T s)
    {
        return s * (T)-1;
    }
    template <typename T>
    inline bool abs_cmp(T v1, T v2)
    {
        return std::abs(v1) < std::abs(v2);
    }
}}}
#endif
