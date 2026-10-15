#include "./logic_util.h"
#include "./graphics_util.h"
#include "./math/vector3d.h"
#ifndef SHAPESH
#define SHAPESH

void point(Canvas* canvas, int x, int y, u32 colour);
void line_old(Canvas* canvas, int x0, int y0, int x1, int y1, u32 colour);
void line(Canvas* canvas, int x0, int y0, int x1, int y1, u32 colour);
void new_line(Canvas* c, Line* l, SimpleBrush* b);

void fill(Canvas* canvas, u32 colour);

void rect(Canvas* canvas, int x, int y, int w, int h, u32 colour);
void fill_rect(Canvas* canvas, int x, int y, int w, int h, u32 colour);
void fill_rect_texture(Canvas* canvas, int x, int y, int w, int h, Texture* tex);

void circle(Canvas* canvas, int x, int y, int r, u32 colour);
void fill_circle(Canvas* canvas, int x, int y, int r, u32 colour);
void fill_circle_texture(Canvas* canvas, int x, int y, int r, Texture* tex);

void ellipse(Canvas* canvas, int x, int y, int r0, int r1, u32 colour);
void fill_ellipse(Canvas* canvas, int x, int y, int r0, int r1, u32 colour);
void fill_ellipse_texture(Canvas* canvas, int x, int y, int r0, int r1, Texture* tex);

void triangle(Canvas* canvas, int x0, int y0, int x1, int y1, int x2, int y2, u32 colour);
void fill_triangle(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, u32 color);
void fill_triangle_texture(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, Texture* tex, int u0, int v0, int u1, int v1, int u2, int v2);
void fill_triangle_bary(Canvas* canvas, int x1, int y1, int x2, int y2, int x3, int y3, u32 c1, u32 c2, u32 c3);

void old_parametric_cubic(Canvas* canvas, 
        int x0, int y0,
        float a0, float b0, float c0, float d0,
        float a1, float b1, float c1, float d1,
        int seg_num,
        u32 colour
        );

void parametric_cubic(Canvas* canvas, ParametricCubic2D* pc2, u32 colour);
void bspline(Canvas* canvas, BSpline* bsp, SimpleBrush* b);
void bspline_modify(Canvas* canvas, BSpline* bsp, SimpleBrush* b, void* opts,
        void(*modify)(Canvas* c, BSpline* bsp, float u, Line* l, SimpleBrush* b, void* opts));
void bspline_lod_test(Canvas* canvas, BSpline* bsp, SimpleBrush* sb, u32 stroke_count);
void bspline_circ_test(Canvas* canvas, BSpline* bsp, SimpleBrush* sb, u32 stroke_count);


void bounded_circle(Canvas* canvas, Circle* c, ParametricLine* l, u32 colour);
void bspline_lina_test(Canvas* canvas, BSpline* bsp, SimpleBrush* sb, u32 stroke_count);
//void fill(Canvas* canvas, Shape type, void* shape);

#endif

