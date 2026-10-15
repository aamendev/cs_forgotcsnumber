#ifndef MATH_LOSSY_COVNERSIONS_H
#define MATH_LOSSY_COVNERSIONS_H
#include "vector2d.h"
#include "vector3d.h"
#include "vector4d.h"
#include "mat4.h"
#include "mat3.h"
#include "mat2.h"
namespace lina { namespace math { namespace conversions {
    template <typename T> inline vector3d<T> drop_last(vector4d<T>&& v)
    {
        return vector3d<T>(v.x, v.y, v.z);
    }
    template <typename T> inline vector2d<T> drop_last(vector3d<T>&& v)
    {
        return vector2d<T>(v.x, v.y);
    }
    template <typename T> inline f64 drop_last(vector2d<T>&& v)
    {
        return v.x;
    }
    template <typename T> inline vector4d<T> extend(vector3d<T>&& v, f64 w = 0.)
    {
        return vector4d<T>(v.x, v.y, v.z, w);
    }
    template <typename T> inline vector3d<T> extend(vector2d<T>&& v, f64 z = 0.)
    {
        return vector3d<T>(v.x, v.y,z);
    }
    template <typename T>
        inline mat3<T> drop(mat4<T>&& m)
        {
            return mat3<T>(
                    drop_last(m.row(0)),
                    drop_last(m.row(1)),
                    drop_last(m.row(2))
                    );
        }
    template <typename T>
        inline mat2<T> drop(mat3<T>&& m)
        {
            return mat2<T>(
                    drop_last(m.row(0)),
                    drop_last(m.row(1))
                    );
        }
}}}
#endif
