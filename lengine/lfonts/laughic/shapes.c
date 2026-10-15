#include "./shapes.h"
#include "bspline_modify_util.h"
#include "logic_util.h"
#include "math/vector3d.h"
#include "raytracer/brdf/brdf.h"
#include "spline_samples.h"
#include <stdio.h>

static void steep_line(Canvas* canvas, int x0, int y0, int x1, int y1, u32 colour)
{
    if (y0 > y1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    /*if (y1 > canvas->height - 1 || y0 < 0 
            || x0 < 0 || x1 < 0 
            || x0 > canvas->width - 1 
            || x1 > canvas->width - 1)
        return;
        */
    int dx = (x1 - x0) * (x0 < x1) + (x0 - x1) * !(x0 < x1);
    int dy = y1 - y0;
    int d = 2 * dx - dy;
    int j = x0;
    for (int i = y0; i < y1 + 1; i++)
    {
        mix_colour(&canvas->pixels[(i % canvas->height) * canvas->width 
                + (j % canvas->width)] , colour);
        j += (d > 0 && x0 < x1) - (d > 0 && !(x0 < x1));
        d += 2 * dx - 2 * dy * (d > 0);
    }
}
static void new_steep_line(Canvas* canvas, Line* l, SimpleBrush* b)
{

    int x0 = l->x0;
    int x1 = l->x1;
    int y0 = l->y0;
    int y1 = l->y1;
    if (y0 > y1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    /*if (y1 > canvas->height - 1 || y0 < 0 
            || x0 < 0 || x1 < 0 
            || x0 > canvas->width - 1 
            || x1 > canvas->width - 1)
        return;
        */
    int dx = (x1 - x0) * (x0 < x1) + (x0 - x1) * !(x0 < x1);
    int dy = y1 - y0;
    int d = 2 * dx - dy;
    int j = x0;
    for (int i = y0; i < y1 + 1; i++)
    {
        fill_circle(canvas, (j % canvas->width),
                i % canvas->height, b->r, b->colour);
        j += (d > 0 && x0 < x1) - (d > 0 && !(x0 < x1));
        d += 2 * dx - 2 * dy * (d > 0);
    }
}

static void shallow_line(Canvas* canvas, int x0, int y0, int x1, int y1, u32 colour)
{
    if (x0 > x1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
   
    int dy = (y1 - y0) * (y0 < y1) + (y0 - y1) * !(y0 < y1);
    int dx = x1 - x0;
    int d = 2 * dy - dx;
    int j = y0;
    for (int i = x0; i < x1 + 1; i++)
    {
        mix_colour(&canvas->pixels[(j % canvas->height) * canvas->width +
                (i % canvas->width)], colour);
        j += (d > 0 && y0 < y1) - (d > 0 && !(y0 < y1));
        d += 2 * dy - 2 * dx * (d > 0);
    }
}
static void new_shallow_line(Canvas* canvas, Line* l, SimpleBrush* b)
{

    int x0 = l->x0;
    int x1 = l->x1;
    int y0 = l->y0;
    int y1 = l->y1;
    if (x0 > x1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
   
    int dy = (y1 - y0) * (y0 < y1) + (y0 - y1) * !(y0 < y1);
    int dx = x1 - x0;
    int d = 2 * dy - dx;
    int j = y0;
    for (int i = x0; i < x1 + 1; i++)
    {
        fill_circle(canvas, (i % canvas->width),
                j % canvas->height, b->r, b->colour);
        j += (d > 0 && y0 < y1) - (d > 0 && !(y0 < y1));
        d += 2 * dy - 2 * dx * (d > 0);
    }
}

void point(Canvas* canvas, int x, int y, u32 colour)
{
    mix_colour(&canvas->pixels[y * canvas->width + x], colour);
}

void line(Canvas* canvas, int x0, int y0, int x1, int y1, u32 colour)
{
    int dy = (y1 - y0) * (y0 < y1) + (y0 - y1) * !(y0 < y1);
    int dx = (x1 - x0) * (x0 < x1) + (x0 - x1) * !(x0 < x1);

    if (dy < dx)
    {
        shallow_line(canvas,x0,y0,x1,y1,colour);
    }
    else
    {
        steep_line(canvas,x0,y0,x1,y1,colour);
    }
}

void new_line(Canvas *c, Line *l, SimpleBrush *b)
{
    int dy = (l->y1 - l->y0) * (l->y0 < l->y1) + 
        (l->y0 - l->y1) * !(l->y0 < l->y1);
    int dx = (l->x1 - l->x0) * (l->x0 < l->x1) + 
        (l->x0 - l->x1) * !(l->x0 < l->x1);

    if (dy < dx)
    {
        new_shallow_line(c, l, b);
        //shallow_line(c,l->x0,l->y0,
         //       l->x1,l->y1,b->colour);
    }

    else
    {
        //steep_line(c,l->x0,l->y0,
         //       l->x1,l->y1,b->colour);
        new_steep_line(c, l, b);
    }
    (void)new_shallow_line;
    (void)new_steep_line;
}

void line_old(Canvas* canvas, int x0, int y0, int x1, int y1, u32 colour)
{
    int dx = (x0 - x1) * (x0 > x1) + (x1 - x0) * !(x0 > x1);
    int dy = (y0 - y1) * (y0 > y1) + (y1 - y0) * !(y0 > y1);
    int sx = (x0 < x1) - !(x0 < x1);
    int sy = (y0 < y1) - !(y0 < y1);
    int e = -2*dy + 2*dx;

    int i = x0;
    int j = y0;

    while(i != x1 || j != y1)
    {
        mix_colour(&canvas->pixels[j * canvas->width + i] , colour);
        if (e > dy - 1)
        {
            if (i == x1) break;
            i += sx;
            e -= 2 * dy;
        }

        if (e < dx + 1)
        {
            if (j == y1) break;
            j += sy;
            e += 2 * dx;
        }
    }
}
void fill(Canvas* canvas, u32 colour)
{
    for (size_t i = 0; i < canvas->width * canvas->height; i++)
    {
        canvas->pixels[i] = colour;
    }
}

void rect(Canvas* canvas, int x, int y, int w, int h, u32 colour)
{
    line(canvas, x, y, x+w, y, colour);
    line(canvas, x, y, x, y+h, colour);
    line(canvas, x, y+h, x + w, y+h, colour);
    line(canvas, x+w, y, x+w, y+h, colour);
}

void fill_rect(Canvas* canvas, int x, int y, int w, int h, u32 colour)
{
    for (int j = y; j < y + h; j++)
    {
        for (int i = x; i < x + w; i++)
        {
            mix_colour(&canvas->pixels[j * canvas->width + i], colour);
        }
    }
}

void fill_rect_texture(Canvas* canvas, int x, int y, int w, int h, Texture* tex)
{
    u32 hRatio = tex->height / h * tex->width;
    u32 wRatio = tex->width / w;

    for (int j = y; j < y + h; j++)
    {
        for (int i = x; i < x + w; i++)
        {
            mix_colour(
                    &canvas->pixels[j * canvas->width + i],
                    tex->data[(j-y) * hRatio + (i - x) * wRatio]
                    );
        }
    }
}

void circle(Canvas* canvas, int x, int y, int r, u32 colour)
{
    int j = 0;
    int i = r;

    while (i > -j)
    {
        mix_colour(&canvas->pixels[(j + y) * canvas->width + i + x] , colour);
        mix_colour(&canvas->pixels[(j + y) * canvas->width - i + x] , colour);
        mix_colour(&canvas->pixels[(y - j) * canvas->width + i + x] , colour);
        mix_colour(&canvas->pixels[(y - j) * canvas->width - i + x] , colour);

        mix_colour(&canvas->pixels[(i + y) * canvas->width + j + x] , colour);
        mix_colour(&canvas->pixels[(i + y) * canvas->width - j + x] , colour);
        mix_colour(&canvas->pixels[(y - i) * canvas->width + j + x] , colour);
        mix_colour(&canvas->pixels[(y - i) * canvas->width - j + x] , colour);

        int i1 = i;
        int jj = j - 1;
        int i2 = i - 1;

        int v1 = i1 * i1 + jj * jj - r * r;
        int v2 = -(i2 * i2 + jj * jj - r * r);

        i = i1 * (v1 <= v2) + i2 * !(v1 <= v2);

        j--;
    }
}

void fill_circle_texture(Canvas* canvas, int x, int y, int r, Texture* tex)
{
    int j = 0;
    int i = r;
    int oldi = i;
    u32 hRatio = tex->height / (2 * r) * tex->width;
    u32 wRatio = tex->width / (2 * r);

    while (i > -j)
    {
        for (int n = -i; n < i; n++)
        {
            mix_colour(
                    &canvas->pixels[(j + y) * canvas->width + n + x],
                    tex->data[(j + r) * hRatio + (n + r) * wRatio]
                    );
            mix_colour(
                    &canvas->pixels[(y - j + 1) * canvas->width + n + x],
                    tex->data[(- j + r) * hRatio + (n + r) * wRatio]
                    );
        }
        for (int n = j; n < -j; n++)
        {
            if (oldi != i)
            {
                mix_colour(
                        &canvas->pixels[(i + y + 1) * canvas->width + n + x],
                        tex->data[(i + r) * hRatio + (n + r) * wRatio]
                        );
                mix_colour(
                        &canvas->pixels[(y - i) * canvas->width + n + x],
                        tex->data[(-i + r) * hRatio + (n + r) * wRatio]
                        );
            }
        }


        oldi = i;
        int i1 = i;
        int jj = j - 1;
        int i2 = i - 1;

        int v1 = i1 * i1 + jj * jj - r * r;
        int v2 = -(i2 * i2 + jj * jj - r * r);
        i = i1 * (v1 <= v2) + i2 * !(v1 <= v2);

        j--;
    }
}

void fill_circle(Canvas* canvas, int x, int y, int r, u32 colour)
{
    int j = 0;
    int i = r;
    int oldi = i;

    while (i > -j)
    {
        for (int n = -i; n < i + 1; n++)
        {
            if (n+x < canvas->width && (j + y) < canvas->height)
            {
            mix_colour(&canvas->pixels[(j + y) * canvas->width + n + x] , colour);
            }
            if (n+x < canvas->width && (y - j + 1) < canvas->height)
            mix_colour(&canvas->pixels[(y - j + 1) * canvas->width + n + x] , colour);
        }
        for (int n = j; n < -(j - 1); n++)
        {
            if (oldi != i)
            {
                if (n+x < canvas->width && (i + y) < canvas->height)
                {
                mix_colour(&canvas->pixels[(i + y) * canvas->width + n + x] , colour);
                }
                if (n+x < canvas->width && (y - i + 1) < canvas->height)
                mix_colour(&canvas->pixels[(y - i + 1) * canvas->width + n + x] , colour);
            }
        }


        oldi = i;
        int i1 = i;
        int jj = j - 1;
        int i2 = i - 1;

        int v1 = i1 * i1 + jj * jj - r * r;
        int v2 = -(i2 * i2 + jj * jj - r * r);
        i = i1 * (v1 <= v2) + i2 * !(v1 <= v2);

        j--;
    }
}

void ellipse(Canvas* canvas, int x, int y, int r0, int r1, u32 colour)
{
    int j = 0;
    int i = r0;
    int r02 = r0 * r0;
    int r12 = r1 * r1;
    int r2 = r02 * r12;

    while (i * r12 > -j * r02)
    {
        mix_colour(&canvas->pixels[(j + y) * canvas->width - i + x] , colour);
        mix_colour(&canvas->pixels[(j + y) * canvas->width + i + x] , colour);

        mix_colour(&canvas->pixels[(y - j) * canvas->width - i + x] , colour);
        mix_colour(&canvas->pixels[(y - j) * canvas->width + i + x] , colour);

        int i1 = i;
        int jj = j - 1;
        int i2 = i - 1;

        int v1 = i1 * i1 * r12 + jj * jj * r02 - r2;
        int v2 = -(i2 * i2 * r12 + jj * jj * r02 - r2);

        i = i1 * (v1 <= v2) + i2 * !(v1 <= v2);

        j--;
    }
    while (i > -1)
    {
        mix_colour(&canvas->pixels[(j + y) * canvas->width - i + x] , colour);
        mix_colour(&canvas->pixels[(j + y) * canvas->width + i + x] , colour);

        mix_colour(&canvas->pixels[(y - j) * canvas->width + i + x] , colour);
        mix_colour(&canvas->pixels[(y - j) * canvas->width - i + x] , colour);

        int ii = i - 1;
        int j1 = j;
        int j2 = j - 1;

        int v1 = -(ii * ii * r12 + j1 * j1 * r02 - r2);
        int v2 = ii * ii * r12 + j2 * j2 * r02 - r2;

        j = j1 * (v1 <= v2) + j2 * !(v1 <= v2);

        i--;
    }
}


void bounded_circle(Canvas* canvas, Circle* c, ParametricLine* l, u32 colour)
{
    // Finding Intersection
    Vector3d center = {.x = c->cx, .y = c->cy, .z = 0};
    Vector3d line_base = {.x = l->x0, .y = l->y0, .z = 0};
    Vector3d base_center = sub(&(center), &line_base);
    Vector3d line_dir = {.x = l->dx, .y = l->dy, .z = 0};
    line_dir = normalise(&line_dir);
    f32 tca = dot(&base_center, &line_dir);

    if (squared_magnitude(&base_center) > c->r * c->r)
    {
        circle(canvas, c->cx, c->cy, c->r, colour);
        return;
    }
    Vector3d rejected = reject(&base_center, &(line_dir));
    if (squared_magnitude(&rejected) > c->r * c->r)
    {
        circle(canvas, c->cx, c->cy, c->r, colour);
        return;
    }
    float thc = sqrt(c->r * c->r - squared_magnitude(&rejected));

    f32 t0 = tca - thc;
    f32 t1 = tca + thc;
    Line pl;

    Point inter_0 = {.x = l->x0 + t0 * l->dx, .y = l->y0 + t0 * l->dy};
    Point inter_1 = {.x = l->x0 + t1 * l->dx, .y = l->y0 + t1 * l->dy};

    pl.x0 = inter_0.x;
    pl.y0 = inter_0.y;
    pl.x1 = inter_1.x;
    pl.y1 = inter_1.y;

    ParametricLine normal;
    get_line_normal(&pl, &normal);

    Vector3d normal_dir = {normal.dx, normal.dy, 0};
    normal_dir = normalise(&normal_dir);

    Vector3d diff;
    diff.z = 0;

    int j = 0;
    int i = c->r;
    while (i > -j)
    {
        diff.x = (i + c->cx - inter_0.x);
        diff.y = (j + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
        mix_colour(&canvas->pixels[(j + c->cy) * canvas->width + i + c->cx] , colour);
        }
        diff.x = (-i + c->cx - inter_0.x);
        diff.y = (j + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
        mix_colour(&canvas->pixels[(j + c->cy) * canvas->width - i + c->cx] , colour);
        }

        diff.x = (i + c->cx - inter_0.x);
        diff.y = (-j + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
            mix_colour(&canvas->pixels[(c->cy - j) * canvas->width + i + c->cx] , colour);
        }

        diff.x = (-i + c->cx - inter_0.x);
        diff.y = (-j + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
            mix_colour(&canvas->pixels[(c->cy - j) * canvas->width - i + c->cx] , colour);
        }

        diff.x = (j + c->cx - inter_0.x);
        diff.y = (i + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
        mix_colour(&canvas->pixels[(i + c->cy) * canvas->width + j + c->cx] , colour);
        }
        diff.x = (-j + c->cx - inter_0.x);
        diff.y = (i + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
        mix_colour(&canvas->pixels[(i + c->cy) * canvas->width - j + c->cx] , colour);
        }

        diff.x = (j + c->cx - inter_0.x);
        diff.y = (-i + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
            mix_colour(&canvas->pixels[(c->cy - i) * canvas->width + j + c->cx] , colour);
        }

        diff.x = (-j + c->cx - inter_0.x);
        diff.y = (-i + c->cy - inter_0.y);
        if (dot(&diff, &normal_dir) > 0)
        {
            mix_colour(&canvas->pixels[(c->cy - i) * canvas->width - j + c->cx] , colour);
        }

        int i1 = i;
        int jj = j - 1;
        int i2 = i - 1;

        int v1 = i1 * i1 + jj * jj - c->r * c->r;
        int v2 = -(i2 * i2 + jj * jj - c->r * c->r);

        i = i1 * (v1 <= v2) + i2 * !(v1 <= v2);

        j--;
    }
}

void fill_ellipse(Canvas* canvas, int x, int y, int r0, int r1, u32 colour)
{
    int j = 0;
    int i = r0;
    int oldj = j;
    int r02 = r0 * r0;
    int r12 = r1 * r1;
    int r2 = r02 * r12;

    while (i * r12 > -j * r02)
    {
        for (int n = -i; n < i; n++)
        {

            mix_colour(&canvas->pixels[(j + y) * canvas->width + n + x] , colour);
            mix_colour(&canvas->pixels[(y - j + 1) * canvas->width + n + x] , colour);
        }

        int i1 = i;
        int jj = j - 1;
        int i2 = i - 1;

        int v1 = i1 * i1 * r12 + jj * jj * r02 - r2;
        int v2 = -(i2 * i2 * r12 + jj * jj * r02 - r2);

        i = i1 * (v1 <= v2) + i2 * !(v1 <= v2);

        j--;
    }
    while (i > -1)
    {
        for (int n = -i; n < i; n++)
        {
            if (oldj != j)
            {
                mix_colour(&canvas->pixels[(j + y) * canvas->width + n + x] , colour);
                mix_colour(&canvas->pixels[(y - j + 1) * canvas->width + n + x] , colour);
            }
        }
        oldj = j;
        int ii = i - 1;
        int j1 = j;
        int j2 = j - 1;

        int v1 = -(ii * ii * r12 + j1 * j1 * r02 - r2);
        int v2 = ii * ii * r12 + j2 * j2 * r02 - r2;

        j = j1 * (v1 <= v2) + j2 * !(v1 <= v2);

        i--;
    }
}

void triangle(Canvas* canvas, int x0, int y0, int x1, int y1, int x2, int y2, u32 colour)
{
    line(canvas, x0, y0, x1, y1, colour);
    line(canvas, x0, y0, x2, y2, colour);
    line(canvas, x2, y2, x1, y1, colour);
}



void fill_triangle(Canvas* canvas, int x0, int y0, int x1, int y1, int x2, int y2, u32 colour)
{
    // (y - y0) = dy / dx * (x - x0)

    sortX(&y0, &x0, &y1, &x1, &y2, &x2);

    int dx01 = x1 - x0;
    int dy01 = y1 - y0;
    int dx02 = x2 - x0;
    int dy02 = y2 - y0;

    int left_x = -1;
    int right_x = -1;

    for (int j = y0; j < y1; j++)
    {
        left_x = dy01 != 0 ? (j - y0) * dx01 / dy01 + x0 : x0;
        right_x = dy02 != 0 ? (j - y0) * dx02 / dy02 + x0: x0;
        if (left_x > right_x)
        {
            swap(&left_x, &right_x);
        }
        for (int i = left_x; i <= right_x; i++)
        {
            mix_colour(&canvas->pixels[j * canvas->width + i] , colour);
        }
    }

    int dx21 = x1 - x2;
    int dy21 = y1 - y2;
    int dx20 = x0 - x2;
    int dy20 = y0 - y2;

    for (int j = y1; j < y2; ++j)
    {
        left_x = dy21 != 0 ? (j - y2) * dx21 / dy21 + x2 : x2;
        right_x = dy20 != 0 ? (j - y2) * dx20 / dy20 + x2 : x2;
        if (left_x > right_x)
        {
            swap(&left_x, &right_x);
        }
        for (int i = left_x; i <= right_x; i++)
        {
            mix_colour(&canvas->pixels[j * canvas->width + i] , colour);
        }
    }
}

void fill_triangle_bary(Canvas* canvas, int x0, int y0, int x1, int y1, int x2, int y2, u32 c0, u32 c1, u32 c2)
{
    sortX(&y0, &x0, &y1, &x1, &y2, &x2);

    int dx02 = x1 - x0;
    int dy02 = y1 - y0;
    int dx03 = x2 - x0;
    int dy03 = y2 - y0;
    float b[3] = {0, 0, 0};
    u8 comp1[4] = {0, 0, 0, 0};
    u8 comp2[4] = {0, 0, 0, 0};
    u8 comp3[4] = {0, 0, 0, 0};
    u32 col = 0;
    int left_x = -1;
    int right_x = -1;

    for (int j = y0; j < y1; j++)
    {
        left_x = dy02 != 0 ? (j - y0)*dx02/dy02 + x0 : x0;
        right_x = dy03 != 0 ? (j - y0)*dx03/dy03 + x0 : x0;
        if (left_x > right_x)
        {
            swap(&left_x, &right_x);
        }
        for (int i = left_x; i <= right_x; i++)
        {
            col = 0;
            bary(x0, y0, x1, y1, x2, y2, i, j, b);
            unpack(comp1, &c0);
            unpack(comp2, &c1);
            unpack(comp3, &c2);
            for (int k = 0; k < 4; k++)
            {
                comp1[k] *= b[0];
                comp2[k] *= b[1];
                comp3[k] *= b[2];
                col |= ((comp1[k] + comp2[k] + comp3[k]) << (8 * k));
            }
            mix_colour(&canvas->pixels[j * canvas->width + i] , col);
        }
    }
    int dx22 = x1 - x2;
    int dy22 = y1 - y2;
    int dx21 = x0 - x2;
    int dy21 = y0 - y2;

    for (int j = y1; j < y2; j++)
    {
        left_x = dy22 != 0 ? (j - y2) * dx22 / dy22 + x2 : x2;
        right_x = dy21 != 0 ? (j - y2) * dx21 / dy21 + x2 : x2;
        if (left_x > right_x)
        {
            swap(&left_x, &right_x);
        }
        for (int i = left_x; i <= right_x; i++)
        {
            col = 0;
            bary(x0, y0, x1, y1, x2, y2, i, j, b);
            unpack(comp1, &c0);
            unpack(comp2, &c1);
            unpack(comp3, &c2);
            for (int k = 0; k < 4; k++)
            {
                comp1[k] *= b[0];
                comp2[k] *= b[1];
                comp3[k] *= b[2];
                col |= ((comp1[k] + comp2[k] + comp3[k]) << (8 * k));
            }
            mix_colour(&canvas->pixels[j * canvas->width + i] , col);
        }
    }
}

void fill_triangle_texture(Canvas* canvas, int x0, int y0, int x1, int y1, int x2, int y2, Texture* tex, int u0, int v0, int u1, int v1, int u2, int v2)
{
    const int stride = 4;

    int data[4 * 3] =
    {x0, y0, u0, v0,
        x1, y1, u1, v1,
        x2, y2, u2, v2};

    sort(data, 4, 3, 1);

    x0 = data[0];
    y0 = data[1];
    u0 = data[2];
    v0 = data[3];

    x1 = data[0 + stride];
    y1 = data[1 + stride];
    u1 = data[2 + stride];
    v1 = data[3 + stride];

    x2 = data[0 + 2 * stride];
    y2 = data[1 + 2 * stride];
    u2 = data[2 + 2 * stride];
    v2 = data[3 + 2 * stride];

    int dx02 = x1 - x0;
    int dy02 = y1 - y0;
    int dx03 = x2 - x0;
    int dy03 = y2 - y0;

    int du12 = u1 - u0;
    int dv12 = v1 - v0;
    int du13 = u2 - u0;
    int dv13 = v2 - v0;

    int v = v0;

    int left_x = -1;
    int right_x = -1;
    int left_u = -1;
    int right_u = -1;

    for (int j = y0; j < y1; j++)
    {
        v += ((dv12) / (y1 - y0));
        left_x = dy02 != 0 ? (j - y0) * dx02 / dy02 + x0 : x0;
        right_x = dy03 != 0 ? (j - y0) * dx03 / dy03 + x0 : x0;

        left_u = dv12 != 0  ? (v - v0) * du12 / dv12 + u0 : u0;
        right_u = dv13 != 0  ? (v - v0) * du13 / dv13 + u0 : u0;
        int u = left_u;

        if (left_x > right_x)
        {
            swap(&left_x, &right_x);
        }
        for (int i = left_x; i < right_x; i++)
        {
            mix_colour(&canvas->pixels[j * canvas->width + i] ,
                    tex->data[((v * tex->height) / 1001 * tex->width) + (u * tex->width) / 1001]);
            u += ((right_u - left_u) / (right_x - left_x));
        }
        mix_colour(&canvas->pixels[j * canvas->width + right_x] ,
                tex->data[((v * tex->height) / 1001 * tex->width) + (u * tex->width) / 1001]);
    }

    int dx22 = x1 - x2;
    int dy22 = y1 - y2;
    int dx21 = x0 - x2;
    int dy21 = y0 - y2;

    int du32 = u1 - u2;
    int dv32 = v1 - v2;
    int du31 = u0 - u2;
    int dv31 = v0 - v2;

    v = v1;

    for (int y = y1; y < y2; ++y)
    {
        v += ((-dv32) / (y2 - y1));
        left_x = dy22 != 0 ? (y - y2)*dx22/dy22 + x2 : x2;
        right_x = dy21 != 0 ? (y - y2)*dx21/dy21 + x2 : x2;

        left_u = dv32 != 0  ? (v - v2)*du32/dv32 + u2 : u2;
        right_u = dv31 != 0  ? (v - v2)*du31/dv31 + u2 : u2;
        int u = right_u;

        if (left_x > right_x) swap(&left_x, &right_x);

        for (int x = left_x; x < right_x; ++x)
        {
            mix_colour(&canvas->pixels[y * canvas->width + x] ,
                    tex->data[((v * tex->height) / 1001 * tex->width) + (u * tex->width) / 1001]);
            u += ((left_u - right_u) / (right_x - left_x));
        }

        mix_colour(&canvas->pixels[y * canvas->width + right_x] ,
                tex->data[((v * tex->height) / 1001 * tex->width) + (u * tex->width) / 1001]);
    }
}


void old_parametric_cubic(Canvas* canvas, 
        int x0, int y0,
        float a0, float b0, float c0, float d0,
        float a1, float b1, float c1, float d1,
        int seg_num,
        u32 colour
        )
{
    float step_size = 1.0f / seg_num;
    float curr_x0 = d0 + x0;
    float curr_x1 = 0;

    float dx0 = 
        c0 * step_size + 
        b0 * step_size * step_size +
        a0 * step_size * step_size * step_size;
    float dx1 = 
        2 * b0 * step_size * step_size + 
        6 * a0 * step_size * step_size * step_size;
    float dx2 = 
        6 * a0 * step_size * step_size * step_size;

    float curr_y0 = d1 + y0;
    float curr_y1 = 0;

    float dy0 = 
        c1 * step_size + 
        b1 * step_size * step_size +
        a1 * step_size * step_size * step_size;
    float dy1 = 
        2 * b1 * step_size * step_size + 
        6 * a1 * step_size * step_size * step_size;
    float dy2 = 
        6 * a1 * step_size * step_size * step_size;
    
    for (int i = 0; i <= seg_num; i++)
    {
        curr_x1 = curr_x0 + dx0;
        dx0 += dx1;
        dx1 += dx2;

        curr_y1 = curr_y0 + dy0; 
        dy0 += dy1;
        dy1 += dy2;
        
        line(canvas, curr_x0, curr_y0, curr_x1, curr_y1, colour);
        curr_x0 = curr_x1;
        curr_y0 = curr_y1;
    }
}
void parametric_cubic(Canvas* canvas, ParametricCubic2D* pc2, u32 colour)
{
    float step_size = 1.0f / pc2->seg_num;
    float curr_x0 = pc2->dx + pc2->p0.x;
    float curr_x1 = 0;

    float dx0 = 
        pc2->cx * step_size + 
        pc2->bx * step_size * step_size +
        pc2->ax * step_size * step_size * step_size;
    float dx1 = 
        2 * pc2->bx * step_size * step_size + 
        6 * pc2->ax * step_size * step_size * step_size;
    float dx2 = 
        6 * pc2->ax * step_size * step_size * step_size;

    float curr_y0 = pc2->dy + pc2->p0.y;
    float curr_y1 = 0;

    float dy0 = 
        pc2->cy * step_size + 
        pc2->by * step_size * step_size +
        pc2->ay * step_size * step_size * step_size;
    float dy1 = 
        2 * pc2->by * step_size * step_size + 
        6 * pc2->ay * step_size * step_size * step_size;
    float dy2 = 
        6 * pc2->ay * step_size * step_size * step_size;
    
    for (int i = 0; i <= pc2->seg_num; i++)
    {
        curr_x1 = curr_x0 + dx0;
        dx0 += dx1;
        dx1 += dx2;

        curr_y1 = curr_y0 + dy0; 
        dy0 += dy1;
        dy1 += dy2;
        
        line(canvas, curr_x0, curr_y0, curr_x1, curr_y1, colour);
        curr_x0 = curr_x1;
        curr_y0 = curr_y1;
    }
}

void bspline(Canvas* canvas, BSpline* bsp, SimpleBrush* sb)
{
    int delta = bsp->order- 1;
    int draw = 0;
    int i = 0;
    float omega;
    Point p0 = {0, 0};
    Point p1 = {0, 0};
    float u = 0.0f;
    float* x_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));
    float* y_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));

    Line l = 
    {
        .x0 = p0.x,
        .y0 = p0.y,
        .x1 = p1.x,
        .y1 = p1.y,
    };

    float inc = 
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1])
        / bsp->seg_count;

    for (u = bsp->knots[bsp->order - 1]; u < bsp->knots[bsp->coeffs_count]; u+=inc)
    {
        delta += (u >= bsp->knots[delta + 1]);
        for (int j = 0; j < bsp->order; j++)
        {
            x_coeffs_copy[j] = bsp->x_coeffs[delta - j];
            y_coeffs_copy[j] = bsp->y_coeffs[delta - j];
        }
        for (int j = bsp->order; j < delta + 1; j++)
        {
            x_coeffs_copy[j] = 0;
            y_coeffs_copy[j] = 0;
        }
        for (int r = bsp->order; r >= 2; r--)
        {
            i = delta;
            for (int s = 0; s <= r-2; s++)
            {
                omega = 0;
                float diff = bsp->knots[i+r - 1] - bsp->knots[i];
                if (diff > 0)
                {
                omega =
                    (u - bsp->knots[i]) / 
                    (diff);
                }
                x_coeffs_copy[s] = 
                    (omega) * x_coeffs_copy[s] + 
                    (1-omega) * x_coeffs_copy[s+1];

                y_coeffs_copy[s] = 
                    (omega) * y_coeffs_copy[s] + 
                    (1-omega) * y_coeffs_copy[s+1];

                i--;
            }
        }
        l.x1 = x_coeffs_copy[0];
        l.y1 = y_coeffs_copy[0];
        // 333
        if (draw > 0)
        {
            new_line(canvas, &l, sb);
        }
        l.x0 = l.x1;
        l.y0 = l.y1;
        draw++;
    }
    free(x_coeffs_copy);
    free(y_coeffs_copy);
}

void bspline_modify(Canvas* canvas, BSpline* bsp, SimpleBrush* b, void* opts,
        void (*modify) (Canvas* c, BSpline* bsp, float u, Line*, SimpleBrush* b, void* opts))
{
    int delta = bsp->order- 1;
    int draw = 0;
    int i = 0;
    float omega;
    Point p0 = {0, 0};
    Point p1 = {0, 0};
    float u = 0.0f;
    float* x_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));
    float* y_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));

    float inc = 
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1])
        / bsp->seg_count;

    Line l = 
    {
        .x0 = p0.x,
        .y0 = p0.y,
        .x1 = p1.x,
        .y1 = p1.y,
    };
    for (u = bsp->knots[bsp->order - 1]; u < bsp->knots[bsp->coeffs_count]; u+=inc)
    {
        delta += (u >= bsp->knots[delta + 1]);
        for (int j = 0; j < bsp->order; j++)
        {
            x_coeffs_copy[j] = bsp->x_coeffs[delta - j];
            y_coeffs_copy[j] = bsp->y_coeffs[delta - j];
        }
        for (int j = bsp->order; j < delta + 1; j++)
        {
            x_coeffs_copy[j] = 0;
            y_coeffs_copy[j] = 0;
        }
        for (int r = bsp->order; r >= 2; r--)
        {
            i = delta;
            for (int s = 0; s <= r-2; s++)
            {
                omega = 0;
                float diff = bsp->knots[i+r - 1] - bsp->knots[i];
                if (diff > 0)
                {
                omega =
                    (u - bsp->knots[i]) / 
                    (diff);
                }
                x_coeffs_copy[s] = 
                    (omega) * x_coeffs_copy[s] + 
                    (1-omega) * x_coeffs_copy[s+1];

                y_coeffs_copy[s] = 
                    (omega) * y_coeffs_copy[s] + 
                    (1-omega) * y_coeffs_copy[s+1];

                i--;
            }
        }
        l.x1 = x_coeffs_copy[0];
        l.y1 = y_coeffs_copy[0];
        modify(canvas, bsp, u, &l, b, opts);
        // 333

        if (draw > 0)
        {
            new_line(canvas, &l, b);
            //circle(canvas, l.x0, l.y0, b->r, b->colour);
            //line(canvas, l.x0, l.y0, l.x1, l.y1,  b->colour);
        }
        (void)p0;
        l.x0 = l.x1;
        l.y0 = l.y1;
        draw++;
    }
    free(x_coeffs_copy);
    free(y_coeffs_copy);
}

void bspline_lod_test(Canvas* canvas, BSpline* bsp, SimpleBrush* sb, u32 stroke_count)
{
    int delta = bsp->order- 1;
    int draw = 0;
    int i = 0;
    float omega;
    Point p0 = {0, 0};
    Point p1 = {0, 0};
    float u = 0.0f;
    float* x_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));
    float* y_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));
    float sub_knots[8] = 
    {
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
    };
    float sub_x_coeffs[4];
    float sub_y_coeffs[4];
    BSpline sub_spline;

    Line l = 
    {
        .x0 = p0.x,
        .y0 = p0.y,
        .x1 = p1.x,
        .y1 = p1.y,
    };

    float inc = 
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1])
        / stroke_count;

    for (u = bsp->knots[bsp->order - 1]; u < bsp->knots[bsp->coeffs_count]; u+=inc)
    {
        delta += (u >= bsp->knots[delta + 1]);
        for (int j = 0; j < bsp->order; j++)
        {
            x_coeffs_copy[j] = bsp->x_coeffs[delta - j];
            y_coeffs_copy[j] = bsp->y_coeffs[delta - j];
        }
        for (int j = bsp->order; j < delta + 1; j++)
        {
            x_coeffs_copy[j] = 0;
            y_coeffs_copy[j] = 0;
        }
        for (int r = bsp->order; r >= 2; r--)
        {
            i = delta;
            for (int s = 0; s <= r-2; s++)
            {
                omega = 0;
                float diff = bsp->knots[i+r - 1] - bsp->knots[i];
                if (diff > 0)
                {
                    omega =
                        (u - bsp->knots[i]) / 
                        (diff);
                }
                x_coeffs_copy[s] = 
                    (omega) * x_coeffs_copy[s] + 
                    (1-omega) * x_coeffs_copy[s+1];

                y_coeffs_copy[s] = 
                    (omega) * y_coeffs_copy[s] + 
                    (1-omega) * y_coeffs_copy[s+1];

                i--;
            }
        }
        l.x1 = x_coeffs_copy[0];
        l.y1 = y_coeffs_copy[0];

        sub_x_coeffs[0] = l.x0;
        sub_x_coeffs[1] = fmax(l.x1, l.x0) + abs(l.x0 - l.x1);

        sub_x_coeffs[2] = fmin(l.x0, l.x1) - abs(l.x0 - l.x1);
        sub_x_coeffs[3] = l.x1;

        sub_y_coeffs[0] = l.y0;
        sub_y_coeffs[1] = fmin(l.y0, l.y1) - 10.0f;
        sub_y_coeffs[2] = fmin(l.y0, l.y1) - 10.0f;
        sub_y_coeffs[3] = l.y1;

            sub_spline.knots = sub_knots;
            sub_spline.knot_count = 8;
            sub_spline.x_coeffs = sub_x_coeffs;
            sub_spline.y_coeffs = sub_y_coeffs;
            sub_spline.coeffs_count = 4;
            sub_spline.order = 4;
            sub_spline.seg_count = 1e2;

        // 333
        if (draw > 0)
        {
            bspline(canvas, &sub_spline, sb);
        }
        l.x0 = l.x1;
        l.y0 = l.y1;
        draw++;
    }
    free(x_coeffs_copy);
    free(y_coeffs_copy);
}
void bspline_lina_test(Canvas* canvas, BSpline* bsp, SimpleBrush* sb, u32 stroke_count)
{
    int delta = bsp->order- 1;
    int draw = 0;
    int i = 0;
    float omega;
    Point p0 = {0, 0};
    Point p1 = {0, 0};
    float u = 0.0f;
    float* x_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));
    float* y_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));

    Line l = 
    {
        .x0 = p0.x,
        .y0 = p0.y,
        .x1 = p1.x,
        .y1 = p1.y,
    };

    float inc = 
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1])
        / stroke_count;

    for (u = bsp->knots[bsp->order - 1]; u < bsp->knots[bsp->coeffs_count]; u+=inc)
    {
        delta += (u >= bsp->knots[delta + 1]);
        for (int j = 0; j < bsp->order; j++)
        {
            x_coeffs_copy[j] = bsp->x_coeffs[delta - j];
            y_coeffs_copy[j] = bsp->y_coeffs[delta - j];
        }
        for (int j = bsp->order; j < delta + 1; j++)
        {
            x_coeffs_copy[j] = 0;
            y_coeffs_copy[j] = 0;
        }
        for (int r = bsp->order; r >= 2; r--)
        {
            i = delta;
            for (int s = 0; s <= r-2; s++)
            {
                omega = 0;
                float diff = bsp->knots[i+r - 1] - bsp->knots[i];
                if (diff > 0)
                {
                    omega =
                        (u - bsp->knots[i]) / 
                        (diff);
                }
                x_coeffs_copy[s] = 
                    (omega) * x_coeffs_copy[s] + 
                    (1-omega) * x_coeffs_copy[s+1];

                y_coeffs_copy[s] = 
                    (omega) * y_coeffs_copy[s] + 
                    (1-omega) * y_coeffs_copy[s+1];

                i--;
            }
        }
        l.x1 = x_coeffs_copy[0];
        l.y1 = y_coeffs_copy[0];

        // 333
        if (draw > 0)
        {
            bspline_lina(canvas, sb,l.x0, l.y0);
        }
        l.x0 = l.x1;
        l.y0 = l.y1;
        draw++;
    }
    free(x_coeffs_copy);
    free(y_coeffs_copy);
}

void bspline_circ_test(Canvas* canvas, BSpline* bsp, SimpleBrush* sb, u32 stroke_count)
{
    int delta = bsp->order- 1;
    int draw = 0;
    int i = 0;
    float omega;
    Point p0 = {0, 0};
    Point p1 = {0, 0};
    float u = 0.0f;
    float* x_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));
    float* y_coeffs_copy = malloc(bsp->coeffs_count * sizeof(float));

    Line l = 
    {
        .x0 = p0.x,
        .y0 = p0.y,
        .x1 = p1.x,
        .y1 = p1.y,
    };

    float inc = 
        (bsp->knots[bsp->coeffs_count] - bsp->knots[bsp->order - 1])
        / stroke_count;

    for (u = bsp->knots[bsp->order - 1]; u < bsp->knots[bsp->coeffs_count]; u+=inc)
    {
        delta += (u >= bsp->knots[delta + 1]);
        for (int j = 0; j < bsp->order; j++)
        {
            x_coeffs_copy[j] = bsp->x_coeffs[delta - j];
            y_coeffs_copy[j] = bsp->y_coeffs[delta - j];
        }
        for (int j = bsp->order; j < delta + 1; j++)
        {
            x_coeffs_copy[j] = 0;
            y_coeffs_copy[j] = 0;
        }
        for (int r = bsp->order; r >= 2; r--)
        {
            i = delta;
            for (int s = 0; s <= r-2; s++)
            {
                omega = 0;
                float diff = bsp->knots[i+r - 1] - bsp->knots[i];
                if (diff > 0)
                {
                    omega =
                        (u - bsp->knots[i]) / 
                        (diff);
                }
                x_coeffs_copy[s] = 
                    (omega) * x_coeffs_copy[s] + 
                    (1-omega) * x_coeffs_copy[s+1];

                y_coeffs_copy[s] = 
                    (omega) * y_coeffs_copy[s] + 
                    (1-omega) * y_coeffs_copy[s+1];

                i--;
            }
        }
        l.x1 = x_coeffs_copy[0];
        l.y1 = y_coeffs_copy[0];

        ParametricLine normal;
        get_line_normal(&l, &normal);
        normal.x0 = l.x1;
        normal.y0 = l.y1;
        Circle circ = {.cx = (l.x0 + l.x1)/2, .cy = (l.y0 + l.y1)/2, .r = 20};

        // 333
        if (draw > 0)
        {
            //bspline(canvas, &sub_spline, sb);
            bounded_circle(canvas, &circ, &normal, sb->colour);
        }
        l.x0 = l.x1;
        l.y0 = l.y1;
        draw++;
    }
    free(x_coeffs_copy);
    free(y_coeffs_copy);
}
/*void fill(Canvas* canvas, Shape type, void* shape)
{
    switch(shape)
    {
        case LINE:
            {
            line(canvas, )
            }
    }
}
*/
