#ifndef LOGICUTIL
#define LOGICUTIL
#include <stdlib.h>
#include <math.h>
#include "types.h"
void swap(int* a, int* b);
void sortX(int* x0, int* y0, int* x1, int* y1, int* x2, int* y2);
void sort(int* data, int stride, int count, int sortElement);
int area(int x0, int y0, int x1, int y1, int x2, int y2);
void bary(int x0, int y0, int x1, int y1, int x2, int y2, int x, int y, float* bary);
float rand_float();
int rand_int();
int rand_int_bound(i32 min, i32 max);

// Processing
void inplace_min(u32* a, u32 b);
void inplace_max(u32* a, u32 b);

// Common comp
i32 u32_comp(void* v1, void* v2);
i32 i32_comp(void* v1, void* v2);

#ifndef PI
#define PI (3.1415926535)
#endif

#define FLT_MIN (0.0005f)
#endif
