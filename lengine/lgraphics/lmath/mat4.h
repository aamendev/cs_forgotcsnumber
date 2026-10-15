#ifndef MAT_4_H
#define MAT_4_H
#include "vector3d.h"
#include "vector4d.h"
#include <array>
namespace lina { namespace math { 
    template <typename T>
        class mat4 
        {
            public:
                mat4(): rs({(std::array<T, 4>){1.0, 0., 0., 0.}, {0., 1., 0., 0.}, {0., 0., 1., 0.}, {0., 0., 0., 1.}}){}
                mat4(vector4d<T>&& ro1, vector4d<T>&& ro2, vector4d<T>&& ro3, vector4d<T>&& ro4): 
                    rs({(std::array<T, 4>)
                            {ro1.x, ro1.y, ro1.z, ro1.w}, 
                            {ro2.x, ro2.y, ro2.z, ro2.w},
                            {ro3.x, ro3.y, ro3.z, ro3.w},
                            {ro4.x, ro4.y, ro4.z, ro4.w},
                            }){}
            public:
                static inline mat4 get_identity();
                static inline mat4 get_zero();

                inline vector4d<T> row(i32 i) const;
                inline void set_row(i32 i, vector4d<T>&& v);
                inline vector4d<T> col(i32 i) const;
                inline void set_col(i32 i, vector4d<T>&& v);
                inline T det() const;

                inline mat4 get_inverse() const;
                inline void transpose();
                inline mat4 get_transpose() const;
                inline void zero();
                inline void identity();

                template <typename G> friend mat4<G> operator/(const mat4<G>& m, G s);
                template <typename G> friend mat4<G> operator*(const mat4<G>& m, G s);
                template <typename G> friend mat4<G> operator*(const mat4<G>& m, const mat4<G>& m2);
                template <typename G> friend mat4<G> operator+(const mat4<G>& m, const mat4<G>& m2);
                template <typename G> friend vector4d<G> operator*(const mat4<G>& m, const vector4d<G>& v);
                template <typename G> friend vector3d<G> operator*(const mat4<G>& m, const vector3d<G>& v);
                template <typename G> friend vector4d<G> operator*(const vector4d<G>& v, const mat4<G>& m);
                template <typename G> friend vector3d<G> operator*(const vector3d<G>& v, const mat4<G>& m);
                template <typename G> friend std::ostream& operator<<(std::ostream& os, const mat4<G>& v);
                inline T& operator()(i32 row, i32 col) {return rs[row][col];}
            private:
                inline T& at_pos(i32 row, i32 col) {return rs[row][col];}
            private:
                std::array<std::array<T, 4>, 4> rs;
        };
    template <typename T>
        inline vector4d<T> mat4<T>::row(i32 i) const
        {
            return vector4d<T>(rs[i][0], rs[i][1], rs[i][2], rs[i][3]);
        }
    template <typename T>
        inline void mat4<T>::set_row(i32 i, vector4d<T>&& v)
        {
            rs[i][0] = v.x;
            rs[i][1] = v.y;
            rs[i][2] = v.z;
            rs[i][3] = v.w;
        }
    template <typename T>
        inline vector4d<T> mat4<T>::col(i32 i) const
        {
            return vector4d<T>(rs[0][i], rs[1][i], rs[2][i], rs[3][i]);
        }
    template <typename T>
        inline void mat4<T>::set_col(i32 i, vector4d<T>&& v)
        {
            rs[0][i] = v.x;
            rs[1][i] = v.y;
            rs[2][i] = v.z;
            rs[3][i] = v.w;
        }

    template <typename T>
        inline T mat4<T>::det() const
        {
            vector4d<T> temp = col(0);
            const vector3d<T> v0(temp.x, temp.y, temp.z);

            temp = col(1);
            const vector3d<T> v1(temp.x, temp.y, temp.z);

            temp = col(2);
            const vector3d<T> v2(temp.x, temp.y, temp.z);

            temp = col(3);
            const vector3d<T> v3(temp.x, temp.y, temp.z);

            const vector4d<T> v4 = vector4d<T>(rs[3][0], rs[3][1], rs[3][2], rs[3][3]);

            vector3d<T> s = v0.cross(v1);
            vector3d<T> t = v2.cross(v3);
            vector3d<T> u = v0 * v4.y - v1 * v4.x;
            vector3d<T> v = v2 * v4.w - v3 * v4.z;
            return (s.dot(v) + t.dot(u));
        }
    template <typename T>
        inline void mat4<T>::transpose()
        {
            *this = get_transpose();
        }
    template <typename T>
        inline mat4<T> mat4<T>::get_transpose() const
        {
            return mat4<T>(col(0), col(1), col(2), col(3));
        }
    template <typename T>
        inline mat4<T> mat4<T>::get_inverse() const
        {
            vector4d<T> temp = col(0);
            const vector3d<T> v0(temp.x, temp.y, temp.z);

            temp = col(1);
            const vector3d<T> v1(temp.x, temp.y, temp.z);

            temp = col(2);
            const vector3d<T> v2(temp.x, temp.y, temp.z);

            temp = col(3);
            const vector3d<T> v3(temp.x, temp.y, temp.z);

            const vector4d<T> v4 = vector4d<T>(rs[3][0], rs[3][1], rs[3][2], rs[3][3]);

            vector3d<T> s = v0.cross(v1);
            vector3d<T> t = v2.cross(v3);
            vector3d<T> u = v0 * v4.y - v1 * v4.x;
            vector3d<T> v = v2 * v4.w - v3 * v4.z;
            T inv_det = 1 / (s.dot(v) + t.dot(u));
            s = s * inv_det;
            u = u * inv_det;
            v = v * inv_det;
            t = t * inv_det;

            vector3d<T> r0 = v1.cross(v) + t * v4.y;
            vector3d<T> r1 = v.cross(v0) - t * v4.x;
            vector3d<T> r2 = v3.cross(u) + s * v4.w;
            vector3d<T> r3 = u.cross(v2) - s * v4.z;

            return mat4<T> (
                    vector4d<T>(
                        r0.x, r0.y, r0.z, - v1.dot(t)
                        ),
                    vector4d<T>(
                        r1.x, r1.y, r1.z, v0.dot(t)
                        ),
                    vector4d<T>(
                        r2.x, r2.y, r2.z, - v3.dot(s)
                        ),
                    vector4d<T>(
                        r3.x, r3.y, r3.z, v2.dot(s)
                        )
                    );
        }
    template <typename T>
        inline void mat4<T>::zero()
        {
            rs.fill({0, 0, 0, 0});
        }
    template <typename T>
        inline void mat4<T>::identity()
        {
            rs[0] = {1., 0., 0., 0.};
            rs[1] = {0., 1., 0., 0.};
            rs[2] = {0., 0., 1., 0.};
            rs[3] = {0., 0., 0., 1.};
        }
    template <typename T>
        inline mat4<T> mat4<T>::get_identity()
        {
            return mat4<T>(
                    vector4d<T>(1., 0., 0., 0.), 
                    vector4d<T>(0., 1., 0., 0.), 
                    vector4d<T>(0., 0., 1., 0.),
                    vector4d<T>(0., 0., 0., 1.)
                    );
        }
    template <typename T>
        inline mat4<T> mat4<T>::get_zero()
        {
            return mat4<T>(
                    vector4d<T>(0., 0., 0., 0.), 
                    vector4d<T>(0., 0., 0., 0.),
                    vector4d<T>(0., 0., 0., 0.),
                    vector4d<T>(0., 0., 0., 0.)
                    );
        }
    template <typename T>
        inline mat4<T> operator/(const mat4<T>& m, T s)
        {
            return mat4<T>(m.row(0)/s, m.row(1)/s, m.row(2)/s, m.row(3)/s);
        }
    template <typename T>
        inline mat4<T> operator*(const mat4<T>& m, T s)
        {
            return mat4<T>(m.row(0)*s, m.row(1)*s, m.row(2) * s, m.row(3) * s);
        }
    template <typename T>
        inline vector4d<T> operator*(const mat4<T>& m, const vector4d<T>& v)
        {
            return vector4d<T>(m.row(0).dot(v), m.row(1).dot(v), m.row(2).dot(v), m.row(3).dot(v));
        }
    template <typename T>
        inline vector3d<T> operator*(const mat4<T>& m, const vector3d<T>& v)
        {
            auto new_v = vector4d<T>(v.x, v.y, v.z, 0.);
            new_v = vector4d<T>(m.row(0).dot(new_v), m.row(1).dot(new_v), m.row(2).dot(new_v), m.row(3).dot(new_v));
            return vector3d<T>(new_v.x, new_v.y, new_v.z);
        }

    template <typename T>
        inline vector4d<T> operator*(const vector4d<T>& v, const mat4<T>& m)
        {
            return m * v;
        }
    template <typename T>
        inline vector3d<T> operator*(const vector3d<T>& v, const mat4<T>& m)
        {
            return m * v;
        }

    template <typename T>
        inline mat4<T> operator*(const mat4<T>& m, const mat4<T>& m2)
        {
            return mat4<T>(
                    vector4d<T>(
                        m.row(0).dot(m2.col(0)),
                        m.row(0).dot(m2.col(1)),
                        m.row(0).dot(m2.col(2)),
                        m.row(0).dot(m2.col(3))
                        ),
                    vector4d<T>(
                        m.row(1).dot(m2.col(0)),
                        m.row(1).dot(m2.col(1)),
                        m.row(1).dot(m2.col(2)),
                        m.row(1).dot(m2.col(3))
                        ),
                    vector4d<T>(
                        m.row(2).dot(m2.col(0)),
                        m.row(2).dot(m2.col(1)),
                        m.row(2).dot(m2.col(2)),
                        m.row(2).dot(m2.col(3))
                        ),
                    vector4d<T>(
                        m.row(3).dot(m2.col(0)),
                        m.row(3).dot(m2.col(1)),
                        m.row(3).dot(m2.col(2)),
                        m.row(3).dot(m2.col(3))
                        )
                        );
        }
    template <typename T>
        inline mat4<T> operator+(const mat4<T>& m, const mat4<T>& m2)
        {
            return mat4<T>(
                    m.row(0) + m2.row(0),
                    m.row(1) + m2.row(1),
                    m.row(2) + m2.row(2),
                    m.row(3) + m2.row(3)
                    );
        }
    template <typename T>
        inline std::ostream& operator<<(std::ostream& os, const mat4<T>& m)
        {
            os << "{\n" << m.row(0) << '\n' << m.row(1) << '\n' << m.row(2) << '\n' << m.row(3)<< "\n}";
            return os;
        }
}}
#endif
