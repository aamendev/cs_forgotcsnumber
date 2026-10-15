#ifndef L_FONT_UTILS_H
#define L_FONT_UTILS_H
#include "types.h"
#include <functional>
#include <iostream>
#include <optional>
#include <string_view>
namespace lina { namespace font { namespace utils {
    template <typename T>
        inline u64 convert_bytes(
                std::string_view str, 
                u64 offset, 
                T* to_store, 
                u64 to_store_count = 1)
        {
            for (int j = 0; j < to_store_count; j++)
            {
                to_store[j] = 0;
                for (int i = 0; i < sizeof(T); i++)
                {
                    to_store[j] |= (((T)(u8)str[i + offset + j * sizeof(T)]) << ((sizeof(T)-i-1)*8));
                }
            }
            return offset + sizeof(T) * to_store_count;
        }

    template <typename T, int ArrSize>
        inline T convert_big_endian(const std::array<u8, ArrSize>& arr)
        {
            T val = 0;
            constexpr u32 max_count = 
                ArrSize * (ArrSize < sizeof(T)) 
                + sizeof(T) * !(ArrSize < sizeof(T));
            for (int i = 0; i < max_count; i++)
            {
                val |= (((T)arr[i]) << ((max_count - i - 1)*8));
            }
            return val;
        }
    template <typename In, typename T, typename G>
        inline std::optional<const std::reference_wrapper<const In>> possibly_unsafe_get(
            const T& container1, const G& container2, u64 idx = 0
                )
        {
        if ((idx >= container1.size() + container2.size())) 
        {
            return std::nullopt;
        }
        if (idx >= container1.size())
        {
            return std::cref(container2[idx - container1.size()]);
        }
        return std::cref(container1[idx]);
        }

}}}
#endif
