#ifndef LMATH_POLY_H
#define LMATH_POLY_H
#include "types.h"
#include "utils.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>
#include <complex>
#define LMATH_POLY_EPS (1e-8)
namespace lina { namespace math { 
    class polynomial
    {
        public:
            struct term 
            {
                f64 coefficient = 0.;
                f64 power = 0.;
            };
            inline static polynomial from_str(std::string str);
            inline static std::vector<f64> solve_depressed_cubic(f64 a, f64 b);
            inline static std::array<f64, 2> to_depressed(f64 a, f64 b, f64 c, f64 d);
            inline static std::vector<f64> solve_cubic(f64 a, f64 b, f64 c, f64 d);
            inline static std::vector<f64> solve_quadratic(f64 a, f64 b, f64 c);
            inline static std::vector<f64> solve_linear(f64 a, f64 b);
            inline f64 root_tolerance(f64 root);
            inline std::string to_str() const;
            inline std::complex<f64> evaluate(std::complex<f64> val) const;
            inline f64 evaluate(f64 val) const;
            inline polynomial derivative() const;
            inline void sort();
            inline void clean_zero();
            inline std::vector<f64> find_zeros();
            inline std::vector<f64> laguerre();
            inline friend polynomial operator/(const polynomial& p1, f64 scale);
            inline friend polynomial operator*(const polynomial& p1, f64 scale);
            inline friend polynomial operator*(const polynomial& p1, const term& t);
            inline friend polynomial operator*(const polynomial& p1, const polynomial& p2);
            inline friend polynomial operator/(const polynomial& p1, const polynomial& p2);
            inline friend term operator/(const term& t1, const term& t2);
            inline friend polynomial operator-(const polynomial& p1, const polynomial& p2);
            inline friend polynomial operator+(const polynomial& p1, const polynomial& p2);
            inline friend std::ostream& operator<<(std::ostream& os, const polynomial& p);
        public:
            inline bool clean_condition(const term& t) const;
        public:
            std::vector<term> m_terms;
            bool m_auto_clean_zero = true;
            std::vector<polynomial> m_derivative_cache;
    };
    inline std::vector<f64> polynomial::solve_linear(f64 a, f64 b)
    {
        return {-b/a};
    }
    inline std::vector<f64> polynomial::solve_quadratic(f64 a, f64 b, f64 c)
    {
        f64 disc = b*b - 4*a*c;
        if (utils::f64_eq(std::abs(disc), 0))
        {
            return {-b/(2*a)};
        }
        if (utils::f64_cmp(disc, 0) < 1)
        {
            return {};
        }
        return {(-b + std::sqrt(disc)) / (2*a), (-b - std::sqrt(disc))/(2*a)};
    }
    inline std::vector<f64> polynomial::solve_cubic(f64 a, f64 b, f64 c, f64 d)
    {
        auto arr = to_depressed(a, b, c, d);
        auto cub = solve_depressed_cubic(arr[0], arr[1]);
        for (auto& c : cub)
        {
            c -= (b/a)/3;
        }
        return cub;
    }
    inline std::array<f64, 2> polynomial::to_depressed(f64 a, f64 b, f64 c, f64 d)
    {
        f64 diff = (b/a)/3;
        std::array<f64, 2> coeffs = 
        {
            3 * diff * diff - (2 * diff ) * (b/a) + (c/a),
            -diff * diff * diff + diff * diff * (b/a) - diff * (c/a) + d/a
        };
        return coeffs;
    }
    inline std::vector<f64> polynomial::solve_depressed_cubic(f64 a, f64 b)
    {
        double disc = (b * b)/4.0 + (a * a * a)/27.0;
        std::vector<double> ret;
        double temp2 = -b/2.0;
        if (disc > LMATH_POLY_EPS)
        {
            double temp = std::sqrt(disc);
            ret= {
                std::cbrt(temp2 + temp) + std::cbrt(temp2-temp),
            };
        }
        else if (std::abs(disc) <= LMATH_POLY_EPS)
        {
            double u = std::cbrt(temp2);
            ret= {
                2 * u
            };
            if (std::abs(u) > LMATH_POLY_EPS)
            {
                ret.push_back(-u);
            }
        }
        else
        {
            double r = 2.0 * sqrt(std::cbrt(temp2 * temp2 - disc));
            double theta = atan2(sqrt(-disc), temp2);
            ret = {
                r * cos(theta/3),
                r * cos((theta + 2. * M_PI)/3),
                r * cos((theta + 4 * M_PI)/3),
            };
        }
        return ret;
    }
    inline polynomial::term operator/(const polynomial::term& t1, const polynomial::term& t2)
    {
        return {t1.coefficient/t2.coefficient, t1.power - t2.power};
    }
    inline polynomial operator*(const polynomial& p1, const polynomial::term& m)
    {
        polynomial result = p1;
        for (auto& t : result.m_terms)
        {
            t.coefficient *= m.coefficient;
            t.power += m.power;
        }
        return result;
    }
    inline polynomial operator*(const polynomial& p1, const polynomial& p2)
    {
        polynomial result;
        for (const auto& t : p2.m_terms)
        {
            result = result + p1 * t;
        }
        return result;
    }
    
    polynomial operator/(const polynomial& p1, const polynomial& p2)
    {
        if (p1.m_terms.empty() || p2.m_terms.empty())
        {
            return polynomial();
        }
        polynomial curr_poly = p1;
        polynomial result;
        while (!curr_poly.m_terms.empty() && curr_poly.m_terms.back().power >= p2.m_terms.back().power)
        {
            auto new_term = curr_poly.m_terms.back() / p2.m_terms.back();
            result.m_terms.push_back(new_term);
            curr_poly = curr_poly - p2 * new_term;
            curr_poly.clean_zero();
            curr_poly.sort();
        }
        result.clean_zero();
        result.sort();
        return result;
    }
    polynomial operator-(const polynomial& p1, const polynomial& p2)
    {
        polynomial result = p1;
        for (const auto& t : p2.m_terms)
        {
            for (auto& t2 : result.m_terms)
            {
                if (utils::f64_eq(t2.power, t.power))
                {
                    t2.coefficient -= t.coefficient;
                    goto main_loop;
                }
            }
            result.m_terms.push_back({-t.coefficient, t.power});
main_loop:
        }
        result.clean_zero();
        return result;
    }
    inline polynomial operator+(const polynomial& p1, const polynomial& p2)
    {
        polynomial result = p1;
        for (const auto& t : p2.m_terms)
        {
            for (auto& t2 : result.m_terms)
            {
                if (utils::f64_eq(t2.power, t.power))
                {
                    t2.coefficient += t.coefficient;
                    goto main_loop;
                }
            }
            result.m_terms.push_back({t.coefficient, t.power});
main_loop:
        }
        result.clean_zero();
        return result;
    }
    inline std::string polynomial::to_str() const
    {
        std::stringstream str_stream;
        u64 count = 0;
        for (const auto& t : m_terms)
        {
            str_stream << t.coefficient << "x^" << t.power;
            if (count < m_terms.size() - 1)
            {
                str_stream << " + ";
            }
            count++;
        }
        return str_stream.str();
    }
    inline std::ostream& operator<<(std::ostream& os, const polynomial& p)
    {
        os << p.to_str();
        return os;
    }
    polynomial polynomial::from_str(std::string str)
    {
        polynomial result;
        f64 coeff = 0.;
        auto it1 = str.begin();
        auto term_end_itr = str.end();
        auto power_iter = it1;
        auto it2 = std::find(it1, str.end(), 'x');
        int cnt = 0;
        while (it2 != str.end())
        {
            f64 val = 1.;
            it1 = std::find_if(it1, it2,
                    [](auto c){return c >= '0' && c<= '9' || c == '-';});
            if (it1 != it2)
            {
                f64 mul = 1.;
                if (*it1 == '-')
                {
                    it1++;
                    mul = -1;
                }
                val = mul * std::stod(std::string(it1, it2));
            }
            if (term_end_itr != str.end())
            {
                val *= -(*term_end_itr == '-') + (*term_end_itr != '-');
            }
            term_end_itr = std::find_if(it2, str.end(),
                    [](auto c){return c == '+' || c == '-';});
            power_iter = std::find(it2, term_end_itr, '^');
            f64 power = 1.;
            if (power_iter != term_end_itr)
            {
                power = std::stod(std::string(power_iter+1, term_end_itr));
            }
            result.m_terms.push_back({val, power});
            it1 = term_end_itr + (term_end_itr != str.end());
            it2 = std::find(it1, str.end(), 'x');
        }
        if (it1 != str.end())
        {
            f64 val = std::stod(std::string(it1, str.end()));
            if (term_end_itr != str.end())
            {
                val *= -(*term_end_itr == '-') + (*term_end_itr != '-');
            }
            result.m_terms.push_back(
                    {val, 
                    0.});
        }
        result.clean_zero();
        result.sort();
        return result;
    }
    inline f64 polynomial::root_tolerance(f64 root)
    {
        if (m_derivative_cache.empty())
        {
            m_derivative_cache.reserve(m_terms.back().power);
            m_derivative_cache.push_back(*this);
        }
        f64 tol = LMATH_POLY_EPS;
        f64 der = 0.;
        u64 curr_mul = 1;
        i64 count = 0;
        while (!m_derivative_cache[count].m_terms.empty() && utils::f64_eq(der, 0.))
        {
            count++;
            curr_mul *= count;
            if (m_derivative_cache.size() < count+1)
            {
                m_derivative_cache.push_back(m_derivative_cache.back().derivative());
            }
            der = std::abs(m_derivative_cache[count].evaluate(root));
        }
        if (!utils::f64_eq(der, 0.))
        {
            tol = std::pow(curr_mul * LMATH_POLY_EPS / der, 1./count);
        }
        return tol;
    }
    inline std::vector<f64> polynomial::laguerre()
    {
        std::vector<f64> zeros;
        polynomial curr_poly = *this;
        curr_poly.clean_zero();
        curr_poly.sort();
        if (curr_poly.m_terms.empty()) return {};
        const auto& last_term = curr_poly.m_terms.back();
        f64 curr_power = last_term.power;
        while (curr_power > 3)
        {
            const auto& last_term = curr_poly.m_terms.back();
            if (!utils::f64_eq(last_term.coefficient, 1.))
            {
                curr_poly = curr_poly / last_term.coefficient;
            }
            f64 invpower = 1./curr_power;
            polynomial df = curr_poly.derivative();
            polynomial ddf = df.derivative();
            std::complex<f64> z = 0;
            std::complex<f64> fz = curr_poly.evaluate(z);
            std::complex<f64> a;
            std::complex<f64> b;
            std::complex<f64> c;
            std::complex<f64> add_factor;
            auto max_queue = 5;
            auto q  = std::deque<std::complex<f64>>() ;

            while (std::abs(fz) > LMATH_POLY_EPS)
            {
                if (q.size() >= max_queue)
                {
                    q.pop_front();
                }
                q.push_back(z);

                a = -df.evaluate(z) / fz;
                b = a * a - ddf.evaluate(z) / fz;
                add_factor = std::sqrt((curr_power - 1) * (curr_power * b - a*a));
                c = invpower * (a + add_factor); 
                c = std::max(c, invpower * (a - add_factor), 
                        utils::abs_cmp<std::complex<f64>>);
                if (utils::f64_eq(std::abs(c), 0.))
                {
                    z += LMATH_POLY_EPS * 2;
                    fz = curr_poly.evaluate(z);
                    continue;
                }
                z += 1./c;
                fz = curr_poly.evaluate(z);
                auto prev = q.back();
                if (std::abs(z - prev) > 0)
                {
                    int i = 0;
                    for (auto& qe : q)
                    {
                        if (i == max_queue - 1)
                        {
                            break;
                        }
                            i++;
                        auto val = std::abs(z - qe) / std::abs(z - prev);
                        if (val <= 0.01)
                        {
                            z += std::complex<f64>{1., 1.};
                            break;
                        }
                    }
                }
            }
            polynomial div;
            if (utils::f64_eq(std::abs(z.imag()), 0))
            {
                zeros.push_back(z.real());
                div.m_terms = {{-z.real(), 0}, {1, 1}};
                curr_poly = curr_poly / div;
                curr_poly.clean_zero();
                curr_power = curr_poly.m_terms.back().power;
            }
            else 
            {
                div.m_terms = {
                    {z.real() * z.real() + z.imag() * z.imag(), 0.},
                    {-2 * z.real(), 1}, 
                    {1, 2},
                };
                curr_poly = curr_poly / div;
                curr_poly.clean_zero();
                curr_power = curr_poly.m_terms.back().power;
            }
        }
        if (!curr_poly.m_terms.empty())
        {
            curr_power = curr_poly.m_terms.back().power;
            f64 a = 0.;
            f64 b = 0.;
            std::vector<f64> new_zeros;
            if (utils::f64_eq(curr_power, 3.))
            {
                f64 c = 0.;
                f64 d = 0.;
                for (const auto& t : curr_poly.m_terms)
                {
                    a += t.coefficient * (utils::f64_eq(t.power, 3.));
                    b += t.coefficient * (utils::f64_eq(t.power, 2.));
                    c += t.coefficient * (utils::f64_eq(t.power, 1.));
                    d += t.coefficient * (utils::f64_eq(t.power, 0.));
                }
                new_zeros = solve_cubic(a, b, c, d);
                zeros.insert(zeros.end(), new_zeros.begin(), new_zeros.end());
            }
            else if (utils::f64_eq(curr_power, 2.))
            {
                f64 c = 0.;
                for (const auto& t : curr_poly.m_terms)
                {
                    a += t.coefficient * (utils::f64_eq(t.power, 2.));
                    b += t.coefficient * (utils::f64_eq(t.power, 1.));
                    c += t.coefficient * (utils::f64_eq(t.power, 0.));
                }
                new_zeros = solve_quadratic(a, b, c);
                zeros.insert(zeros.end(), new_zeros.begin(), new_zeros.end());
            }
            else if (utils::f64_eq(curr_power, 1.))
            {
                for (const auto& t : curr_poly.m_terms)
                {
                    a += t.coefficient * (utils::f64_eq(t.power, 1.));
                    b += t.coefficient * (utils::f64_eq(t.power, 0.));
                }
                new_zeros = solve_linear(a, b);
                zeros.insert(zeros.end(), new_zeros.begin(), new_zeros.end());
            }
        }
        std::sort(zeros.begin(), zeros.end());

        auto it = std::unique(zeros.begin(), zeros.end(), [&](auto v1, auto v2)
                {
                    f64 tol = root_tolerance(v1);
                    return utils::f64_eq(v1, v2, tol);
                }
                );
        zeros.erase(it, zeros.end());
        return zeros;
    }
    inline std::complex<f64> polynomial::evaluate(std::complex<f64> val) const
    {
        std::complex<f64> ret = 0;
        for (const auto& t : m_terms)
        {
            if (utils::f64_eq(std::abs(t.power), 0.))
            {
                ret += t.coefficient;
                continue;
            }
            ret += t.coefficient * std::pow(val, t.power);
        }
        return ret;
    }
    inline f64 polynomial::evaluate(f64 val) const
    {
        f64 ret = 0;
        for (const auto& t : m_terms)
        {
            ret += t.coefficient * std::pow(val, t.power);
        }
        return ret;
    }
    inline polynomial polynomial::derivative() const
    {
        polynomial p;
        p.m_terms.reserve(m_terms.size());
        for (const auto& t : m_terms)
        {
            if (std::abs(t.power) > LMATH_POLY_EPS && !clean_condition(t))
            {
                p.m_terms.emplace_back(t.coefficient * t.power, t.power - 1);
            }
        }
        return p;
    }
    inline void polynomial::sort()
    {
        std::sort(m_terms.begin(), m_terms.end(), [&](
            const auto& t1,
            const auto& t2
                    ){
                        return t1.power < t2.power && !clean_condition(t1);
                });
    }
    inline void polynomial::clean_zero()
    {
        std::erase_if(m_terms, [](auto& t){return std::abs(t.coefficient) < LMATH_POLY_EPS;});
    }
    inline bool polynomial::clean_condition(const term& t) const
    {
        return m_auto_clean_zero && std::abs(t.coefficient) < LMATH_POLY_EPS;
    }

    inline polynomial operator/(const polynomial& p1, f64 scale)
    {
        polynomial result = p1;
        for (auto& t : result.m_terms)
        {
            t.coefficient /= scale;
        }
        return result;
    }
    inline polynomial operator*(const polynomial& p1, f64 scale)
    {
        polynomial result = p1;
        for (auto& t : result.m_terms)
        {
            t.coefficient *= scale;
        }
        return result;
    }
}}
#endif
