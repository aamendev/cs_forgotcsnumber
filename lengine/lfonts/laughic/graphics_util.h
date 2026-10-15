#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "types.h"
#include "stb_image.h"
#include "./stb_image_write.h"
#ifndef GRAPHICSUTIL
#define GRAPHICSUTIL

typedef enum
{
    PNG,
    JPG,
    PPM,
    FORMATS
} Format;

typedef enum
{
    ChannelRed = 0,
    ChannelGreen,
    ChannelBlue,
    ChannelAlpha
} Channel;

typedef struct Canvas
{
    u32 default_colour;
   const char* name;
   u32* pixels;
   size_t width;
   size_t height;
} Canvas;
typedef struct SignedCanvas
{
   const char* name;
   i64* pixels;
   size_t width;
   size_t height;
} SignedCanvas;
typedef struct ColourHistogramBar
{
    u32 col;
    u32 count;
} ColourHistogramBar;

typedef struct LargeCanvas
{
   const char* name;
   u64* rg;
   u64* ba;
   size_t width;
   size_t height;
} LargeCanvas;

typedef struct Texture
{
    const char* path;
    u32* data;
    size_t width;
    size_t height;
} Texture;

void mix_component(u8* c1, u8 c2, u8 a);
void unpack(u8* , u32*);
void long_unpack(u32* comp, u64* rg, u64* ba);
void pack(u8* comp, u32* c);
void long_pack(u32* comp, u64* rg, u64* ba);
void mix_colour(u32* c1, u32 c2);
u32 complement(u32 colour);
void resize(Texture* src, u32 wn, u32 hn, u32** dst);
void downscale_canvas(Canvas* src, Canvas* dst);
void print_channels(u32 col);
int is_grey(u32 col);
u32 weighted_sum(u32* cols, float* means, u32 count);

void mandelbrot(Canvas* canvas, u16 iterations, u32 base_colour);
void julia(Canvas* canvas, u16 iterations, float zx, float zy, u32 bg);
//void interpolate(float a0, float a1, float w);
//float randgradient(int x0, int y0, float* x, float* y);
//float dotGridGrading(int x0, int y0, float x, float y);
//float perlin(float x, float y)
void newton(Canvas* canvas, u16 iterations, u32 base_colour);
void newton2(Canvas* cavnas, u16 iterations, void (*func)(float, float, float*, float*), void (*deri)(float, float, float*, float*), float* roots, u16 nRoots);

// Per Colour Operations
void set_opacity(u32* col, u8 val);
void set_channel(u32* col, u8 val, Channel channel);
u8 get_channel(u32* col, Channel channel);

f32 colour_distance(u32 col1, u32 col2);

// Signed

void signed_unpack(i16* comp, i64* c);
void signed_pack(i16* comp, i64* c);
void signed_to_unsigned(i64* sgn, u32* un);
void unsigned_to_signed(i64* sgn, u32* un);

// Comp
i8 compare_with_comps(u32 c, u8 t);

void get_line_normal(Line* l, ParametricLine* normal);
void line_to_parametric(Line* l, ParametricLine* pl);
void parametric_to_line(ParametricLine* pl, Line* l);

// Canvas
void read_to_canvas(Canvas* c, char* path);
void read_texture(Texture* tex);
int save_to_img(Canvas* canvas, Format format, char* dst);
u32 get_cols(Canvas* c, u32* cols, u32 size);
u32 get_cols_histogram(Canvas* c, ColourHistogramBar* bar, u32 size);
#endif
