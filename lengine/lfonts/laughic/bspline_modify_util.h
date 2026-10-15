#ifndef BSPLINE_MODIFY_UTIL_H
#define BSPLINE_MODIFY_UTIL_H
#include "graphics_util.h"
#include "shapes.h"

typedef struct RadiusOpt
{
    u8 min_r;
    u8 max_r;
} RadiusOpt;

typedef struct RadiusColOpt
{
    RadiusOpt rad_opt;
    Channel chan;
} RadiusColOpt;

typedef struct SimpleWiggleOpt
{
    u8 prob;
    int min_wiggle_x;
    int max_wiggle_x;
    int min_wiggle_y;
    int max_wiggle_y;
} SimpleWiggleOpt;

typedef struct SimpleRangeWiggleOpt
{
    u8 prob;
    int min_wiggle_x;
    int max_wiggle_x;
    int min_wiggle_y;
    int max_wiggle_y;
    float min_u;
    float max_u;
} SimpleRangeWiggleOpt;

void bspline_width_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts);
void bspline_mid_width_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts);
void bspline_channel_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts);

void bspline_channel_width_modify(Canvas* c, BSpline* bsp, float u, Line* l, 
        SimpleBrush* b, void* opts);

void bspline_simple_wiggle_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts);
void bspline_simple_range_wiggle_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts);
#endif
