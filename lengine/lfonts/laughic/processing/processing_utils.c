#include "processing_utils.h"
#include <limits.h>

void p_generic_inplace_min(u32* a, u32 b, i32* h, u32 kernel_idx)
{
    *a = *a * (*a < b) + b * !(*a < b);
}
void p_generic_inplace_max(u32* a, u32 b, i32* h, u32 kernel_idx)
{
    *a = *a * (*a > b) + b * !(*a > b);
}

void p_s_generic_inplace_min(i64* a, i64 b, i32* h, u32 kernel_idx)
{
    if (h[kernel_idx] == INT_MAX)
    {
        return;
    }
    *a = *a * (*a < b) + b * !(*a < b);
}
void p_s_generic_inplace_max(i64* a, i64 b, i32* h, u32 kernel_idx)
{
    if (h[kernel_idx] == -INT_MAX)
    {
        return;
    }
    *a = *a * (*a > b) + b * !(*a > b);
}
