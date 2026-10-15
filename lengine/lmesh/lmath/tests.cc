#include "tests.h"
#include "poly.h"
#include "transformations.h"
#include "utils.h"
#include "vector2d.h"
#include "vector3d.h"
#include "vector4d.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
namespace lina { namespace math { namespace tests{
    void v2d_tests()
    {
        BEGIN_TEST("vec2");
        vector2d<f64> v1(0., 2.);

        assert(utils::f64_eq(v1.x, 0.));
        assert(utils::f64_eq(v1.y, 2.));

        v1.x = 5;
        v1.y = 5;

        vector2d<f64> v2(3., 4.);

        f64 d = v1.dot(v2);
        f64 c = v1.cross(v2);

        assert(utils::f64_eq(d, 35.));
        assert(utils::f64_eq(c, 5.));
        assert(utils::f64_eq(v1.sq_norm(), 50));
        assert(utils::f64_eq(v1.norm(), std::sqrt(50)));

        v1 += 2;
        assert(utils::f64_eq(v1.x, 7.));
        assert(utils::f64_eq(v1.y, 7.));

        auto v = v1 / v2;

        assert(utils::f64_eq(v.x, 7./3.));
        assert(utils::f64_eq(v.y, 7./4.));

        v2.x = 10.;

        v = vector2d<f64>::max(v1, v2);
        assert(utils::f64_eq(v.x, 10.));
        assert(utils::f64_eq(v.y, 7.));
        v = vector2d<f64>::min(v1, v2);
        assert(utils::f64_eq(v.x, 7.));
        assert(utils::f64_eq(v.y, 4.));

        v = v1.normalized();
        assert(utils::f64_eq(v.x, 1./sqrt(2)));
        assert(utils::f64_eq(v.y, 1./sqrt(2)));

        v1.normalize();
        assert(utils::f64_eq(v1.x, 1./sqrt(2)));
        assert(utils::f64_eq(v1.y, 1./sqrt(2)));
        END_TEST("vec2");
    }
    void v3d_tests()
    {
        BEGIN_TEST("vec3");
        vector3d<f64> v1(0., 2., 3.);

        assert(utils::f64_eq(v1.x, 0.));
        assert(utils::f64_eq(v1.y, 2.));
        assert(utils::f64_eq(v1.z, 3.));

        v1.x = 5;
        v1.y = 5;
        v1.z = 5;

        vector3d<f64> v2(3., 4., 5.);

        f64 d = v1.dot(v2);

        assert(utils::f64_eq(d, 60.));
        assert(utils::f64_eq(v1.sq_norm(), 75));
        assert(utils::f64_eq(v1.norm(), std::sqrt(75)));

        v1 += 2;
        assert(utils::f64_eq(v1.x, 7.));
        assert(utils::f64_eq(v1.y, 7.));
        assert(utils::f64_eq(v1.z, 7.));

        auto v = v1 / v2;

        assert(utils::f64_eq(v.x, 7./3.));
        assert(utils::f64_eq(v.y, 7./4.));
        assert(utils::f64_eq(v.z, 7./5.));

        v2.x = 10.;

        v = vector3d<f64>::max(v1, v2);
        assert(utils::f64_eq(v.x, 10.));
        assert(utils::f64_eq(v.y, 7.));
        assert(utils::f64_eq(v.z, 7.));

        v = vector3d<f64>::min(v1, v2);
        assert(utils::f64_eq(v.x, 7.));
        assert(utils::f64_eq(v.y, 4.));
        assert(utils::f64_eq(v.z, 5.));

        v = v1.normalized();
        assert(utils::f64_eq(v.x, 1./sqrt(3)));
        assert(utils::f64_eq(v.y, 1./sqrt(3)));
        assert(utils::f64_eq(v.z, 1./sqrt(3)));

        v1.normalize();
        assert(utils::f64_eq(v1.x, 1./sqrt(3)));
        assert(utils::f64_eq(v1.y, 1./sqrt(3)));
        assert(utils::f64_eq(v1.z, 1./sqrt(3)));

        v1 = vector3d<f64>(1., 0., 0.);
        v2 = vector3d<f64>(0., 1., 0.);
        auto v3 = v1.cross(v2);
        auto v4 = v2.cross(v1);

        assert(utils::f64_eq(v3.x, 0.));
        assert(utils::f64_eq(v3.y, 0.));
        assert(utils::f64_eq(v3.z, 1.));

        assert(utils::f64_eq(v4.x, 0.));
        assert(utils::f64_eq(v4.y, 0.));
        assert(utils::f64_eq(v4.z, -1.));

        END_TEST("vec3");
    }

    void mat2_tests()
    {
        BEGIN_TEST("mat2<f64>");
        using namespace math::utils;
        mat2<f64> m;

        mat2<f64> minv = m.get_inverse();

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                assert(f64_eq(m(i, j), minv(i, j)));
            }
        }


        auto det = m.det();
        assert(f64_eq(det, 1.));

        m = m * 2.;

        assert(f64_eq(m(0, 0), 2.));
        assert(f64_eq(m(1, 1), 2.));

        det = m.det();
        assert(f64_eq(det, 4.));

        m(0, 0) = 2.;
        m(1, 1) = 3.;
        m(0, 1) = 1.;
        m(1, 0) = 1.;

        minv = m.get_inverse();

        assert(f64_eq(minv(0, 0), 0.6));
        assert(f64_eq(minv(0, 1), -0.2));
        assert(f64_eq(minv(1, 0), -0.2));
        assert(f64_eq(minv(1, 1), 0.4));

        vector2d<f64> v(2, 3);

        v = minv * v;

        assert(f64_eq(v.x, 0.6));
        assert(f64_eq(v.y, 0.8));

        END_TEST("mat2<f64>");
    }
    void mat3_tests()
    {
        BEGIN_TEST("mat3<f64>");
        using namespace math::utils;
        mat3<f64> m;
        mat3<f64> minv = m.get_inverse();


        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                assert(f64_eq(m(i, j), minv(i, j)));
            }
        }


        auto det = m.det();
        assert(f64_eq(det, 1.));

        m = m * 2.;

        assert(f64_eq(m(0, 0), 2.));
        assert(f64_eq(m(1, 1), 2.));
        assert(f64_eq(m(2, 2), 2.));

        det = m.det();
        assert(f64_eq(det, 8.));

        m = mat3<f64>(vector3d<f64>(2, 1, 11), vector3d<f64>(1, 3, 13), vector3d<f64>(4, 7, 15));

        assert(f64_eq(m.det(), -110));
        minv = m.get_inverse();

        assert(f64_eq(minv(0, 0), 23./55.));
        assert(f64_eq(minv(0, 1), -31./55.));
        assert(f64_eq(minv(0, 2), 2./11.));

        assert(f64_eq(minv(1, 0), -37./110));
        assert(f64_eq(minv(1, 1), 7./55.));
        assert(f64_eq(minv(1, 2), 3./22.));

        assert(f64_eq(minv(2, 0), 1./22.));
        assert(f64_eq(minv(2, 1), 1./11.));
        assert(f64_eq(minv(2, 2), -1./22.));

        vector3d<f64> v(2, 3, 5);

        mat3<f64> seq(vector3d<f64>(1, 2, 3), vector3d<f64>(4, 5, 6), vector3d<f64>(7, 8, 9));

        v = seq * v;

        assert(f64_eq(v.x, 23.));
        assert(f64_eq(v.y, 53.));
        assert(f64_eq(v.z, 83.));

        END_TEST("mat3<f64>");
    }

    void mat4_tests()
    {
        BEGIN_TEST("mat4<f64>");
        using namespace math::utils;
        mat4<f64> m;
        mat4<f64> minv = m.get_inverse();


        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                assert(f64_eq(m(i, j), minv(i, j)));
            }
        }


        auto det = m.det();
        assert(f64_eq(det, 1.));

        m = m * 2.;

        assert(f64_eq(m(0, 0), 2.));
        assert(f64_eq(m(1, 1), 2.));
        assert(f64_eq(m(2, 2), 2.));
        assert(f64_eq(m(3, 3), 2.));

        det = m.det();
        assert(f64_eq(det, 16.));

        m = mat4<f64>(
                vector4d<f64>(2, 1, 11, 4),
                vector4d<f64>(1, 3, 13, 2),
                vector4d<f64>(4, 7, 15, 1),
                vector4d<f64>(1, 2, 3, 4));

        assert(f64_eq(m.det(), -465));
        minv = m.get_inverse();

        assert(f64_eq(minv(0, 0), 37./93.));
        assert(f64_eq(minv(0, 1), -53./93.));
        assert(f64_eq(minv(0, 2), 22./93.));
        assert(f64_eq(minv(0, 3), -16./93.));

        assert(f64_eq(minv(1, 0), -48./155.));
        assert(f64_eq(minv(1, 1), 21./155.));
        assert(f64_eq(minv(1, 2), 2./31.));
        assert(f64_eq(minv(1, 3), 7./31.));

        assert(f64_eq(minv(2, 0), 17./465.));
        assert(f64_eq(minv(2, 1), 41./465.));
        assert(f64_eq(minv(2, 2), -2./93.));
        assert(f64_eq(minv(2, 3), -7./93.));

        assert(f64_eq(minv(3, 0), 13./465.));
        assert(f64_eq(minv(3, 1), 4./465.));
        assert(f64_eq(minv(3, 2), -7./93.));
        assert(f64_eq(minv(3, 3), 22./93.));

        vector4d<f64> v(2, 3, 5, -7);

        mat4<f64> seq(vector4d<f64>(1, 2, 3, 4), 
                vector4d<f64>(5, 6, 7, 8),
                vector4d<f64>(9, 10, 11, 12),
                vector4d<f64>(13, 14, 15, 16)
                );

        v = seq * v;

        assert(f64_eq(v.x, -5.));
        assert(f64_eq(v.y, 7.));
        assert(f64_eq(v.z, 19.));
        assert(f64_eq(v.w, 31.));

        END_TEST("mat4<f64>");
    }

    void transofmration_tests()
    {
        BEGIN_TEST("transformations");
        using namespace utils;
        using namespace transformations;
        vector4d<f64> origin(0, 0, 0, 1);
        auto trans = translate_matrix(vector3d<f64>(1, 0, 0));

        origin = trans * origin;
        assert(f64_eq(origin.x, 1.));
        assert(f64_eq(origin.y, 0.));
        assert(f64_eq(origin.z, 0.));

        trans = translate_matrix(vector3d<f64>(-1, 2, 3));
        origin = trans * origin;

        assert(f64_eq(origin.x, 0.));
        assert(f64_eq(origin.y, 2.));
        assert(f64_eq(origin.z, 3.));

        origin.x = 1;
        origin.y = 0;
        origin.z = 0;

        trans = rot_matrix(vector3d<f64>(0, 0, 1), M_PI/2.);
        origin = trans * origin;

        assert(f64_eq(origin.x, 0.));
        assert(f64_eq(origin.y, 1.));
        assert(f64_eq(origin.z, 0.));

        origin = vector4d<f64>(1, 2, 3, 1);
        trans = scale_matrix(vector3d<f64>(-1, 2, 3));
        origin = trans * origin;

        assert(f64_eq(origin.x, -1));
        assert(f64_eq(origin.y, 4.));
        assert(f64_eq(origin.z, 9.));

        END_TEST("transformations");
    }
    void poly()
    {
        std::vector<std::string> polys_str = 
        {
            "2x + 4x^5 + 2 x ^ 2 + 3",
            "2x",
            "2",
            "23  x  + 4 x ^  5 + 3",
        };
        {
            BEGIN_TEST("polynomial string");
            std::vector<std::string> expected = 
            {
                "3x^0 + 2x^1 + 2x^2 + 4x^5",
                "2x^1",
                "2x^0",
                "3x^0 + 23x^1 + 4x^5"
            };

            for (int i = 0; i < polys_str.size(); i++)
            {
                assert(expected[i] == polynomial::from_str(polys_str[i]).to_str());
            }
            END_TEST("polynomial string");
        }
        {
            BEGIN_TEST("polynomial ops");
            std::vector<std::string> polys_str2 = 
            {
                "2x",
                "3x^5",
                "2",
                "3",
                "2x + 4x^5 + 2x^2 + 3",
            };
            std::vector<std::string> 
                expected = 
                {
                    "3x^0 + 2x^2 + 4x^5",
                    "3x^0 + 2x^1 + 2x^2 + 1x^5",
                    "1x^0 + 2x^1 + 2x^2 + 4x^5",
                    "2x^1 + 2x^2 + 4x^5",
                    "",
                };
            for (int i = 0; i < polys_str2.size(); i++)
            {
                assert(
                        (polynomial::from_str(polys_str[0]) 
                         - polynomial::from_str(polys_str2[i])).to_str() == expected[i]
                      );
            }
            expected = 
            {
                "3x^0 + 4x^1 + 2x^2 + 4x^5",
                "3x^0 + 2x^1 + 2x^2 + 7x^5",
                "5x^0 + 2x^1 + 2x^2 + 4x^5",
                "6x^0 + 2x^1 + 2x^2 + 4x^5",
                "6x^0 + 4x^1 + 4x^2 + 8x^5",
            };
            for (int i = 0; i < polys_str2.size(); i++)
            {
                assert(
                        (polynomial::from_str(polys_str[0]) 
                         + polynomial::from_str(polys_str2[i])).to_str() == expected[i]
                      );
            }

            expected = 
            {
                "6x^1 + 4x^2 + 4x^3 + 8x^6",
                "9x^5 + 6x^6 + 6x^7 + 12x^10",
                "6x^0 + 4x^1 + 4x^2 + 8x^5",
                "9x^0 + 6x^1 + 6x^2 + 12x^5",
            };
            for (int i = 0; i < polys_str2.size() - 1; i++)
            {
                assert(
                        (polynomial::from_str(polys_str[0]) 
                         * polynomial::from_str(polys_str2[i])).to_str() == expected[i]
                      );
            }
            polys_str2 = 
            {
                "x^2 -3x -10",
                "x^6 + 2x^4 + 6x -9",
            };

            std::vector<std::string> polys_str_div = 
            {
                "x + 2",
                "x^3 + 3",
            };
            expected = 
            {
                "-5x^0 + 1x^1",
                "-3x^0 + 2x^1 + 1x^3",
            };
            for (int i = 0; i < polys_str2.size(); i++)
            {
                /*
                assert(
                        (polynomial::from_str(polys_str2[i]) 
                         / polynomial::from_str(polys_str_div[i])).to_str() == expected[i]
                      );
                      */
            }
            END_TEST("polynomial ops");
        }
        {
            BEGIN_TEST("polynomial equations");
            std::vector<std::string> linear_polys = 
            {
                "2x^1 - 4x^0",
                "5x^1 + 15x^0",
                "-3x^1 + 9x^0",
            };
            std::vector<std::string> quad_polys = 
            {
                "2x^2 - 8x^1 + 6x^0",
                "3x^2 + 12x^1 + 15x^0",
                "x^2 - 9x^0",
            };
            std::vector<std::string> cubic_polys = 
            {
                "2x^3 - 6x^2 + 4x^1",
                "3x^3 + 3x^1 + 6x^0",
                "x^3 - 6x^2 + 11x^1 - 6x^0",
            };
            std::vector<std::string> quartic_polys = 
            {
                "2x^4 - 10x^2 + 8x^0",
                "3x^4 + 6x^2 + 12x^0",
                "x^4 - 5x^2 + 4x^0",
            };
            std::vector<std::string> quintic_polys = 
            {
                "2x^5 - 4x^3 + 2x^1",
                "3x^5 + 6x^3 + 9x^1",
                "x^5 - 5x^3 + 4x^1",
            };
            std::vector<std::vector<f64>> linear_sols = 
            {
                {2}, {-3}, {3}
            };
            std::vector<std::vector<f64>> quad_sols = 
            {
                {3, 1}, {}, {3, -3}
            };
            std::vector<std::vector<f64>> cubic_sols = 
            {
                {0, 1, 2}, {-1}, {1, 2, 3}
            };
            std::vector<std::vector<f64>> quartic_sols = 
            {
                {-2, -1, 1, 2}, {}, {-2, -1, 1, 2}
            };
            std::vector<std::vector<f64>> quintic_sols = 
            {
                {-1, 0, 1}, {0}, {-2, -1, 0, 1, 2}
            };

            std::vector<f64> sols;
            for (int i = 0; i < linear_polys.size(); i++)
            {
                sols = polynomial::from_str(linear_polys[i]).laguerre();
                assert(sols.size() == linear_sols[i].size());
                for (auto s : sols)
                {
                    bool found = false;
                    for (auto cs : linear_sols[i])
                    {
                        if (utils::f64_eq(cs, s))
                        {
                            found = true;
                            break;
                        }
                    }
                }
            }
            for (int i = 0; i < quad_polys.size(); i++)
            {
                sols = polynomial::from_str(quad_polys[i]).laguerre();
                assert(sols.size() == quad_sols[i].size());
                for (auto s : sols)
                {
                    bool found = false;
                    for (auto cs : quad_sols[i])
                    {
                        if (utils::f64_eq(cs, s))
                        {
                            found = true;
                            break;
                        }
                    }
                }
            }
            for (int i = 0; i < cubic_polys.size(); i++)
            {
                sols = polynomial::from_str(cubic_polys[i]).laguerre();
                assert(sols.size() == cubic_sols[i].size());
                for (auto s : sols)
                {
                    bool found = false;
                    for (auto cs : cubic_sols[i])
                    {
                        if (utils::f64_eq(cs, s))
                        {
                            found = true;
                            break;
                        }
                    }
                    assert(found);
                }
            }
            for (int i = 0; i < quartic_polys.size(); i++)
            {
                sols = polynomial::from_str(quartic_polys[i]).laguerre();
                assert(sols.size() == quartic_sols[i].size());
                for (auto s : sols)
                {
                    bool found = false;
                    for (auto cs : quartic_sols[i])
                    {
                        if (utils::f64_eq(cs, s))
                        {
                            found = true;
                            break;
                        }
                    }
                    assert(found);
                }
            }
            for (int i = 0; i < quintic_polys.size(); i++)
            {
                auto pol = polynomial::from_str(quintic_polys[i]);
                sols = pol.laguerre();
                assert(sols.size() == quintic_sols[i].size());
                for (auto s : sols)
                {
                    bool found = false;
                    for (auto cs : quintic_sols[i])
                    {
                        if (utils::f64_eq(cs, s, pol.root_tolerance(cs)))
                        {
                            found = true;
                            break;
                        }
                    }
                    assert(found);
                }
            }
            END_TEST("polynomial equations");
        }
    }
    void all()
    {
        v2d_tests();
        v3d_tests();
        mat2_tests();
        mat3_tests();
        mat4_tests();
        transofmration_tests();
        poly();
    }
}}}
