#ifndef VECTOR_4D_H
#define VECTOR_4D_H
#include "types.h"
#include <algorithm>
#include <cmath>
#include <ostream>
namespace lina { namespace math{
    template <typename T>
    class vector4d
    {
        public:
            vector4d(T x, T y, T z, T w): x(x), y(y), z(z), w(w){}
            vector4d(): x(0), y(0), z(0), w(0){}
            T x;
            T y;
            T z;
            T w;
        public:
            // vec operations
            inline T norm() const;
            inline T sq_norm() const;
            inline T dot(const vector4d& other) const;
            inline vector4d cross(const vector4d& other) const;
            inline vector4d normalized() const;
            inline void normalize();
            //math operations
            inline void operator/=(T s);
            inline void operator*=(T s);
            inline void operator-=(T s);
            inline void operator+=(T s);
            inline vector4d operator-() const;

            template <typename G> inline friend vector4d<G> operator/(const vector4d<G>& v, G s);
            template <typename G> inline friend vector4d<G> operator*(const vector4d<G>& v, G s);
            template <typename G> inline friend vector4d<G> operator-(const vector4d<G>& v, G s);
            template <typename G> inline friend vector4d<G> operator+(const vector4d<G>& v, G s);
            template <typename G> inline friend vector4d<G> operator/(const vector4d<G>& v, const vector4d<G>& v4);
            template <typename G> inline friend vector4d<G> operator*(const vector4d<G>& v, const vector4d<G>& v4);
            template <typename G> inline friend vector4d<G> operator+(const vector4d<G>& v, const vector4d<G>& v4);
            template <typename G> inline friend vector4d<G> operator-(const vector4d<G>& v, const vector4d<G>& v4);
            template <typename G> inline friend std::ostream& operator<<(const std::ostream& os, const vector4d<G>& v);


            inline void operator/=(const vector4d& v);
            inline void operator*=(const vector4d& v);
            inline void operator-=(const vector4d& v);
            inline void operator+=(const vector4d& v);

            static inline vector4d max(const vector4d& v, const vector4d& v4);
            static inline vector4d min(const vector4d& v, const vector4d& v4);

    };
    template <typename T> inline vector4d<T> vector4d<T>::operator-() const
    {
        return *this * (T)-1;
    }
    template <typename T> inline T vector4d<T>::norm() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    template <typename T> inline T vector4d<T>::sq_norm() const
    {
        return x * x + y * y + z * z;
    }
    template <typename T> inline T vector4d<T>::dot(const vector4d<T>& other) const 
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    template <typename T> inline vector4d<T> vector4d<T>::cross(const vector4d<T>& other) const
    {
        return vector4d<T>(y * other.z  - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x, 1.);
    }
    template <typename T> inline vector4d<T> operator/(const vector4d<T>& v, T s)
    {
        return vector4d<T>(v.x/s, v.y/s, v.z/s, v.w/s);
    }
    template <typename T> inline vector4d<T> operator*(const vector4d<T>& v, T s)
    {
        return vector4d<T>(v.x*s, v.y*s, v.z * s, v.w * s);
    }
    template <typename T> inline vector4d<T> operator+(const vector4d<T>& v, T s)
    {
        return vector4d<T>(v.x+s, v.y+s, v.z + s, v.w + s);
    }
    template <typename T> inline vector4d<T> operator-(const vector4d<T>& v, T s)
    {
        return vector4d<T>(v.x-s, v.y-s, v.z - s, v.w - s);
    }
    template <typename T> inline vector4d<T> operator/(const vector4d<T>& v, const vector4d<T>& v4)
    {
        return vector4d<T>(v.x / v4.x, v.y / v4.y, v.z / v4.z, v.w / v4.w);
    }
    template <typename T> inline vector4d<T> operator*(const vector4d<T>& v, const vector4d<T>& v4)
    {
        return vector4d<T>(v.x * v4.x, v.y * v4.y, v.z * v4.z, v.w * v4.w);
    }
    template <typename T> inline vector4d<T> operator+(const vector4d<T>& v, const vector4d<T>& v4)
    {
        return vector4d<T>(v.x + v4.x, v.y + v4.y, v.z + v4.z, v.w + v4.w);
    }
    template <typename T> inline vector4d<T> operator-(const vector4d<T>& v, const vector4d<T>& v4)
    {
        return vector4d<T>(v.x - v4.x, v.y - v4.y, v.z - v4.z, v.w - v4.w);
    }

    template <typename T> inline void vector4d<T>::operator/=(T s)
    {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
    }
    template <typename T> inline void vector4d<T>::operator*=(T s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
    }
    template <typename T> inline void vector4d<T>::operator-=(T s)
    {
        x -= s;
        y -= s;
        z -= s;
        w -= s;
    }
    template <typename T> inline void vector4d<T>::operator+=(T s)
    {
        x += s;
        y += s;
        z += s;
        w += s;
    }

    template <typename T> inline void vector4d<T>::operator/=(const vector4d<T>& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
    }
    template <typename T> inline void vector4d<T>::operator*=(const vector4d<T>& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
    }
    template <typename T> inline void vector4d<T>::operator-=(const vector4d<T>& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
    }
    template <typename T> inline void vector4d<T>::operator+=(const vector4d<T>& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
    }
    template <typename T> inline vector4d<T> vector4d<T>::normalized() const
    {
       return *this / norm();
    }
    template <typename T> inline void vector4d<T>::normalize()
    {
       *this = *this / norm();
    }

    template <typename T> inline vector4d<T> vector4d<T>::max(const vector4d<T>& v, const vector4d<T>& v4)
    {
        return vector4d<T>(std::max(v.x, v4.x), std::max(v.y, v4.y), std::max(v.z, v4.z), std::max(v.w, v4.w));
    }

    template <typename T> inline vector4d<T> vector4d<T>::min(const vector4d<T>& v, const vector4d<T>& v4)
    {
        return vector4d<T>(std::min(v.x, v4.x), std::min(v.y, v4.y), std::min(v.z, v4.z), std::min(v.w, v4.w));
    }

    template <typename T> inline std::ostream& operator<<(std::ostream& os, const vector4d<T>& v)
    {
        os << "(" << v.x  << ", " << v.y  << ", " << v.z << ", " << v.w << ")";
        return os;
    }

}}
#endif
