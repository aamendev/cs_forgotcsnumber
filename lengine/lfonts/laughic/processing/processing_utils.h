#ifndef PROCESSING_UTILS_H
#define PROCESSING_UTILS_H
#include "../types.h"
#include "../graphics_util.h"
typedef struct ColourBox
{
    ColourHistogramBar* cols;
    u32 col_count;
    u32 level;
    u32 minCol;
    u32 maxCol;

    struct ColourBox* next;
    struct ColourBox* prev;
} ColourBox;
void p_generic_inplace_min(u32* a, u32 b, i32* h, u32 s1);
void p_generic_inplace_max(u32* a, u32 b, i32* h, u32 s1);

void p_s_generic_inplace_max(i64* a, i64 b, i32* h, u32 s1);
void p_s_generic_inplace_min(i64* a, i64 b, i32* h, u32 s1);
#endif

