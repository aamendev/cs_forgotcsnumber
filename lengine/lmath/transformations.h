#ifndef MATH_TRANSFORMATIONS_H
#define MATH_TRANSFORMATIONS_H
#include "vector3d.h"
#include "all.h"
namespace lina { namespace math { namespace transformations {
    template <typename T>
        inline mat4<T> translate_matrix(vector3d<T>&& v)
        {
            mat4<T> m;
            m(3, 0) = v.x;
            m(3, 1) = v.y;
            m(3, 2) = v.z;
            return m;
        }
    template <typename T>
        inline mat4<T> translate_matrix(vector3d<T>& v)
        {
            return translate_matrix<T>(std::move(v));
        }
    template <typename T>
        inline mat4<T> scale_matrix(vector3d<T>&& v)
        {
            mat4<T> m;
            m(0, 0) = v.x;
            m(1, 1) = v.y;
            m(2, 2) = v.z;
            return m;
        }
    template <typename T>
        inline mat4<T> scale_matrix(vector3d<T>& v)
        {
            return scale_matrix<T>(std::move(v));
        }
    template <typename T>
        inline mat4<T> rot_matrix(vector3d<T>&& axis, f64 theta)
        {
            f64 c = cos(theta);
            f64 s = sin(theta);
            f64 d = 1 - c;
            vector3d<T> a = axis.normalized();
            return mat4<T>(
                    vector4d<T>(c + d * a.x * a.x, d * a.x * a.y - s * a.z, d * a.x * a.z + s * a.y, 0),
                    vector4d<T>(d * a.x * a.y + s * a.z, c + d * a.y * a.y, d * a.y * a.z - s * a.x, 0),
                    vector4d<T>(d * a.x * a.z - s * a.y, d * a.y * a.z + s * a.x, c + d * a.z * a.z, 0),
                    vector4d<T>(0, 0, 0, 1)
                    );
        }
    template <typename T>
        inline mat4<T> rot_matrix(vector3d<T>& axis, f64 theta)
        {
            return rot_matrix<T>(std::move(axis), theta);
        }
    template <typename T>
        inline mat4<T> rot_matrix(vector3d<T>&& angles)
        {
            return 
                rot_matrix<T>(vector3d<T>(1, 0, 0), angles.x) * 
                rot_matrix<T>(vector3d<T>(0, 1, 0), angles.y) *
                rot_matrix<T>(vector3d<T>(0, 0, 1), angles.z);
        }
    template <typename T>
        inline mat4<T> rot_matrix(vector3d<T>& angles)
        {
            return rot_matrix<T>(std::move(angles));
        }
    template <typename T>
        inline mat4<T> proj_matrix(f64 fov, f64 ar, f64 nearf = 0., f64 farf =1e3)
        {
            f64 rad = fov * 3.1415 / 180.0;
            f64 tanHalfFov = tan(rad * 0.5);
            f64 f = 1 / tanHalfFov;
            f64 range = farf-nearf;
            f64 A = (-farf - nearf) / range;
            f64 B = 2.0f * farf * nearf / range;
            return mat4<T>(
                    vector4d<T>(f / ar, 0, 0, 0),
                    vector4d<T>(0, f, 0, 0),
                    vector4d<T>(0, 0, A, B),
                    vector4d<T>(0, 0, 1, 0)).get_transpose();
        }
}}}
#endif
