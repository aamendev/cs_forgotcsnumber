#ifndef MAT_2_H
#define MAT_2_H
#include "vector2d.h"
#include <array>
namespace lina { namespace math { 
    template <typename T>
        class mat2 
        {
            public:
                mat2(): rs({(std::array<T, 2>){1.0, 0.}, {0., 1.}}){}
                mat2(vector2d<T>&& ro1, vector2d<T>&& ro2): 
                    rs({(std::array<T, 2>){ro1.x, ro1.y}, {ro2.x, ro2.y}}){}
            public:
                static inline mat2 get_identity();
                static inline mat2 get_zero();

                inline vector2d<T> row(i32 i) const;
                inline void set_row(i32 i, vector2d<T>&& v);
                inline vector2d<T> col(i32 i) const;
                inline void set_col(i32 i, vector2d<T>&& v);
                inline T det() const;

                inline mat2 get_inverse() const;
                inline void transpose();
                inline mat2 get_transpose() const;
                inline void zero();
                inline void identity();

                template <typename G>
                inline friend mat2<G> operator/(const mat2<G>& m, G s);
                template <typename G>
                inline friend mat2<G> operator*(const mat2<G>& m, G s);
                template <typename G>
                inline friend mat2<G> operator*(const mat2<G>& m, const mat2<G>& m2);
                template <typename G>
                inline friend mat2<G> operator+(const mat2<G>& m, const mat2<G>& m2);
                template <typename G>
                inline friend vector2d<G> operator*(const mat2<G>& m, const vector2d<G>& v);
                template <typename G>
                inline friend std::ostream& operator<<(std::ostream& os, const mat2<G>& v);
                inline T& operator()(i32 row, i32 col) {return rs[row][col];}
            private:
                std::array<std::array<T, 2>, 2> rs;
        };
    template <typename T>
        inline vector2d<T> mat2<T>::row(i32 i) const
        {
            return vector2d<T>(rs[i][0], rs[i][1]);
        }
    template <typename T>
        inline void mat2<T>::set_row(i32 i, vector2d<T>&& v)
        {
            rs[i][0] = v.x;
            rs[i][1] = v.y;
        }
    template <typename T>
        inline vector2d<T> mat2<T>::col(i32 i) const
        {
            return vector2d<T>(rs[0][i], rs[1][i]);
        }
    template <typename T>
        inline void mat2<T>::set_col(i32 i, vector2d<T>&& v)
        {
            rs[0][i] = v.x;
            rs[1][i] = v.y;
        }
    template <typename T>
        inline T mat2<T>::det() const
        {
            return row(0).x * row(1).y - row(0).y * row(1).x;
        }
    template <typename T>
        inline void mat2<T>::transpose()
        {
            rs = (std::array<std::array<T, 2>, 2>){(std::array<T, 2>){row(0).x, row(1).x}, (std::array<T, 2>){row(0).y, row(1).y}};
        }
    template <typename T>
        inline mat2<T> mat2<T>::get_transpose() const
        {
            return mat2<T>(vector2d<T>(row(0).x, row(1).x), vector2d<T>(row(0).y, row(1).y));
        }
    template <typename T>
        inline mat2<T> mat2<T>::get_inverse() const
        {
            return  mat2<T>(vector2d<T>(rs[1][1], - rs[0][1]), vector2d<T>(-rs[1][0], rs[0][0])) * (1. / det());
        }
    template <typename T>
        inline void mat2<T>::zero()
        {
            rs.fill({0, 0});
        }
    template <typename T>
        inline void mat2<T>::identity()
        {
            rs[0][0] = 1.;
            rs[0][1] = 0.;
            rs[1][0] = 0.;
            rs[1][1] = 1.;
        }
    template <typename T>
        inline mat2<T> mat2<T>::get_identity()
        {
            return mat2<T>(vector2d<T>(1., 0.), vector2d<T>(0., 1.));
        }
    template <typename T>
        inline mat2<T> mat2<T>::get_zero()
        {
            return mat2<T>(vector2d<T>(0., 0.), vector2d<T>(0., 0.));
        }
    template <typename T>
        inline mat2<T> operator/(const mat2<T>& m, T s)
        {
            return mat2<T>(m.row(0)/s, m.row(1)/s);
        }
    template <typename T>
        inline mat2<T> operator*(const mat2<T>& m, T s)
        {
            return mat2<T>(m.row(0)*s, m.row(1)*s);
        }
    template <typename T>
        inline vector2d<T> operator*(const mat2<T>& m, const vector2d<T>& v)
        {
            return vector2d<T>(m.row(0).dot(v), m.row(1).dot(v));
        }
    template <typename T>
        inline mat2<T> operator*(const mat2<T>& m, const mat2<T>& m2)
        {
            return mat2<T>(
                    vector2d<T>(
                        m.row(0).x * m2.row(0).x + m.row(0).y * m2.row(1).x,
                        m.row(0).x * m2.row(0).y + m.row(0).y * m2.row(1).y
                        ), 
                    vector2d<T>(
                        m.row(1).x * m2.row(0).x + m.row(1).y * m2.row(1).x,
                        m.row(1).x * m2.row(0).y + m.row(1).y * m2.row(1).y
                        ));
        }
    template <typename T>
        inline mat2<T> operator+(const mat2<T>& m, const mat2<T>& m2)
        {
            return mat2<T>(
                    m.row(0) + m2.row(0),
                    m.row(1) + m2.row(1)
                    );
        }
    template <typename T>
        inline std::ostream& operator<<(std::ostream& os, const mat2<T>& m)
        {
            os << "{\n" << m.row(0) << '\n' << m.row(1) << "\n}";
            return os;
        }
}}
#endif
