#ifndef PROCESSING_H
#define PROCESSING_H
#include "../graphics_util.h"
#include "processing_utils.h"
#include "../colours.h"
#include "../shapes.h"
#include <float.h>

// Modifiers
void translate(Canvas* c, i32 valx, i32 valy);
void mult_contrast(Canvas* c, float val);
void inc_channel(Canvas*, Channel channel, i16 val);
void inc_brightness(Canvas* c, i16 val);
void invert(Canvas* c);
void filter(Canvas* c, Channel channel);
void filter_threshold(Canvas* c, Channel channel, u8 t);
void clamp_channel(Canvas*, Channel channel, u8 t0, u8 t1);
void clamp_all(Canvas*, u8 t0, u8 t1);
void grey_scale(Canvas*);
void weighted_subtract_img(Canvas* c1, u32* c2, float weight);
void weighted_add_img(Canvas* c1, u32* c2, float weight);

void populisity_quantize_colours(Canvas* c, u32 new_count, u32 expected_old_count, u32* new_cols);

void median_cut_quantize_colours(Canvas* c, u32 new_count, u32 expected_old_count);
// median_cut_quantize_colours_helpers
ColourBox createColourBox(ColourHistogramBar* bars, u32 bars_count, u32 level);
ColourBox* find_box_to_split(ColourBox* root);
void split_box(ColourBox* root, ColourBox* to_split);
void sort_bars(ColourHistogramBar* bars, u32 bars_count);

// TODO: Histogram Methods
//void generate_histograms(Canvas* c);

// Filters:

typedef enum FilterDirection
{
    FilterX,
    FilterY
} FilterDirection;

// generic helpers
void signed_general_linear_1d_filter(i64* s, u32 w, u32 h, i32* h1, u32 s1, FilterDirection);
void general_linear_1d_filter(Canvas* s, i32* h1, u32 s1, FilterDirection);
void long_general_linear_1d_filter(LargeCanvas* s, i32* h1, u32 s1, FilterDirection);

void general_linear_separated_filter(Canvas* s, i32* h1, u32 s1, i32* h2, u32 s2);
void s_general_linear_separated_filter(i64* s, u32 w, u32 h, i32* h1, u32 s1, i32* h2, u32 s2);
void long_linear_separated_filter(LargeCanvas* s, i32* h1, u32 s1, i32* h2, u32 s2);

void signed_general_linear_separated_filter(Canvas* s, i32* h1, u32 s1, i32* h2, u32 s2);
void general_2d_generic_operator_filter(Canvas* s, i32* h1, u32 s1, 
        u32 val_init, i32 normalize_factor, void (*op)(u32* a, u32 b, i32* h1, u32 s1));
void s_general_2d_generic_operator_filter(
i64* s, u32 width, u32 height, i32* h1, u32 s1, 
        i64 val_init, i32 normalize_factor, void (*op)(i64* a, i64 b, i32* h1, u32 s1)
        );
// end generic helpers

void jitter_1d_filter(Canvas* s, u32 s1, FilterDirection fd);
void jitter_2d_filter(Canvas* s, u32 s1);
void box_filter(Canvas* s, u32 dim);
void gaussian_filter(Canvas* s, float sigma);
void binomial_filter(Canvas* s, u32 rad);
void s_binomial_filter(i64* s, u32 w, u32 h , u32 rad);
void long_binomial_filter(LargeCanvas* s, u32 rad);
void diff_of_gaussian(Canvas* s, float s1, float s2, SignedCanvas* sc);
void min_filter(Canvas* s, u32 dim);
void max_filter(Canvas* s, u32 dim);
void jitter_filter(Canvas* s, u32 dim);
void median_filter(Canvas* s, u32 dim);
void sobel_filter_with_grads(Canvas* s, float* grad);
void sobel_filter_with_grads_signed(SignedCanvas* s, float* grad_val);
void sobel_filter(Canvas* s);
void simulate_acii_edges(Canvas* s, unsigned char* chars, u32 char_count, u32 char_pixel_size, char* path);
void downscale(Canvas* s, float a);
void canny_filter(Canvas* s, u8 t_low, u8 t_high);

void sharpen(Canvas* s, float sharpen_factor);
void harris_filter(Canvas* s, float alpha, float threshold);

void dilate(Canvas* s, i32* dilate_kernel, u32 k_width);
void erode(Canvas* s, i32* erode_kernel, u32 k_width);

// Helpers
u8 canny_get_orientation(i64 dx, i64 dy);
u8 canny_is_local_max(u32* mag, u32 w, u32 x, u32 y, u32 orientation, u8 t_lo);
void canny_trace_contour(u32* maximas, u32* mag,
        u32* contour, u32 w, u32 h, u32 x, u32 y, u8 t_low);
int check_max_pixel(u32* pixels, int width, int height, int x, int y);
int check_max_pixel_large(u64* rg, u64* ba, int w, int h, int x, int y);



// NPR
void ordered_dithering(Canvas* s, u8* d, u32 dim);
void grey_scale_ordered_dithering(Canvas* s, u8* d, u32 dim);
void floyd_stienberg(Canvas* s, u8* d, u32 length);
void grey_scale_floyd_stienberg(Canvas* s, u8* d, u32 length);
void line_floyd_steinberg(Canvas* s, u8 * d, u32 length, i32 x_line_offset, i32 y_line_offset, u32 threshold);

// defaults NPR
void default_ordered_dithering(Canvas* s);
void default_grey_scale_ordered_dithering(Canvas* s);
void default_floyd_steinberg(Canvas* s);
void default_grey_scale_floyd_steinberg(Canvas* s);
void default_line_floyd_steinberg(Canvas* s, u32 threshold);
#endif
