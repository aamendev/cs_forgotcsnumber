#include "bspline_modify_util.h"
void bspline_width_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts)
{
    RadiusOpt* ro = (RadiusOpt*) opts;
    float ratio = 
        (u - bsp->knots[bsp->order - 1]) /
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1]);
    b->r = ratio * (ro->max_r - ro->min_r) + ro->min_r;
}

void bspline_mid_width_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts)
{
    RadiusOpt* ro = (RadiusOpt*) opts;
    float ratio = 0;
    if (u <= 0.5f)
    {
    ratio = 
        (2 * u - bsp->knots[bsp->order - 1]) /
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1]);
    b->r = ro->max_r - ratio * (ro->max_r - ro->min_r);
    }
    else 
    {
    ratio = 
        (2 * (u - 0.5f) - bsp->knots[bsp->order - 1]) /
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1]);
    b->r = ro->min_r + ratio * (ro->max_r - ro->min_r);
    }
}

void bspline_channel_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts)
{
    float ratio = 
        (u - bsp->knots[bsp->order - 1]) /
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1]);
    
    u8 current_ch = ratio * 0xff;
    set_channel(&b->colour, current_ch, * (Channel*)opts);
}

void bspline_channel_width_modify(Canvas* c, BSpline* bsp, 
        float u, Line* l, SimpleBrush* b, void* opts)
{
    RadiusColOpt* rco = (RadiusColOpt*) opts;
    bspline_width_modify(c, bsp, u, l, b, &rco->rad_opt);
    bspline_channel_modify(c, bsp, u, l, b, &rco->chan);
}

void bspline_simple_wiggle_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts)
{
    SimpleWiggleOpt* wo = (SimpleWiggleOpt*) opts;
    if (rand_int_bound(0, 100) < wo->prob)
    {
        l->x1 += rand_int_bound(wo->min_wiggle_x, wo->max_wiggle_x);
        l->y1 += rand_int_bound(wo->min_wiggle_y, wo->max_wiggle_y);
    }
}

void bspline_simple_range_wiggle_modify(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts)
{
    SimpleRangeWiggleOpt* wo = (SimpleRangeWiggleOpt*) opts;
    if (rand_int_bound(0, 100) < wo->prob && 
            u >= wo->min_u && u <= wo->max_u)
    {
        l->x1 += rand_int_bound(wo->min_wiggle_x, wo->max_wiggle_x);
        l->y1 += rand_int_bound(wo->min_wiggle_y, wo->max_wiggle_y);
    }
}
