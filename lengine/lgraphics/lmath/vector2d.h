#ifndef VECTOR_2D_H
#define VECTOR_2D_H
#include "types.h"
#include <algorithm>
#include <cmath>
#include <ostream>
namespace lina { namespace math{
    template <typename T>
    class vector2d
    {
        public:
            vector2d(): x(0), y(0){}
            vector2d(T x, T y): x(x), y(y){}
            T x;
            T y;
        public:
            // vec operations
            inline T norm() const;
            inline T sq_norm() const;
            inline T dot(const vector2d& other) const;
            inline T cross(const vector2d& other) const;
            inline vector2d normalized() const;
            inline void normalize();
            //math operations
            inline void operator/=(T s);
            inline void operator*=(T s);
            inline void operator-=(T s);
            inline void operator+=(T s);

            template <typename G> inline friend vector2d<G> operator/(const vector2d<G>& v, G s);
            template <typename G> inline friend vector2d<G> operator*(const vector2d<G>& v, G s);
            template <typename G> inline friend vector2d<G> operator-(const vector2d<G>& v, G s);
            template <typename G> inline friend vector2d<G> operator+(const vector2d<G>& v, G s);
            template <typename G> inline friend vector2d<G> operator/(const vector2d<G>& v, const vector2d<G>& v2);
            template <typename G> inline friend vector2d<G> operator*(const vector2d<G>& v, const vector2d<G>& v2);
            template <typename G> inline friend vector2d<G> operator+(const vector2d<G>& v, const vector2d<G>& v2);
            template <typename G> inline friend vector2d<G> operator-(const vector2d<G>& v, const vector2d<G>& v2);
            template <typename G> inline friend std::ostream& operator<<(const std::ostream& os, const vector2d<G>& v);

            inline void operator/=(const vector2d& v);
            inline void operator*=(const vector2d& v);
            inline void operator-=(const vector2d& v);
            inline void operator+=(const vector2d& v);


            static inline vector2d max(const vector2d& v, const vector2d& v2);
            static inline vector2d min(const vector2d& v, const vector2d& v2);

    };
    template <typename T> inline T vector2d<T>::norm() const
    {
        return std::sqrt(x * x + y * y);
    }
    template <typename T> inline T vector2d<T>::sq_norm() const
    {
        return x * x + y * y;
    }
    template <typename T> inline T vector2d<T>::dot(const vector2d<T>& other) const 
    {
        return x * other.x + y * other.y;
    }
    template <typename T> inline T vector2d<T>::cross(const vector2d<T>& other) const
    {
        return x * other.y  - y * other.x;
    }
    template <typename T> inline vector2d<T> operator/(const vector2d<T>& v, T s)
    {
        return vector2d<T>(v.x/s, v.y/s);
    }
    template <typename T> inline vector2d<T> operator*(const vector2d<T>& v, T s)
    {
        return vector2d<T>(v.x*s, v.y*s);
    }
    template <typename T> inline vector2d<T> operator+(const vector2d<T>& v, T s)
    {
        return vector2d<T>(v.x+s, v.y+s);
    }
    template <typename T> inline vector2d<T> operator-(const vector2d<T>& v, T s)
    {
        return vector2d<T>(v.x-s, v.y-s);
    }
    template <typename T> inline vector2d<T> operator/(const vector2d<T>& v, const vector2d<T>& v2)
    {
        return vector2d<T>(v.x / v2.x, v.y / v2.y);
    }
    template <typename T> inline vector2d<T> operator*(const vector2d<T>& v, const vector2d<T>& v2)
    {
        return vector2d<T>(v.x * v2.x, v.y * v2.y);
    }
    template <typename T> inline vector2d<T> operator+(const vector2d<T>& v, const vector2d<T>& v2)
    {
        return vector2d<T>(v.x + v2.x, v.y + v2.y);
    }
    template <typename T> inline vector2d<T> operator-(const vector2d<T>& v, const vector2d<T>& v2)
    {
        return vector2d<T>(v.x - v2.x, v.y - v2.y);
    }

    template <typename T> inline void vector2d<T>::operator/=(T s)
    {
        x /= s;
        y /= s;
    }
    template <typename T> inline void vector2d<T>::operator*=(T s)
    {
        x *= s;
        y *= s;
    }
    template <typename T> inline void vector2d<T>::operator-=(T s)
    {
        x -= s;
        y -= s;
    }
    template <typename T> inline void vector2d<T>::operator+=(T s)
    {
        x += s;
        y += s;
    }

    template <typename T> inline void vector2d<T>::operator/=(const vector2d<T>& v)
    {
        x /= v.x;
        y /= v.y;
    }
    template <typename T> inline void vector2d<T>::operator*=(const vector2d<T>& v)
    {
        x *= v.x;
        y *= v.y;
    }
    template <typename T> inline void vector2d<T>::operator-=(const vector2d<T>& v)
    {
        x -= v.x;
        y -= v.y;
    }
    template <typename T> inline void vector2d<T>::operator+=(const vector2d<T>& v)
    {
        x += v.x;
        y += v.y;
    }
    template <typename T> inline vector2d<T> vector2d<T>::normalized() const
    {
       return *this / norm();
    }
    template <typename T> inline void vector2d<T>::normalize()
    {
       *this = *this / norm();
    }

    template <typename T> inline vector2d<T> vector2d<T>::max(const vector2d<T>& v, const vector2d<T>& v2)
    {
        return vector2d<T>(std::max(v.x, v2.x), std::max(v.y, v2.y));
    }

    template <typename T> inline vector2d<T> vector2d<T>::min(const vector2d<T>& v, const vector2d<T>& v2)
    {
        return vector2d<T>(std::min(v.x, v2.x), std::min(v.y, v2.y));
    }

    template <typename T> inline std::ostream& operator<<(std::ostream& os, const vector2d<T>& v)
    {
        os << "(" << v.x  << ", " << v.y << ")";
        return os;
    }

}}
#endif
