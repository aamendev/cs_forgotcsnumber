#ifndef VECTOR_3D_H
#define VECTOR_3D_H
#include "types.h"
#include <algorithm>
#include <cmath>
#include <ostream>
namespace lina { namespace math{
    template <typename T>
    class vector3d
    {
        public:
            vector3d(T x, T y, T z): x(x), y(y), z(z){}
            vector3d(): x(0), y(0), z(0){}
            T x;
            T y;
            T z;
        public:
            // vec operations
            inline T norm() const;
            inline T sq_norm() const;
            inline T dot(const vector3d& other) const;
            inline vector3d cross(const vector3d& other) const;
            inline vector3d normalized() const;
            inline bool same_direction(const vector3d& other) const;
            inline bool same_direction(vector3d&& other) const;
            inline void normalize();
            //math operations
            inline void operator/=(T s);
            inline void operator*=(T s);
            inline void operator-=(T s);
            inline void operator+=(T s);
            inline vector3d operator-() const;

            template <typename G> friend vector3d<G> operator/(const vector3d<G>& v, G s);
            template <typename G> friend vector3d<G> operator*(const vector3d<G>& v, G s);
            template <typename G> friend vector3d<G> operator-(const vector3d<G>& v, G s);
            template <typename G> friend vector3d<G> operator+(const vector3d<G>& v, G s);
            template <typename G> friend vector3d<G> operator/(const vector3d<G>& v, const vector3d<G>& v3);
            template <typename G> friend vector3d<G> operator*(const vector3d<G>& v, const vector3d<G>& v3);
            template <typename G> friend vector3d<G> operator+(const vector3d<G>& v, const vector3d<G>& v3);
            template <typename G> friend vector3d<G> operator-(const vector3d<G>& v, const vector3d<G>& v3);
            template <typename G> friend std::ostream& operator<<(const std::ostream& os, const vector3d<G>& v);


            inline void operator/=(const vector3d& v);
            inline void operator*=(const vector3d& v);
            inline void operator-=(const vector3d& v);
            inline void operator+=(const vector3d& v);

            static inline vector3d max(const vector3d& v, const vector3d& v3);
            static inline vector3d min(const vector3d& v, const vector3d& v3);

    };
    template <typename T> inline vector3d<T> vector3d<T>::operator-() const
    {
        return *this * (T)-1;
    }
    template <typename T> inline bool vector3d<T>::same_direction(vector3d<T>&& other) const
    {
        return dot(other) > 0;
    }
    template <typename T> inline bool vector3d<T>::same_direction(const vector3d<T>& other) const
    {
        return dot(other) > 0;
    }
    template <typename T> inline T vector3d<T>::norm() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    template <typename T> inline T vector3d<T>::sq_norm() const
    {
        return x * x + y * y + z * z;
    }
    template <typename T> inline T vector3d<T>::dot(const vector3d<T>& other) const 
    {
        return x * other.x + y * other.y + z * other.z;
    }
    template <typename T> inline vector3d<T> vector3d<T>::cross(const vector3d<T>& other) const
    {
        return vector3d<T>(y * other.z  - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }
    template <typename T> inline vector3d<T> operator/(const vector3d<T>& v, T s)
    {
        return vector3d<T>(v.x/s, v.y/s, v.z/s);
    }
    template <typename T> inline vector3d<T> operator*(const vector3d<T>& v, T s)
    {
        return vector3d<T>(v.x*s, v.y*s, v.z * s);
    }
    template <typename T> inline vector3d<T> operator+(const vector3d<T>& v, T s)
    {
        return vector3d<T>(v.x+s, v.y+s, v.z + s);
    }
    template <typename T> inline vector3d<T> operator-(const vector3d<T>& v, T s)
    {
        return vector3d<T>(v.x-s, v.y-s, v.z - s);
    }
    template <typename T> inline vector3d<T> operator/(const vector3d<T>& v, const vector3d<T>& v3)
    {
        return vector3d<T>(v.x / v3.x, v.y / v3.y, v.z / v3.z);
    }
    template <typename T> inline vector3d<T> operator*(const vector3d<T>& v, const vector3d<T>& v3)
    {
        return vector3d<T>(v.x * v3.x, v.y * v3.y, v.z * v3.z);
    }
    template <typename T> inline vector3d<T> operator+(const vector3d<T>& v, const vector3d<T>& v3)
    {
        return vector3d<T>(v.x + v3.x, v.y + v3.y, v.z + v3.z);
    }
    template <typename T> inline vector3d<T> operator-(const vector3d<T>& v, const vector3d<T>& v3)
    {
        return vector3d<T>(v.x - v3.x, v.y - v3.y, v.z - v3.z);
    }

    template <typename T> inline void vector3d<T>::operator/=(T s)
    {
        x /= s;
        y /= s;
        z /= s;
    }
    template <typename T> inline void vector3d<T>::operator*=(T s)
    {
        x *= s;
        y *= s;
        z *= s;
    }
    template <typename T> inline void vector3d<T>::operator-=(T s)
    {
        x -= s;
        y -= s;
        z -= s;
    }
    template <typename T> inline void vector3d<T>::operator+=(T s)
    {
        x += s;
        y += s;
        z += s;
    }

    template <typename T> inline void vector3d<T>::operator/=(const vector3d<T>& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
    }
    template <typename T> inline void vector3d<T>::operator*=(const vector3d<T>& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }
    template <typename T> inline void vector3d<T>::operator-=(const vector3d<T>& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }
    template <typename T> inline void vector3d<T>::operator+=(const vector3d<T>& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    template <typename T> inline vector3d<T> vector3d<T>::normalized() const
    {
       return *this / norm();
    }
    template <typename T> inline void vector3d<T>::normalize()
    {
       *this = *this / norm();
    }

    template <typename T> inline vector3d<T> vector3d<T>::max(const vector3d<T>& v, const vector3d<T>& v3)
    {
        return vector3d<T>(std::max(v.x, v3.x), std::max(v.y, v3.y), std::max(v.z, v3.z));
    }

    template <typename T> inline vector3d<T> vector3d<T>::min(const vector3d<T>& v, const vector3d<T>& v3)
    {
        return vector3d<T>(std::min(v.x, v3.x), std::min(v.y, v3.y), std::min(v.z, v3.z));
    }

    template <typename T> inline std::ostream& operator<<(std::ostream& os, const vector3d<T>& v)
    {
        os << "(" << v.x  << ", " << v.y  << ", " << v.z<< ")";
        return os;
    }

}}
#endif
