#ifndef L_FONT_CFF_UTILS_H
#define L_FONT_CFF_UTILS_H
#include <cmath>
#include <variant>
#include <vector>
#include <array>
#include <math.h>
namespace lina { namespace font { namespace tables{
    
    template <typename T, typename G>
        T get_variant(const G& var)
        {
            return std::visit([](auto v)
                    {return static_cast<T>(v);}, var);
        }
}}}
#endif
