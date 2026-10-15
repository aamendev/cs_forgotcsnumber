#ifndef SPLINE_SAMPLES_H
#define SPLINE_SAMPLES_H
#include "bspline_modify_util.h"
#include "shapes.h"
void bspline_face_outline(Canvas * c, SimpleBrush* sb, int x0, int y0);

void bspline_l_eye(Canvas* c, SimpleBrush* sb, int x0, int y0);
void bspline_r_eye(Canvas * c, SimpleBrush* sb, int x0, int y0);
void bspline_smile(Canvas* c, SimpleBrush* sb, int x0, int y0);
void bspline_nose(Canvas* c, SimpleBrush* sb, int x0, int y0);

void bspline_wiggle_face_outline(
        Canvas* canvas, SimpleBrush* b, int x0, int y0, void* opts,
        void(*modify)
        (Canvas* c, 
         BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts));

void bspline_lod_face_outline(Canvas* canvas, SimpleBrush* b, int x0, int y0,
        u32 seg_count
        );
void bspline_width_l_eye(
        Canvas* canvas, SimpleBrush* b, int x0, int y0, void* opts,
        void(*modify)
        (Canvas* c, 
         BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts));

void bspline_width_r_eye(
        Canvas* canvas, SimpleBrush* b, int x0, int y0, void* opts,
        void(*modify)
        (Canvas* c, 
         BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts));
    
void bspline_width_nose(
        Canvas* canvas, SimpleBrush* b, int x0, int y0, void* opts,
        void(*modify)
        (Canvas* c, 
         BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts));

void bspline_width_smile(
        Canvas* canvas, SimpleBrush* b, int x0, int y0, void* opts,
        void(*modify)
        (Canvas* c, 
         BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts));

void bspline_lina(Canvas* canvas, SimpleBrush* b, int x0, int y0);
#endif
