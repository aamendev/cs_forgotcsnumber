#include "processing_tests.h"
#include "processing.h"
#include <limits.h>

#define KER_RAD (5)
void processing_all_tests(Canvas *c)
{
//    p_hatching_test(c);
 //   p_morpho_tests(c);
 p_quantize_tests(c);
}

void p_hatching_test(Canvas* c)
{
    read_to_canvas(c, "./assets/levi.jpg");
    default_line_floyd_steinberg(c, 150);
    save_to_img(c, JPG, "./imgs/npr/line_hatch");

    fill(c, BLACK);
    fill_circle(c, c->width / 2, c->height/2, 200, RED);
    default_line_floyd_steinberg(c, 120);
    save_to_img(c, JPG, "./imgs/npr/line_hatch_2");
    
}

void p_morpho_tests(Canvas* c)
{
    i32 dilate_kernel[KER_RAD * KER_RAD] = {
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1,1,  2, 1, 1, 
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
    };

    i32 erode_kernel[KER_RAD * KER_RAD];

    for (int i = 0; i < KER_RAD * KER_RAD; i++)
    {
        erode_kernel[i] = -dilate_kernel[i];
    }

    (void)erode_kernel;
    //dilate(c, dilate_kernel, KER_RAD);
   // erode(c, erode_kernel, KER_RAD);
   //grey_scale(c);
    read_to_canvas(c, "./assets/levi.jpg");
    dilate(c, dilate_kernel, KER_RAD);
    save_to_img(c, JPG, "./imgs/filters/dilate");

    read_to_canvas(c, "./assets/levi.jpg");
    erode(c, erode_kernel, KER_RAD);
    save_to_img(c, JPG, "./imgs/filters/erode");
}

void p_quantize_tests(Canvas* c)
{
    /*
    read_to_canvas(c, "./assets/levi.jpg");
    //grey_scale(c);
    populisity_quantize_colours(c, 6, 1e4);
    save_to_img(c, JPG, "./imgs/processing/quantized");

    read_to_canvas(c, "./assets/levi.jpg");
    median_cut_quantize_colours(c, 6, 1e4);
    save_to_img(c, JPG, "./imgs/processing/quantized_median");
    */

    read_to_canvas(c, "./assets/hunter_chibi");
    //median_cut_quantize_colours(c, 200, 1e4);
    populisity_quantize_colours(c, 200, 1e4, NULL);
    save_to_img(c, JPG, "./imgs/processing/quantized_median_2");
}
void p_ascii_test(Canvas *c)
{
    read_to_canvas(c, "assets/hunter_chibi");
    unsigned char chars[10] = 
    {' ',
        '.',
        ':',
        ';',
        'o',
        '=',
        '+',
        '*',
        '#',
        '@'};
    simulate_acii_edges(c, chars, 10, 3, "./imgs/processing/ascii.txt");
}

