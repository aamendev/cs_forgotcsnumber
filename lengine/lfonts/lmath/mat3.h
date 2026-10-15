#ifndef MAT_3_H
#define MAT_3_H
#include "vector3d.h"
#include <array>
namespace lina { namespace math { 
    template <typename T>
        class mat3 
        {
            public:
                mat3(): rs({(std::array<T, 3>){1.0, 0., 0.}, {0., 1., 0.}, {0., 0., 1.}}){}
                mat3(vector3d<T>&& ro1, vector3d<T>&& ro2, vector3d<T>&& ro3): 
                    rs({(std::array<T, 3>){ro1.x, ro1.y, ro1.z}, {ro2.x, ro2.y, ro2.z}, {ro3.x, ro3.y, ro3.z}}){}
            public:
                static inline mat3 get_identity();
                static inline mat3 get_zero();

                inline vector3d<T> row(i32 i) const;
                inline vector3d<T> col(i32 i) const;

                inline void set_row(i32 i, vector3d<T>&&);
                inline void set_col(i32 i,vector3d<T>&&);
                inline T det() const;

                inline mat3 get_inverse() const;
                inline void transpose();
                inline mat3 get_transpose() const;
                inline void zero();
                inline void identity();

                template <typename G> friend mat3<G> operator/(const mat3<G>& m, G s);
                template <typename G> friend mat3<G> operator*(const mat3<G>& m, G s);
                template <typename G> friend mat3<G> operator*(const mat3<G>& m, const mat3<G>& m2);
                template <typename G> friend mat3<G> operator+(const mat3<G>& m, const mat3<G>& m2);
                template <typename G> friend vector3d<G> operator*(const mat3<G>& m, const vector3d<G>& v);
                template <typename G> friend std::ostream& operator<<(std::ostream& os, const mat3<G>& v);
                inline T& operator()(i32 row, i32 col) {return rs[row][col];}
            private:
                inline T& at_pos(i32 row, i32 col) {return rs[row][col];}
            private:
                std::array<std::array<T, 3>, 3> rs;
        };
    template <typename T>
        inline vector3d<T> mat3<T>::row(i32 i) const
        {
            return vector3d<T>(rs[i][0], rs[i][1], rs[i][2]);
        }
    template <typename T>
        inline vector3d<T> mat3<T>::col(i32 i) const
        {
            return vector3d<T>(rs[0][i], rs[1][i], rs[2][i]);
        }
    template <typename T>
        inline void mat3<T>::set_row(i32 i, vector3d<T>&& v)
        {
            rs[i][0] = v.x;
            rs[i][1] = v.y;
            rs[i][2] = v.z;
        }

    template <typename T>
        inline void mat3<T>::set_col(i32 i, vector3d<T>&& v)
        {
            rs[0][i] = v.x;
            rs[1][i] = v.y;
            rs[2][i] = v.z;
        }

    template <typename T>
        inline T mat3<T>::det() const
        {
            return rs[0][0] * (rs[1][1] * rs[2][2] - rs[1][2] * rs[2][1])
                + rs[0][1] * (rs[1][2] * rs[2][0] - rs[1][0] * rs[2][2])
                + rs[0][2] * (rs[1][0] * rs[2][1] - rs[1][1] * rs[2][0]);
        }
    template <typename T>
        inline void mat3<T>::transpose()
        {
            *this = get_transpose();
        }
    template <typename T>
        inline mat3<T> mat3<T>::get_transpose() const
        {
            return mat3<T>(col(0), col(1), col(2));
        }
    template <typename T>
        inline mat3<T> mat3<T>::get_inverse() const
        {
            const vector3d<T>& v0 = row(0);
            const vector3d<T>& v1 = row(1);
            const vector3d<T>& v2 = row(2);
            vector3d<T> r0 = v2.cross(v1);
            vector3d<T> r1 = v0.cross(v2);
            vector3d<T> r2 = v1.cross(v0);
            T inv_det = 1 / r2.dot(v2);
            return mat3<T>(
                    vector3d<T>(r0.x *inv_det, r1.x * inv_det, r2.x * inv_det),
                    vector3d<T>(r0.y *inv_det, r1.y * inv_det, r2.y * inv_det),
                    vector3d<T>(r0.z *inv_det, r1.z * inv_det, r2.z * inv_det)
                    );
        }
    template <typename T>
        inline void mat3<T>::zero()
        {
            rs.fill({0, 0, 0});
        }
    template <typename T>
        inline void mat3<T>::identity()
        {
            rs[0] = {1., 0., 0.};
            rs[1] = {0., 1., 0.};
            rs[2] = {0., 0., 1.};
        }
    template <typename T>
        inline mat3<T> mat3<T>::get_identity()
        {
            return mat3<T>(vector3d<T>(1., 0., 0.), vector3d<T>(0., 1., 0.), vector3d<T>(0., 0., 1.));
        }
    template <typename T>
        inline mat3<T> mat3<T>::get_zero()
        {
            return mat3<T>(vector3d<T>(0., 0., 0.), vector3d<T>(0., 0., 0.), vector3d<T>(0., 0., 0.));
        }
    template <typename T>
        inline mat3<T> operator/(const mat3<T>& m, T s)
        {
            return mat3<T>(m.row(0)/s, m.row(1)/s, m.row(2)/s);
        }
    template <typename T>
        inline mat3<T> operator*(const mat3<T>& m, T s)
        {
            return mat3<T>(m.row(0)*s, m.row(1)*s, m.row(2) * s);
        }
    template <typename T>
        inline vector3d<T> operator*(const mat3<T>& m, const vector3d<T>& v)
        {
            return vector3d<T>(m.row(0).dot(v), m.row(1).dot(v), m.row(2).dot(v));
        }
    template <typename T>
        inline mat3<T> operator*(const mat3<T>& m, const mat3<T>& m2)
        {
            return mat3<T>(
                    vector3d<T>(
                        m.row(0).dot(m2.col(0)),
                        m.row(0).dot(m2.col(1)),
                        m.row(0).dot(m2.col(2))
                        ),
                    vector3d<T>(
                        m.row(1).dot(m2.col(0)),
                        m.row(1).dot(m2.col(1)),
                        m.row(1).dot(m2.col(2))
                        ),
                    vector3d<T>(
                        m.row(2).dot(m2.col(0)),
                        m.row(2).dot(m2.col(1)),
                        m.row(2).dot(m2.col(2))
                        ));
        }
    template <typename T>
        inline mat3<T> operator+(const mat3<T>& m, const mat3<T>& m2)
        {
            return mat3<T>(
                    m.row(0) + m2.row(0),
                    m.row(1) + m2.row(1),
                    m.row(2) + m2.row(2)
                    );
        }
    template <typename T>
        inline std::ostream& operator<<(std::ostream& os, const mat3<T>& m)
        {
            os << "{\n" << m.row(0) << '\n' << m.row(1) << '\n' << m.row(2) << "\n}";
            return os;
        }
}}
#endif
