#include <stdint.h>
#ifndef TYPESH
#define TYPESH

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t i32;
typedef int64_t i64;
typedef int16_t i16;
typedef int8_t i8;
typedef float f32;
typedef double f64;
typedef int Errno;


#define COS_PI_OVER_8 (0.9238795f)
#define SIN_PI_OVER_8 (0.3826834f)
#define PI (3.14159265359f)
#define INV_PI (0.31830988618f)

typedef struct Point
{
    int x;
    int y;
} Point;
typedef struct SimpleBrush
{
    u32 r;
    u32 colour;
} SimpleBrush;

typedef struct Line
{
    int x0;
    int y0;
    int x1;
    int y1;
} Line;

typedef struct ParametricLine
{
    int x0;
    int y0;
    int dx;
    int dy;
    float t;
} ParametricLine;

typedef struct Circle
{
    int r;
    int cx;
    int cy;
} Circle;

typedef struct Rectangle
{
    int x;
    int y;
    int w;
    int h;
    u32 colour;
} Rectangle;

typedef struct Ellipse
{
   int x;
   int y;
   int r0;
   int r1;
} Ellipse;

typedef struct Triangle
{
    int x0;
    int y0;
    int x1;
    int y1;
    int x2;
    int y2;
    u32 colour;
} Triangle;

typedef struct ParametricCubic2D
{
    Point p0;
    float ax;
    float bx;
    float cx;
    float dx;
    float ay;
    float by;
    float cy;
    float dy;
    int seg_num;
} ParametricCubic2D;

typedef struct BSpline
{
    float* knots;
    int order;
    int knot_count;
    float* x_coeffs;
    float* y_coeffs;
    int coeffs_count;
    int seg_count;
} BSpline;


typedef enum Shape
{
    LINE,
    RECTANGLE,
    CIRCLE,
    ELLIPSE,
    TRIANGLE,
    TOTAL
} Shape;


#endif
