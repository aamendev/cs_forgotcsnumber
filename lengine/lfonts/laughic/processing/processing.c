#include "processing.h"
#include "processing_utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>

u32 factorial(u32 v)
{
    u32 curr = 1;
    for (u32 i = 2; i < v; i++)
    {
        curr *= i;
    }
    return curr * v * (v>0) + !(v>0);
}
void translate(Canvas* c, i32 valx, i32 valy)
{
    u32* copy = malloc(c->width * c->height * sizeof(u32));

    for (int i = 0; i < c->width * c->height; i++)
    {
        copy[i] = c->pixels[i];
    }

    int newx, newy;
    for (int i = 0; i < c->width * c->height; i++)
    {
        newx = (i % c->width - valx);
        newx = (c->width + newx) * (newx < 0) + (newx - (int)c->width) * (newx >= (int)c->width)
            + newx * (newx > 0 && newx < (int)c->width);
        newy = i/c->width - valy;
        newy = (c->height + newy) * (newy < 0) + (newy - c->height) * (newy >= (i32)c->height)
            + newy * (newy > 0 && newy < (i32)c->height);

        c->pixels[i] = copy[newy * c->width + newx];
    }
}

void gaussian_filter(Canvas* s, float sigma)
{
    int rad =  (int)(sigma + 0.5f) * 3;
    if (rad == 0) return;
    i32* gauss = malloc((2 * rad + 1) * sizeof(u32));
    int scale = 1u<<16;
    for (int i = 0; i < 2*rad + 1; i++)
    {
        gauss[i] = pow(M_E, -0.5 *  (float)((i - rad) * (i-rad)) / (sigma * sigma)) * scale;
    }
    general_linear_separated_filter(s, gauss, 2*rad + 1, gauss, 2*rad + 1);
    //free(gauss);
}
void diff_of_gaussian(Canvas* s, float s1, float s2, SignedCanvas* sc)
{
    u32* pixels = malloc(sizeof(u32) * s->width * s->height);
    for (int i = 0; i < s->width * s->height; i++)
    {
        pixels[i] = s->pixels[i];
    }
    Canvas copy_canvas = 
    { s->default_colour, "default",
        pixels,
        s->width,
        s->height
    };
    gaussian_filter(s, s1);
    gaussian_filter(&copy_canvas, s2);
    i16 comps[4] = {0, 0, 0, 0};
    for (int i = 0; i < s->width * s->height; i++)
    {
        i16 r = 
            (int)get_channel(&s->pixels[i], ChannelRed)
            - 
            (int)get_channel(&pixels[i], ChannelRed);
        i16 g = 
            
            (int)get_channel(&s->pixels[i], ChannelGreen)
            - 
            (int)get_channel(&pixels[i], ChannelGreen);
        i16 b = 
            (int)get_channel(&s->pixels[i], ChannelBlue)
            - 
            (int)get_channel(&pixels[i], ChannelBlue);
        sc->pixels[i] = 0;
        comps[0] = r;
        comps[1] = g;
        comps[2] = b;
        comps[3] = 255;
        signed_pack(comps, &sc->pixels[i]);
    }
    free(pixels);
}
void simulate_acii_edges(Canvas* s, unsigned char* chars, u32 char_count, u32 char_pixel_size, char* path)
{
    int brightness_count = 2;
    u32 new_width = s->width/char_pixel_size;
    u32 new_height = s->height/(2*char_pixel_size);
    u32* pixels = malloc(sizeof(u32) * new_width * new_height);
    u32* pixels2 = malloc(sizeof(u32) * new_width * new_height);
    i64* edge_pixels = malloc(sizeof(i64) * new_width * new_height);
    float* grads = malloc(sizeof(float) * new_width * new_height);
    Canvas copy_canvas = 
    { BG, "default",
        pixels,
        new_width,
        new_height
    };
    SignedCanvas edge_canvas = 
    { "default",
        edge_pixels,
        new_width,
        new_height
    };
    Canvas edge_canvas_unsigned = 
    { 
        BG,
        "default",
        pixels2,
        new_width,
        new_height
    };
    downscale_canvas(s, &copy_canvas);

    grey_scale(&copy_canvas);
    mult_contrast(&copy_canvas, 0.95);
    sharpen(&copy_canvas, 0.15);

    for (int i = 0; i < new_height * new_width; i++)
    {
        u8 r = get_channel(&copy_canvas.pixels[i], ChannelRed);
        int idx = ((float)r/255 * (char_count) + 0.5);
        r = idx * (255 / (char_count));
        set_channel(&copy_canvas.pixels[i],r, ChannelRed);
        set_channel(&copy_canvas.pixels[i], r,ChannelGreen);
        set_channel(&copy_canvas.pixels[i], r,ChannelBlue);
        edge_canvas_unsigned.pixels[i] = copy_canvas.pixels[i];
    }
    diff_of_gaussian(&edge_canvas_unsigned,1., 8., &edge_canvas);
    sobel_filter_with_grads_signed(&edge_canvas, grads);
    for (int i = 0; i < new_width * new_height; i++)
    {
        signed_to_unsigned(&edge_canvas.pixels[i], &edge_canvas_unsigned.pixels[i]);
    }
    
    u8 max_r = 0;
    for (int i = 0; i < new_height * new_width; i++)
    {
        u8 r = get_channel(&edge_canvas_unsigned.pixels[i], ChannelRed);
        max_r = max_r * (max_r > r) + r * !(max_r > r);
    }

    unsigned char line_vals[9] = {
        '|', '/',
        151, '\\', 
        '|', '/', 
        151, '\\', 
        '|'};;
    FILE* fptr;
    fptr=  fopen(path, "w");
    unsigned char c;
    if (fptr == NULL) return;
    for (int j = 0; j < new_height; j++)
    {
        for (int i = 0; i < new_width; i++)
        {
            u32 col = edge_canvas_unsigned.pixels[j * new_width + i];
            u8 r = get_channel(&col, ChannelRed);
            u8 r2 = get_channel(&copy_canvas.pixels[j * new_width + i], ChannelRed);
            if (r > 0.3 * max_r)
            {
                int idx = 9 * (0.5f * ((grads[j * new_width + i] + M_PI)) / M_PI);
                idx = idx * (idx != 9) + 8 * (idx == 9);
                c = line_vals[idx];
                if (c == 151)
                {
                    fputc(0xE2, fptr);
                    fputc(0x80, fptr);
                    fputc(0x94, fptr);
                }
                else 
                {
                    fputc(line_vals[idx], fptr);
                }
            }
            else 
            {
                int idx = char_count * (float)r2/255.;
                idx = idx * (idx != char_count) + (char_count-1) * (idx == char_count);
                fputc(chars[idx], fptr);
            }
        }
            fputc('\n', fptr);
    }
    fclose(fptr);
    free(pixels);
}
void populisity_quantize_colours(Canvas* c, u32 new_count, u32 expected_old_count, u32* new_cols)
{
    expected_old_count += fmax((new_count * 2), 100) * (expected_old_count == 0);
    ColourHistogramBar* cols = malloc(expected_old_count * sizeof(ColourHistogramBar));
    u32 old_count = get_cols_histogram(c, cols, expected_old_count);
    if (new_count > expected_old_count)
    {
        return;
    }
    sort_bars(cols, old_count);
    if (new_cols == NULL)
    {
        new_cols = malloc(new_count * sizeof(u32));
    }
    for (int i = 0; i < new_count; i++)
    {
        new_cols[i] = cols[i].col;
    }

    for (int i = 0; i < c->width * c->height; i++)
    {
        f32 current_min = FLT_MAX;
        u32 current_col = 0;
        f32 current_dist = FLT_MAX;
        for (int j = 0; j < new_count; j++)
        {
            current_dist = colour_distance(c->pixels[i], new_cols[j]);
           if ( current_dist < current_min )
           {
               current_min = current_dist;
               current_col = new_cols[j];
           }
        }
        c->pixels[i] = current_col;
    }
    free(new_cols);
    free(cols);
}
// Not Done Yet
void median_cut_quantize_colours(Canvas* c, u32 new_count, u32 expected_old_count)
{
    expected_old_count += fmax((new_count * 2), 100) * (expected_old_count == 0);
    ColourHistogramBar* cols = malloc(expected_old_count * sizeof(ColourHistogramBar));
    u32 old_count = get_cols_histogram(c, cols, expected_old_count);
    if (new_count > expected_old_count)
    {
        return;
    }
 
    //sort_bars(cols, old_count);
    int box_count = 0;
    ColourBox root = createColourBox(cols, old_count, 0);
    box_count++;

    while (box_count < new_count)
    {
        ColourBox* split = find_box_to_split(&root);
        if (split == NULL) break;
        box_count++;
    }
    ColourBox* current = &root;
    u32* new_cols = malloc(new_count * sizeof(u32));
    int colidx = 0;
    while (current != NULL)
    {
        u32 avg_red= 0;
        u32 avg_green = 0;
        u32 avg_blue = 0;
        u32 avg_col = 0;
        for (int i = 0; i < current->col_count; i++)
        {
            avg_red += get_channel(&current->cols[i].col, ChannelRed);
            avg_green += get_channel(&current->cols[i].col, ChannelGreen);
            avg_blue += get_channel(&current->cols[i].col, ChannelBlue);
        }
        avg_red /= current->col_count;
        avg_blue /= current->col_count;
        avg_green /= current->col_count;
        set_channel(&avg_col, (u8)avg_red, ChannelRed); 
        set_channel(&avg_col, (u8)avg_green, ChannelGreen); 
        set_channel(&avg_col, (u8)avg_blue, ChannelBlue); 
        set_channel(&avg_col, 0xff, ChannelAlpha);
        new_cols[colidx++] = avg_col;
        current = current->next;
    }
    for (int i = 0; i < c->width * c->height; i++)
    {
        f32 current_min = FLT_MAX;
        u32 current_col = 0;
        f32 current_dist = FLT_MAX;
        for (int j = 0; j < new_count; j++)
        {
            current_dist = colour_distance(c->pixels[i], new_cols[j]);
           if ( current_dist < current_min )
           {
               current_min = current_dist;
               current_col = new_cols[j];
           }
        }
        c->pixels[i] = current_col;
    }
    current = &root;
    /*ColourBox* next = current->next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }*/
    free(cols);
    free(new_cols);
}

void sort_bars(ColourHistogramBar* bars, u32 bars_count)
{
    for (int i = 0; i < bars_count; i++)
    {
        for (int j = 0; j < bars_count - 1; j++)
        {
            if (bars[j].count <= bars[j+1].count)
            {
                ColourHistogramBar temp = bars[j];
                bars[j] = bars[j + 1];
                bars[j + 1] = temp;
            }
        }
    }
}
void split_box(ColourBox* root, ColourBox* to_split)
{
    u8 r_range = 
        get_channel(&to_split->maxCol, ChannelRed) - 
        get_channel(&to_split->minCol, ChannelRed);
    u8 b_range = 
        get_channel(&to_split->maxCol, ChannelBlue) - 
        get_channel(&to_split->minCol, ChannelBlue);
    u8 g_range = 
        get_channel(&to_split->maxCol, ChannelGreen) - 
        get_channel(&to_split->minCol, ChannelGreen);
    u8 max_range = fmax(r_range, fmax(g_range, b_range));

    Channel split_channel;
    if (max_range == r_range)
    {
        split_channel = ChannelRed;
    }
    else if (max_range == b_range)
    {
        split_channel = ChannelBlue;
    }
    else 
    {
        split_channel = ChannelGreen;
    }

    sort_bars(to_split->cols, to_split->col_count);
    ColourHistogramBar* c1_bar = malloc(to_split->col_count * sizeof(ColourHistogramBar));
    ColourHistogramBar* c2_bar = malloc(to_split->col_count * sizeof(ColourHistogramBar));
    
    int c1idx = 0;
    int c2idx = 0;
    for (int i = 0; i< to_split->col_count; i++)
    {
        if (get_channel(&to_split->cols[to_split->col_count/2].col, split_channel)
                <= get_channel(&to_split->cols[i].col, split_channel))
        {
            c1_bar[c1idx++] = to_split->cols[i];
        }
        else 
        {
            c2_bar[c2idx++] = to_split->cols[i];
        }
    }
    c1_bar = realloc(c1_bar, c1idx);
    c2_bar = realloc(c2_bar, c2idx);
    ColourBox* box1;
    ColourBox* box2 = malloc(sizeof(ColourBox)); 
    *box2 = createColourBox(c2_bar, c2idx, to_split->level+1);
    if (to_split->prev != NULL)
    {
        box1 = malloc(sizeof(ColourBox));
        *box1 = createColourBox(c1_bar, c1idx, to_split->level+1);
        to_split->prev->next = box1;
        free(to_split->cols);
        free(to_split);
    }
    else 
    {
        box1 = root;
    }
    box1->next = box2;
    box2->prev = box1;
}
ColourBox* find_box_to_split(ColourBox* root)
{
    ColourBox* current = root;
    ColourBox* split = NULL;

    while (current != NULL && current->col_count <= 1)
    {
        current = current->next;
    }

    split = current;
    while (current != NULL)
    {
        if (current->col_count > 1 && current->level < split->level)
        {
                split = current;
        }
        current = current->next;
    }
    return split;
}
ColourBox createColourBox(ColourHistogramBar* bars, u32 bars_count, u32 level)
{
    ColourBox box;
    box.cols = bars;
    box.col_count = bars_count;
    box.level = level;

    u8 minr = 0, ming = 0, minb = 0;
    u8 maxr = 0xff, maxg = 0xff, maxb = 0xff;
    for (int i = 0; i < bars_count; i++)
    {
        minr = fmin(minr, get_channel(&bars[i].col, ChannelRed));
        ming = fmin(ming, get_channel(&bars[i].col, ChannelGreen));
        minb = fmin(minb, get_channel(&bars[i].col, ChannelBlue));
        maxr = fmax(maxr, get_channel(&bars[i].col, ChannelRed));
        maxg = fmax(maxg, get_channel(&bars[i].col, ChannelGreen));
        maxb = fmax(maxb, get_channel(&bars[i].col, ChannelBlue));
    }
    set_channel(&box.minCol, minr, ChannelRed);
    set_channel(&box.minCol, ming, ChannelGreen);
    set_channel(&box.minCol, minb, ChannelBlue);
    set_channel(&box.maxCol, maxr, ChannelRed);
    set_channel(&box.maxCol, maxg, ChannelGreen);
    set_channel(&box.maxCol, maxb, ChannelBlue);
    box.next = NULL;
    box.prev = NULL;
    return box;
}
void weighted_subtract_img(Canvas* c1, u32* c2, float w)
{
    for (int i = 0; i < c1->width * c1->height; i++)
    {
        u8 channels[4];
        unpack(channels, &c1->pixels[i]);
        u8 channels2[4];
        unpack(channels2, &c2[i]);
        for (int j = 0; j < 3; j++)
        {
            channels[j] = (channels[j] - w * (float)channels2[j])
                * ((float)channels[j] > w * (float) channels2[j]);
        }
        c1->pixels[i] = 0;
        pack(channels, &c1->pixels[i]);
    }
}

void weighted_add_img(Canvas* c1, u32* c2, float w)
{
    for (int i = 0; i < c1->width * c1->height; i++)
    {
        u8 channels[4];
        unpack(channels, &c1->pixels[i]);
        u8 channels2[4];
        unpack(channels2, &c2[i]);
        for (int j = 0; j < 3; j++)
        {
            channels[j] = (channels[j] + w * (float)channels2[j])
                * ((float)channels[j] + w * (float) channels2[j] < 255)
            + 255 * !((float)channels[j] + w * (float) channels2[j] < 255);
        }
        c1->pixels[i] = 0;
        pack(channels, &c1->pixels[i]);
    }
}
void mult_contrast(Canvas* c, float val)
{
    if (val > 255.0f) return;
    for (int i = 0; i < c->width * c->height; i++)
    {
        u8 channels[4];
        unpack(channels, &c->pixels[i]);
        c->pixels[i] = 0;
        for (int j = 0; j < 3; j++)
        {
            channels[j] =  channels[j] * val * (((u16)channels[j] * val) < 256)
                + 255 * !(((u16)channels[j] * val) < 256);
        }
        pack(channels, & c->pixels[i]);
    }
}

void inc_brightness(Canvas* c, i16 val)
{
    if (val * val > 65025) return;
    for (int i = 0; i < c->width * c->height; i++)
    {
        u8 channels[4];
        unpack(channels, &c->pixels[i]);
        c->pixels[i] = 0;
        for (int j = 0; j < 3; j++)
        {
            channels[j] =  
                (channels[j] + val) * ((u16)((u16)channels[j] + val) < 256)
                + 255 * !(((u16)((u16)channels[j] + val) < 256) || val < 0);
        }
        pack(channels, & c->pixels[i]);
    }
}

void inc_channel(Canvas* c, Channel channel, i16 val)
{
    if (val * val > 65025) return;
    for (int i = 0; i < c->width * c->height; i++)
    {
        u8 channels[4];
        unpack(channels, &c->pixels[i]);
        c->pixels[i] = 0;
        channels[channel] =  
            (channels[channel] + val) * ((u16)((u16)channels[channel] + val) < 256)
            + 255 * !((u16)((u16)channels[channel] + val) < 256 || val < 0);
        pack(channels, & c->pixels[i]);
    }
}


void invert(Canvas* c)
{
    for (int i = 0; i < c->width * c->height; i++)
    {
        c->pixels[i] = complement(c->pixels[i]);
    }
}

void filter(Canvas* c, Channel channel)
{
    filter_threshold(c, channel, 0);
}

void filter_threshold(Canvas* c, Channel channel, u8 t)
{
    for (int i = 0; i < c->width * c->height; i++)
    {
        u8 comps[4];
        unpack(comps, &c->pixels[i]);
        c->pixels[i] = c->default_colour * (comps[channel] > t) + 
            c->pixels[i] * !(comps[channel] > t);
    }
}

void clamp_channel(Canvas* c, Channel channel, u8 t0, u8 t1)
{
    for (int i = 0; i < c->width * c->height; i++)
    {
        u8 comps[4];
        unpack(comps, &c->pixels[i]);
        comps[channel] = 
            comps[channel] * (comps[channel] >= t0 && comps[channel] <= t1)
            + t0 * (comps[channel] < t0)
            + t1 * (comps[channel] > t1);
        c->pixels[i] = 0;
        pack(comps, &c->pixels[i]);
    }
}
void clamp_all(Canvas* c, u8 t0, u8 t1)
{
    for (int i = 0; i < c->width * c->height; i++)
    {
        u8 comps[4];
        unpack(comps, &c->pixels[i]);
        for (int j = 0; j < 3; j++)
        {
            comps[j] = 
                comps[j] * (comps[j] >= t0 && comps[j] <= t1)
                + t0 * (comps[j] < t0)
                + t1 * (comps[j] > t1);
        }
        c->pixels[i] = 0;
        pack(comps, &c->pixels[i]);
    }
}
void grey_scale(Canvas *c)
{
    for (int i = 0; i < c->width * c->height; i++)
    {
        u8 comps[4];
        unpack(comps, &c->pixels[i]);
        u16 val = 0.299f * comps[0] + 0.587 * comps[1] + (0.144 * comps[2]);
        comps[0] = fmin(255, val);
        comps[1] = comps[0];
        comps[2] = comps[0];
        c->pixels[i] = 0;
        pack(comps, &c->pixels[i]);
    }
}

void general_2d_generic_operator_filter(Canvas* s, i32* h1, u32 s1, u32 val_init,
        i32 normalize_factor,
        void (*op)(u32* a, u32 b, i32* h1, u32 s1))
{
    u32 oldCanvas[s->width*s->height];
    i32 c1 = s1/2;
    u32 curr_cell = 0;
    f32 norm1 = 0;
    u8 comps[4];
    u32 vals[4] = {0, 0, 0, 0};
    int add1 = 0;
    int add2 = 0;

    if (normalize_factor == 0)
    {
    for (int i = 0; i < s1; i++)
    {
        norm1 += abs(h1[i]);
    }
    norm1 = 1.0f/norm1;
    }
    else {
    norm1 = normalize_factor;
    }

    for (int i = 0; i < s->width * s->height; i++)
    {
        oldCanvas[i] = s->pixels[i];
    }
    for (int j = 0; j < s->height; j++)
    {
        for (int i = 0; i < s->width; i++)
        {
            for (int v = 0; v < 4; v++)
            {
                vals[v] = val_init;
            }
            for (int u = -c1; u < c1 + (s1 % 2 == 1); u++)
            {
                add1 = u * (i + u > -1 && i+u < s->width);
                for (int v = -c1; v< c1 + (s1 %2 == 1); v++)
                {
                add2 = v * (j + v > -1 && j+v < s->height);

                curr_cell = oldCanvas[(j + add2) * (s->width) + i + add1];
                unpack(comps, &curr_cell);
                for (int k = 0; k < 4; k++)
                {
                    op(&vals[k], h1[(c1 + v) * s1 + c1 + u] * comps[k], h1, s1);
                }
                }
            }
            for (int v = 0; v < 4; v++)
            {
                vals[v] *= norm1;
                comps[v] = vals[v];
            }
            s->pixels[j * (s->width) + i] = 0;
            pack(comps, &(s->pixels[j * (s->width) + i]));
        }
    }
}

void s_general_2d_generic_operator_filter(
i64* s, u32 width, u32 height, i32* h1, u32 s1, 
        i64 val_init, i32 normalize_factor, void (*op)(i64* a, i64 b, i32* h1, u32 s1)
        )
{
    i64* oldCanvas = malloc(sizeof(i64) * width * height);
    i32 c1 = s1/2;
    i64 curr_cell = 0;
    f32 norm1 = 0;
    i16 comps[4];
    i64 vals[4] = {0, 0, 0, 0};
    int add1 = 0;
    int add2 = 0;

    if (normalize_factor == 0)
    {
    for (int i = 0; i < s1; i++)
    {
        norm1 += abs(h1[i]);
    }
    norm1 = 1.0f/norm1;
    }
    else {
    norm1 = normalize_factor;
    }

    for (int i = 0; i < width * height; i++)
    {
        oldCanvas[i] = s[i];
    }
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            for (int v = 0; v < 4; v++)
            {
                vals[v] = val_init;
            }
            for (int u = -c1; u < c1 + (s1 % 2 == 1); u++)
            {
                add1 = u * (i + u > -1 && i+u < width);
                for (int v = -c1; v< c1 + (s1 %2 == 1); v++)
                {
                add2 = v * (j + v > -1 && j+v < height);

                curr_cell = oldCanvas[(j + add2) * (width) + i + add1];
                signed_unpack(comps, &curr_cell);
                for (int k = 0; k < 4; k++)
                {
                    op(&vals[k], h1[(c1 + v) * s1 + c1 + u] + comps[k], h1, 
                            (c1 + v) * s1 + c1 + u);
                }
                }
            }
            for (int v = 0; v < 4; v++)
            {
                vals[v] *= norm1;
                comps[v] = vals[v];
            }
            s[j * (width) + i] = 0;
            signed_pack(comps, &(s[j * (width) + i]));
        }
    }
}

void jitter_1d_filter(Canvas* s, u32 s1, FilterDirection fd)
{
    u32 oldCanvas[s->width*s->height];
    i32 luck = 0;
    i32 c1 = s1/2;
    u32 curr_cell = 0;
    u8 comps[4];

    for (int i = 0; i < s->width * s->height; i++)
    {
        oldCanvas[i] = s->pixels[i];
    }
    for (int j = 0; j < s->height; j++)
    {
        for (int i = 0; i < s->width; i++)
        {
                luck = rand_int_bound(0, s1) - c1;
                int add1 = (fd == FilterX) * luck * (i + luck > -1 && i+luck < s->width);
                int add2 = (fd == FilterY) * luck * (j + luck > -1 && j+luck < s->height);
                curr_cell = oldCanvas[(j + add2) * (s->width) + i + add1];
                unpack(comps, &curr_cell);
                s->pixels[j * (s->width) + i] = 0;
                pack(comps, &(s->pixels[j * (s->width) + i]));
        }
    }
}

void jitter_2d_filter(Canvas* s, u32 s1)
{
    u32 oldCanvas[s->width*s->height];
    i32 luckx = 0;
    i32 lucky = 0;
    i32 c1 = s1/2;
    u32 curr_cell = 0;
    u8 comps[4];

    for (int i = 0; i < s->width * s->height; i++)
    {
        oldCanvas[i] = s->pixels[i];
    }
    for (int j = 0; j < s->height; j++)
    {
        for (int i = 0; i < s->width; i++)
        {
                luckx = rand_int_bound(0, s1 * s1);
                lucky = luckx / s1 - c1;
                luckx %= s1;
                luckx -= c1;
                int add1 =  luckx * (i + luckx > -1 && i+luckx < s->width);
                int add2 =  lucky * (j + lucky > -1 && j+lucky < s->height);
                curr_cell = oldCanvas[(j + add2) * (s->width) + i + add1];
                unpack(comps, &curr_cell);
                s->pixels[j * (s->width) + i] = 0;
                pack(comps, &(s->pixels[j * (s->width) + i]));
        }
    }
}

void signed_general_linear_1d_filter(i64* s, u32 w, u32 h, i32* h1, u32 s1, 
        FilterDirection fd)
{
    i64* oldCanvas = malloc(w * h * sizeof(i64));
    i32 c1 = s1/2;
    i64 curr_cell = 0;
    f32 norm1 = 0;
    i16 vals[4] = {0, 0, 0, 0};
    i16 comps[4] = {0, 0, 0, 0};

    for (int i = 0; i < s1; i++)
    {
        norm1 += abs(h1[i]);
    }
    norm1 = 1.0f/norm1;

    for (int i = 0; i < w * h; i++)
    {
        oldCanvas[i] = s[i];
    }
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            for (int v = 0; v < 3; v++)
            {
                vals[v] = 0;
            }
            for (int u = -c1; u < c1 + (s1 % 2 == 1); u++)
            {
                int add1 = (fd == FilterX) * u * (i + u > -1 && i+u < w);
                int add2 = (fd == FilterY) * u * (j + u > -1 && j+u < h);
                curr_cell = oldCanvas[(j + add2) * (w) + i + add1];

                signed_unpack(comps, &curr_cell);
                for (int v = 0; v < 3; v++)
                {
                    vals[v] += (h1[c1 +u] * comps[v]);
                }
                vals[3] = comps[3];
            }
            s[j * (w) + i] = 0;
            for (int v = 0; v < 3; v++)
            {
                vals[v] *= norm1;
            }
            signed_pack(vals, &s[j * w + i]);
        }
    }
    free(oldCanvas);
}

void s_general_linear_separated_filter(i64* s, u32 w, u32 h, i32* h1, u32 s1, i32* h2, u32 s2)
{
   signed_general_linear_1d_filter(s, w, h, h1, s1, FilterX);
   signed_general_linear_1d_filter(s, w, h, h2, s2, FilterY);
}

void long_linear_separated_filter(LargeCanvas* s, i32* h1, u32 s1, i32* h2, u32 s2)
{
    long_general_linear_1d_filter(s, h1, s1, FilterX);
    long_general_linear_1d_filter(s, h2, s2, FilterY);
}


void general_linear_1d_filter(Canvas* s, i32* h1, u32 s1, FilterDirection fd)
{
    u32* oldCanvas = malloc(s->width * s->height * sizeof(u32));
    i32 c1 = s1/2;
    u32 curr_cell = 0;
    f32 norm1 = 0;
    u8 comps[4];
    i32 vals[4] = {0, 0, 0, 0};

    for (int i = 0; i < s1; i++)
    {
        norm1 += abs(h1[i]);
    }
    norm1 = 1.0f/norm1;


    for (int i = 0; i < s->width * s->height; i++)
    {
        oldCanvas[i] = s->pixels[i];
    }
    for (int j = 0; j < s->height; j++)
    {
        for (int i = 0; i < s->width; i++)
        {
            for (int v = 0; v < 3; v++)
            {
                vals[v] = 0;
            }
            for (int u = -c1; u < c1 + (s1 % 2 == 1); u++)
            {
                int add1 = (fd == FilterX) * u * (i + u > -1 && i+u < s->width);
                int add2 = (fd == FilterY) * u * (j + u > -1 && j+u < s->height);
                curr_cell = oldCanvas[(j + add2) * (s->width) + i + add1];

                unpack(comps, &curr_cell);
                for (int v = 0; v < 3; v++)
                {
                    vals[v] += (norm1 * (h1[c1 +u] * (i16)comps[v]));
                }
            }
            for (int v = 0; v < 3; v++)
            {
                comps[v] = vals[v] * (vals[v] > 0);
            }
            s->pixels[j * (s->width) + i] = 0;
            pack(comps, &(s->pixels[j * (s->width) + i]));
        }
    }
    free(oldCanvas);
}
void long_general_linear_1d_filter(LargeCanvas* s, i32* h1, u32 s1, FilterDirection fd)
{
    u64* oldCanvas_rg = malloc(s->width * s->height * sizeof(u64));
    u64* oldCanvas_ba = malloc(s->width * s->height * sizeof(u64));
    i32 c1 = s1/2;
    u64 curr_cell_rg = 0;
    u64 curr_cell_ba = 0;
    f32 norm1 = 0;
    u32 comps[4];
    i16 vals[4] = {0, 0, 0, 0};

    for (int i = 0; i < s1; i++)
    {
        norm1 += abs(h1[i]);
    }
    norm1 = 1.0f/norm1;


    for (int i = 0; i < s->width * s->height; i++)
    {
        oldCanvas_rg[i] = s->rg[i];
        oldCanvas_ba[i] = s->ba[i];
    }
    for (int j = 0; j < s->height; j++)
    {
        for (int i = 0; i < s->width; i++)
        {
            for (int v = 0; v < 3; v++)
            {
                vals[v] = 0;
            }
            for (int u = -c1; u < c1 + (s1 % 2 == 1); u++)
            {
                int add1 = (fd == FilterX) * u * (i + u > -1 && i+u < s->width);
                int add2 = (fd == FilterY) * u * (j + u > -1 && j+u < s->height);
                curr_cell_rg = oldCanvas_rg[(j + add2) * (s->width) + i + add1];
                curr_cell_ba = oldCanvas_ba[(j + add2) * (s->width) + i + add1];

                long_unpack(comps, &curr_cell_rg, &curr_cell_ba);
                for (int v = 0; v < 3; v++)
                {
                    vals[v] += (h1[c1 +u] * (i16)comps[v]);
                }
            }
            for (int v = 0; v < 3; v++)
            {
                vals[v] *= norm1;
                comps[v] = vals[v] * (vals[v] > 0);
            }
            s->rg[j * (s->width) + i] = 0;
            s->ba[j * (s->width) + i] = 0;
            long_pack(comps, &(s->rg[j * (s->width) + i]), &(s->ba[j * (s->width) + i]));
        }
    }
    free(oldCanvas_rg);
    free(oldCanvas_ba);
}

void general_linear_separated_filter(Canvas* s, i32* h1, u32 s1, i32* h2, u32 s2)
{
   general_linear_1d_filter(s, h1, s1, FilterX);
   general_linear_1d_filter(s, h2, s2, FilterY);
}

void box_filter(Canvas *s, u32 dim)
{
    i32* box = malloc(dim * sizeof(u32));
    for (int i = 0; i < dim; i++)
    {
        box[i] = 1;
    }
    general_linear_separated_filter(s, box, dim, box, dim);
    free(box);
}
// f = 0.219482


void binomial_filter(Canvas* s, u32 rad)
{
    i32* bin = malloc((2 * rad + 1) * sizeof(u32));
    for (int i = 0; i < 2*rad + 1; i++)
    {
          bin[i] = factorial(2 * rad) / (factorial(i) * factorial(2*rad - i));
    }
    general_linear_separated_filter(s, bin, 2*rad + 1, bin, 2*rad + 1);
    //free(gauss);
}

void s_binomial_filter(i64* s, u32 w, u32 h , u32 rad)
{
    i32* bin = malloc((2 * rad + 1) * sizeof(u32));
    for (int i = 0; i < 2*rad + 1; i++)
    {
          bin[i] = factorial(2 * rad) / (factorial(i) * factorial(2*rad - i));
          //printf("%d\n", bin[i]);
    }
    s_general_linear_separated_filter(s, w, h, bin, 2*rad + 1, bin, 2*rad + 1);
    //free(gauss);
}

void long_binomial_filter(LargeCanvas* s, u32 rad)
{
    i32* bin = malloc((2 * rad + 1) * sizeof(u32));
    for (int i = 0; i < 2*rad + 1; i++)
    {
          bin[i] = factorial(2 * rad) / (factorial(i) * factorial(2*rad - i));
          //printf("%d\n", bin[i]);
    }
    long_linear_separated_filter(s, bin, 2*rad + 1, bin, 2*rad + 1);
    //free(gauss);
}

void min_filter(Canvas *s, u32 dim)
{
    i32* h = malloc(dim * dim * sizeof(u32));
    for (int k = 0; k < dim*dim; k++)
    {
        h[k] = 1;
    }
    general_2d_generic_operator_filter(s, h, dim, UINT32_MAX, 1, p_generic_inplace_min);
    free(h);
}
void max_filter(Canvas *s, u32 dim)
{
    i32* h = malloc(dim * dim * sizeof(u32));
    for (int k = 0; k < dim*dim; k++)
    {
        h[k] = 1;
    }
    general_2d_generic_operator_filter(s, h, dim, 0, 1, p_generic_inplace_max);
    free(h);
}

void jitter_filter(Canvas* s, u32 dim)
{
    jitter_1d_filter(s, dim, FilterX);
    jitter_1d_filter(s, dim, FilterY);
}


void grey_scale_ordered_dithering(Canvas* s, u8* d, u32 dim)
{
    u32 oldCanvas[s->width*s->height];
    u32 curr_cell = 0;
    u8 comps[4];
    f32 intensity;

    for (int i = 0; i < s->width * s->height; i++)
    {
        oldCanvas[i] = s->pixels[i];
    }
    for (int j = 0; j < s->height - dim; j += dim)
    {
        for (int i = 0; i < s->width - dim; i += dim)
        {
            for (int v = 0; v < dim; v++)
            {
                for (int u = 0; u < dim; u++)
                {
                    curr_cell = oldCanvas[(j + v) * (s->width) + i + u];
                    unpack(comps, &curr_cell);
                    intensity = 
                        0.299f * comps[0] + 0.587 * comps[1] + (0.144 * comps[2]);
                        for (int k = 0; k < 3; k++)
                        {
                                comps[k] = 0xff * !(intensity < d[v * dim + u]);
                        }
                    comps[3] = 0xff;
                    s->pixels[(j + v) * (s->width) + (i + u)] = 0;
                    pack(comps, &(s->pixels[(j + v) * (s->width) + (i + u)]));
                }
            }
        }
    }
}
void ordered_dithering(Canvas* s, u8* d, u32 dim)
{
    u32 oldCanvas[s->width*s->height];
    u32 curr_cell = 0;
    u8 comps[4];

    for (int i = 0; i < s->width * s->height; i++)
    {
        oldCanvas[i] = s->pixels[i];
    }
    for (int j = 0; j < s->height - dim; j += dim)
    {
        for (int i = 0; i < s->width - dim; i += dim)
        {
            for (int v = 0; v < dim; v++)
            {
                for (int u = 0; u < dim; u++)
                {
                    curr_cell = oldCanvas[(j + v) * (s->width) + i + u];
                    unpack(comps, &curr_cell);
                    for (int k = 0; k < 3; k++)
                    {
                            comps[k] = 0xff * !(comps[k] < d[v * dim + u]);
                    }
                    comps[3] = 0xff;
                    s->pixels[(j + v) * (s->width) + (i + u)] = 0;
                    pack(comps, &(s->pixels[(j + v) * (s->width) + (i + u)]));
                }
            }
        }
    }
}
void default_ordered_dithering(Canvas* s)
{
    u32 dim = 5;
    u8* d = malloc(dim * dim * sizeof(u8));
    for (int i = 0; i < dim * dim; i++)
    {
        d[i] = i * 0xff / (dim * dim - 1);
    }
    ordered_dithering(s, d, dim);
    free(d);
}

void default_grey_scale_ordered_dithering(Canvas* s)
{
    u32 dim = 5;
    u8* d = malloc(dim * dim * sizeof(u8));
    for (int i = 0; i < dim * dim; i++)
    {
        d[i] = i * 0xff / (dim * dim - 1);
    }
    grey_scale_ordered_dithering(s, d, dim);
    free(d);
}

void grey_scale_floyd_stienberg(Canvas* s, u8* d,u32 length)
{
    grey_scale(s);
    floyd_stienberg(s, d, length);
}
void floyd_stienberg(Canvas* s, u8* d, u32 length)
{
    u32 curr_cell = 0;
    u8 comps[4];
    u8 neighbour_comps[4];
    f32 err_factors[4] = {7.0f/16, 3.f/16, 5.f/16, 1.f/16};
    i8 offsets[8] = {1, 0, -1, 1, 0, 1, 1, 1};
    i16 min_diff[3] = {0xff, 0xff, 0xff};
    u8 approx[3];
    i16 curr_diff;

    for (int j = 0; j < s->height - 1; j ++)
    {
        for (int i = 1; i < s->width - 1; i++)
        {
                    curr_cell = j * (s->width) + i;

                    unpack(comps, &s->pixels[curr_cell]);
                    for (int k = 0; k < 3; k++)
                    {
                        min_diff[k] = 0xff;
                        approx[k] = 0xff;
                        for (int l = 0; l < length; l++)
                        {
                            curr_diff = (i16)comps[k] - (i16)d[l];
                            approx[k] = d[l] 
                                * !(abs(curr_diff) > abs(min_diff[k])) + 
                                approx[k] 
                                * (abs(curr_diff) > abs(min_diff[k]));

                            min_diff[k] = curr_diff * 
                                !(abs(curr_diff) > abs(min_diff[k]))
                                + min_diff[k] * 
                                (abs(curr_diff) > abs(min_diff[k]));
                        }
                            comps[k] = approx[k];
                    }
                    comps[3] = 0xff;
                    s->pixels[curr_cell] = 0;
                    pack(comps, &(s->pixels[curr_cell]));
                    for (int l = 0; l < 8; l+=2)
                    {
                        unpack(neighbour_comps, 
                                &s->pixels[(j + offsets[l + 1])
                                * s->width + i + offsets[l]]);
                        for (int y = 0; y < 3; y++)
                        {
                            /*i16 test = (i16)neighbour_comps[y]
                                + err_factors[l/2] * min_diff[y];
                            if (test > 0)
                            {
                            printf("TEST: %d\n", test);
                            }
                            */
                            neighbour_comps[y] = fmax(0, fmin(0xff, 
                                    (i16)neighbour_comps[y] + 
                                    err_factors[l/2] * min_diff[y]));
                        }
                        neighbour_comps[3] = 0xff;
                        s->pixels[(j + offsets[l + 1])
                            * s->width + i + offsets[l]] = 0;
                        pack(neighbour_comps, 
                                &s->pixels[(j + offsets[l + 1])
                                * s->width + i + offsets[l]]);
                    }
        }
    }
}
void line_floyd_steinberg(Canvas* s, u8 * d, u32 length, 
        i32 x_line_offset, i32 y_line_offset,
        u32 threshold)
{
    u32 curr_cell = 0;
    u8 comps[4];
    u8 neighbour_comps[4];
    f32 err_factors[4] = {7.0f/16, 3.f/16, 5.f/16, 1.f/16};
    i8 offsets[8] = {1, 0, -1, 1, 0, 1, 1, 1};
    i16 min_diff[3] = {0xff, 0xff, 0xff};
    u8 approx[3];
    i16 curr_diff;

    for (int j = 0; j < s->height - 1; j ++)
    {
        for (int i = 1; i < s->width - 1; i++)
        {
                    curr_cell = j * (s->width) + i;

                    unpack(comps, &s->pixels[curr_cell]);
                    for (int k = 0; k < 3; k++)
                    {
                        min_diff[k] = 0xff;
                        approx[k] = 0xff;
                        for (int l = 0; l < length; l++)
                        {
                            curr_diff = (i16)comps[k] - (i16)d[l];
                            approx[k] = d[l] 
                                * !(abs(curr_diff) > abs(min_diff[k])) + 
                                approx[k] 
                                * (abs(curr_diff) > abs(min_diff[k]));

                            min_diff[k] = curr_diff * 
                                !(abs(curr_diff) > abs(min_diff[k]))
                                + min_diff[k] * 
                                (abs(curr_diff) > abs(min_diff[k]));
                        }
                            comps[k] = approx[k];
                    }
                    comps[3] = 0xff;
                    int draw_line = (approx[0] >= threshold);
                    if (draw_line)
                    {
                        line(s, i - x_line_offset / 2, j - y_line_offset / 2, 
                                i + x_line_offset / 2 , j + y_line_offset / 2, 
                                0xff000000);
                    for (int k = 0; k < 3; k++)
                    {
                        min_diff[k] += 
                            (fmax(abs(y_line_offset), abs(x_line_offset)) - 1)
                            * (min_diff[k] >= 0) + 
                            - (fmax(abs(y_line_offset), abs(x_line_offset))) * (min_diff[k] < 0);
                    }
                    }
                    else 
                    {
                        for (int k = 0; k < 3; k++)
                        {
                        approx[k] = 0;
                        min_diff[k] = comps[k];
                        }
                    }

                    for (int l = 0; l < 8; l+=2)
                    {
                        unpack(neighbour_comps, 
                                &s->pixels[(j + offsets[l + 1])
                                * s->width + i + offsets[l]]);
                        for (int y = 0; y < 3; y++)
                        {
                            neighbour_comps[y] = fmax(0, fmin(0xff, 
                                    (i16)neighbour_comps[y] + 
                                    err_factors[l/2] * min_diff[y]));
                        }
                        neighbour_comps[3] = 0xff;
                        s->pixels[(j + offsets[l + 1])
                            * s->width + i + offsets[l]] = 0;
                        pack(neighbour_comps, 
                                &s->pixels[(j + offsets[l + 1])
                                * s->width + i + offsets[l]]);
                    }
        }
    }
}
void default_line_floyd_steinberg(Canvas *s, u32 threshold)
{
    grey_scale(s);
    u32 dim = 5;
    i32 fac = 7;
    i32 x_off = -2 * fac;
    i32 y_off = 1 * fac;
    u8* d = malloc(dim * sizeof(u8));
    for (int i = 0; i < dim; i++)
    {
        d[i] = i * 0xff / (dim - 1);
    }
    line_floyd_steinberg(s, d, dim, x_off, y_off, threshold);
    free(d);
    invert(s);
}
void default_grey_scale_floyd_steinberg(Canvas* s)
{
    grey_scale(s);
    default_floyd_steinberg(s);
}
void default_floyd_steinberg(Canvas* s)
{
    u32 dim = 5;
    u8* d = malloc(dim * sizeof(u8));
    for (int i = 0; i < dim; i++)
    {
        d[i] = i * 0xff / (dim - 1);
    }
    floyd_stienberg(s, d, dim);
    free(d);
}
void median_filter(Canvas* s, u32 dim)
{
}
void sobel_filter_with_grads_signed(SignedCanvas* s, float* grad_val)
{
    i64* pixh = malloc(s->width*s->height * sizeof(i64));
    i64* pixv = malloc(s->width*s->height * sizeof(i64));

    i16 comps1[4] = {0, 0, 0, 0};
    i16 comps2[4] = {0, 0, 0, 0};
    for (int i = 0; i < s->width * s->height; i++)
    {
        pixh[i] = s->pixels[i];
        pixv[i] = s->pixels[i];
    }

    i32 grad[3] = {-1, 0, 1};
    i32 blur[3] = {1, 2, 1};

    signed_general_linear_1d_filter(pixh,s->width, s->height, blur, 3, FilterY);
    signed_general_linear_1d_filter(pixh,s->width, s->height, grad, 3, FilterX);
    signed_general_linear_1d_filter(pixv,s->width, s->height, blur, 3, FilterX);
    signed_general_linear_1d_filter(pixv,s->width, s->height, grad, 3, FilterY);


    for (int j = 0; j < s->width * s->height; j++)
    {
        signed_unpack(comps1, &pixh[j]);
        signed_unpack(comps2, &pixv[j]);
        grad_val[j] = atan2(comps2[0], comps1[0]);
        for (int i = 0; i < 3; i++)
        {
            comps1[i] = 
                sqrt(((f32)comps1[i] * (f32)comps1[i] 
                            + (f32)comps2[i] * (f32)comps2[i]));
        }
        s->pixels[j] = 0;
        signed_pack(comps1, &s->pixels[j]);
    }
}
void sobel_filter_with_grads(Canvas* s, float* grad_val)
{
    i64* pixh = malloc(s->width*s->height * sizeof(i64));
    i64* pixv = malloc(s->width*s->height * sizeof(i64));

    i16 comps1[4] = {0, 0, 0, 0};
    i16 comps2[4] = {0, 0, 0, 0};
    for (int i = 0; i < s->width * s->height; i++)
    {
        unsigned_to_signed(&pixh[i], &s->pixels[i]);
        unsigned_to_signed(&pixv[i], &s->pixels[i]);
    }

    i32 grad[3] = {-1, 0, 1};
    i32 blur[3] = {1, 2, 1};

    signed_general_linear_1d_filter(pixh,s->width, s->height, blur, 3, FilterY);
    signed_general_linear_1d_filter(pixh,s->width, s->height, grad, 3, FilterX);
    signed_general_linear_1d_filter(pixv,s->width, s->height, blur, 3, FilterX);
    signed_general_linear_1d_filter(pixv,s->width, s->height, grad, 3, FilterY);


    for (int j = 0; j < s->width * s->height; j++)
    {
        signed_unpack(comps1, &pixh[j]);
        signed_unpack(comps2, &pixv[j]);
        grad_val[j] = atan2(comps2[0], comps1[0]);
        for (int i = 0; i < 3; i++)
        {
            comps1[i] = 
                sqrt(((f32)comps1[i] * (f32)comps1[i] 
                            + (f32)comps2[i] * (f32)comps2[i]));
        }
        pixh[j] = 0;
        signed_pack(comps1, &pixh[j]);
        signed_to_unsigned(&pixh[j], &s->pixels[j]);
        if (!is_grey(s->pixels[j]))
        {
            print_channels(s->pixels[j]);
        }
    }
}
void sobel_filter(Canvas* s)
{
    i64* pixh = malloc(s->width*s->height * sizeof(i64));
    i64* pixv = malloc(s->width*s->height * sizeof(i64));

    i16 comps1[4] = {0, 0, 0, 0};
    i16 comps2[4] = {0, 0, 0, 0};
    for (int i = 0; i < s->width * s->height; i++)
    {
        unsigned_to_signed(&pixh[i], &s->pixels[i]);
        unsigned_to_signed(&pixv[i], &s->pixels[i]);
    }

    i32 grad[3] = {-1, 0, 1};
    i32 blur[3] = {1, 2, 1};

    signed_general_linear_1d_filter(pixh,s->width, s->height, blur, 3, FilterY);
    signed_general_linear_1d_filter(pixh,s->width, s->height, grad, 3, FilterX);
    signed_general_linear_1d_filter(pixv,s->width, s->height, blur, 3, FilterX);
    signed_general_linear_1d_filter(pixv,s->width, s->height, grad, 3, FilterY);


    for (int j = 0; j < s->width * s->height; j++)
    {
        signed_unpack(comps1, &pixh[j]);
        signed_unpack(comps2, &pixv[j]);
        (void)comps2;
        for (int i = 0; i < 3; i++)
        {
            comps1[i] = 
                sqrt(((f32)comps1[i] * comps1[i] 
                            + (f32)comps2[i] + comps2[i]));
        }
        pixh[j] = 0;
        signed_pack(comps1, &pixh[j]);
        signed_to_unsigned(&pixh[j], &s->pixels[j]);
    }
}

void canny_filter(Canvas* s, u8 t_low, u8 t_high)
{
    u8 temp = t_low;
    t_low = t_low * (t_low < t_high) + t_high * !(t_low < t_high);
    t_high = t_high * (temp < t_high) + temp * !(temp < t_high);
    i64* x_grad = malloc(s->width*s->height * sizeof(i64));
    i64* y_grad = malloc(s->width*s->height * sizeof(i64));
    u32* mag = malloc(s->width*s->height * sizeof(u32));
    u32* maximas = malloc(s->width*s->height * sizeof(u32));
    (void)maximas;
    i16 x_comps[4] = {0, 0, 0, 0};
    i16 y_comps[4] = {0, 0, 0, 0};
    u8 u_comps[4] = {0, 0, 0, 0};
    u8 orientation = 0;

    binomial_filter(s, 3);

    for (int i = 0; i < s->width * s->height; i++)
    {
        unsigned_to_signed(&x_grad[i], &s->pixels[i]);
        unsigned_to_signed(&y_grad[i], &s->pixels[i]);
        mag[i] = 0;
        maximas[i] = 0;
        s->pixels[i] = 0;
    }

    i32 grad_kernel[3] = {-1, 0, 1};
    signed_general_linear_1d_filter(x_grad, s->width, s->height, grad_kernel, 3, FilterX);
    signed_general_linear_1d_filter(y_grad, s->width, s->height, grad_kernel, 3, FilterY);

    for (int j = 0; j < s->height; j++)
    {
        for (int i = 0; i < s->width; i++)
        {
            signed_unpack(x_comps, &x_grad[j * s->width + i]);
            signed_unpack(y_comps, &y_grad[j * s->width + i]);
            u_comps[3] = x_comps[3];
        for (int v = 0; v < 3; v++)
        {
            u_comps[v] = 
                sqrt((f32)((f32)x_comps[v] * x_comps[v] 
                            + (f32)y_comps[v] + y_comps[v]));
        }
        pack(u_comps, &mag[j * s->width + i]);
        }
    }
    for (u32 j = 1; j < s->height - 1; j++)
    {
        for (u32 i = 1; i < s->width - 1; i++)
        {
           orientation = canny_get_orientation(x_grad[j * s->width + i], 
                   y_grad[j * s->width + i]);
           if (canny_is_local_max(mag, s->width, i, j, orientation, t_low))
           {
            //    maximas[j * s->width + i] = mag[j * s->width + i];
                s->pixels[j * s->width + i] = mag[j * s->width + i];
           }
        }
    }

    for (u32 j = 1; j < s->height - 1; j++)
    {
        for (u32 i = 1; i < s->width - 1; i++)
        {
            if (compare_with_comps(maximas[j * s->width + i], t_high) != -1 
                    &&  s->pixels[j * s->width + i] == 0)
            {
                printf("HERE\n");
                canny_trace_contour(maximas, mag, 
                        s->pixels, s->width, s->height, i, j, t_low); 
            }
        }
        }
    free(x_grad);
    free(y_grad);
    free(maximas);
    free(mag);
}

u8 canny_get_orientation(i64 dx, i64 dy)
{
    f64 tdx = (f64)dx;
    f64 tdy = (f64)dy;
    tdx = COS_PI_OVER_8 * tdx + -SIN_PI_OVER_8 * tdy;
    tdy = COS_PI_OVER_8 * tdy + SIN_PI_OVER_8 * tdx;
    tdx = tdx * (tdy >=0) - tdx * (tdy < 0);
    tdy = tdy * (tdy >=0) - tdy * (tdy < 0);

    return 
        (dx >= 0 && tdx < tdy) 
        + 2 * (dx < 0 && -dx < dy) + 
        3 * (dx < 0 && -dx >= dy);
}

u8 canny_is_local_max(u32* mag, u32 w, u32 x, u32 y, u32 orientation, u8 t_lo)
{
    if (compare_with_comps(mag[y * w + x], t_lo) == -1) return 0;
    u32 ml = 0;
    u32 mh = 0;
    switch (orientation)
    {
        case 0:
            {
                ml = mag[y * w + (x-1)];
                mh = mag[y * w + (x+1)];
            }
            break;
        case 1:
            {
                ml = mag[(y-1) * w + (x-1)];
                mh = mag[(y+1) * w + (x+1)];
            }
            break;
        case 2:
            {
                ml = mag[(y-1) * w + x];
                mh = mag[(y+1) * w + x];
            }
            break;
        case 3:
            {
                ml = mag[(y+1) * w + x - 1];
                mh = mag[(y-1) * w + x + 1];
            }
            break;
    }
    return (ml <= mag[y*w + x]) && (mh <= mag[y*w + x]);
}

void canny_trace_contour(u32* maximas, u32* mag, u32* contour,
        u32 w, u32 h,  u32 x, u32 y, u8 t_low)
{
    contour[y * w + x] = mag[y * w + x];
    u32 newminx = (x-1) * (((i64)x - 1) >= 0);
    u32 newmaxx = (x+1) * ((x + 1) <= (w-1)) + (w-1) * !((x+1) <= (w-1));
    u32 newminy = (y-1) * (((i64)y - 1) >= 0);
    u32 newmaxy = (y+1) * ((y + 1) <= (h-1)) + (h-1) * !((y+1) <= (h-1));

    for (u32 xx = newminx; xx < newmaxx + 1; xx++)
    {
        for (u32 yy = newminy; yy < newmaxy + 1; yy++)
        {
            if (compare_with_comps(maximas[yy * w + xx], t_low) != -1 && 
                    contour[yy * w + x] == 0)
            {
                canny_trace_contour(maximas, mag, contour, w, h, xx, yy, t_low);
            }
        }
    }
    return;
}

void sharpen(Canvas* s, float sharpen_factor)
{
    u32* smoothed_copy = malloc(s->width * s->height * sizeof(u32));
    u32* second_copy = malloc(s->width * s->height * sizeof(u32));

    for (int i =0; i < s->width * s->height; i++)
    {
        smoothed_copy[i] = s->pixels[i];
        second_copy[i] = s->pixels[i];
    }

    Canvas copy  = {.width = s->width, .height = s->height, .pixels = smoothed_copy};

    binomial_filter(&copy, 5);
    copy.pixels = second_copy;
    weighted_subtract_img(&copy, smoothed_copy, 1);

    weighted_add_img(s, copy.pixels, sharpen_factor);
}

void harris_filter(Canvas* s, float alpha, float threshold)
{
    u32* ix_pixels = malloc(s->width * s->height * sizeof(u32));
    u32* iy_pixels = malloc(s->width * s->height * sizeof(u32));
    u64* a_pixels_rg = malloc(s->width * s->height * sizeof(u64));
    u64* a_pixels_ba = malloc(s->width * s->height * sizeof(u64));

    u64* b_pixels_rg = malloc(s->width * s->height * sizeof(u64));
    u64* b_pixels_ba = malloc(s->width * s->height * sizeof(u64));

    u64* c_pixels_rg = malloc(s->width * s->height * sizeof(u64));
    u64* c_pixels_ba = malloc(s->width * s->height * sizeof(u64));

    u64* q_pixels_rg = malloc(s->width * s->height * sizeof(u64));
    u64* q_pixels_ba = malloc(s->width * s->height * sizeof(u64));


    for (int i = 0; i < s->width * s->height; i++)
    {
        ix_pixels[i] = s->pixels[i]; 
        iy_pixels[i] = s->pixels[i]; 
    }
    int hp[3] = {2, 5, 2};
    int hd[3] = {-1.0f, 0, 1.0f};
    Canvas Ix  = {.width = s->width, .height = s->height, .pixels = ix_pixels};
    Canvas Iy  = {.width = s->width, .height = s->height, .pixels = iy_pixels};

    general_linear_1d_filter(&Ix, hp, 3,    FilterX);
    general_linear_1d_filter(&Ix, hd, 3,    FilterX);

    general_linear_1d_filter(&Iy, hp, 3,    FilterY);
    general_linear_1d_filter(&Iy, hd, 3,    FilterY);


    for (int i = 0; i < s->width * s->height; i++)
    {
        u32 a_channels[4];
        long_unpack(a_channels, &a_pixels_rg[i], &a_pixels_ba[i]);

        u32 b_channels[4];
        long_unpack(b_channels, &b_pixels_rg[i], &b_pixels_ba[i]);

        u32 c_channels[4];
        long_unpack(c_channels, &c_pixels_rg[i], &c_pixels_ba[i]);

        u8 ychannels[4];
        unpack(ychannels, &Iy.pixels[i]);

        u8 xchannels[4];
        unpack(xchannels, &Ix.pixels[i]);

        for (int j = 0; j < 3; j++)
        {
            a_channels[j] = (((u32)xchannels[j] * xchannels[j]));
            b_channels[j] = (((u32)ychannels[j] * ychannels[j]));
            c_channels[j] = (((u32)ychannels[j] * xchannels[j]));
        }
        a_pixels_rg[i] = 0;
        a_pixels_ba[i] = 0;

        b_pixels_rg[i] = 0;
        b_pixels_ba[i] = 0;

        c_pixels_rg[i] = 0;
        c_pixels_ba[i] = 0;

        long_pack(a_channels, &a_pixels_rg[i], &a_pixels_ba[i]);
        long_pack(b_channels, &b_pixels_rg[i], &b_pixels_ba[i]);
        long_pack(c_channels, &c_pixels_rg[i], &c_pixels_ba[i]);
    }

    LargeCanvas A = {.width = s->width, .height = s->height, .rg = a_pixels_rg, .ba = a_pixels_ba};
    LargeCanvas B = {.width = s->width, .height = s->height, .rg = b_pixels_rg, .ba = b_pixels_ba};
    LargeCanvas C = {.width = s->width, .height = s->height, .rg = c_pixels_rg, .ba = c_pixels_ba};

    long_binomial_filter(&A, 2);
    long_binomial_filter(&B, 2);
    long_binomial_filter(&C, 2);
    
    (void)A;
    (void)B;
    (void)C;
#if 0
    for (int i = 0; i < s->width * s->height; i++)
    {
        if (A.ba[i] != 0)
            printf("%lx, %lx\n", A.rg[i], A.ba[i]);
    }
#endif


    for (int i = 0; i < s->width * s->height; i++)
    {

        u32 a_channels[4];
        long_unpack(a_channels, &a_pixels_rg[i], &a_pixels_ba[i]);

        u32 b_channels[4];
        long_unpack(b_channels, &b_pixels_rg[i], &b_pixels_ba[i]);

        u32 c_channels[4];
        long_unpack(c_channels, &c_pixels_rg[i], &c_pixels_ba[i]);

        u32 q_channels[4];
        long_unpack(q_channels, &q_pixels_rg[i], &q_pixels_ba[i]);

        for (int j = 0; j < 3; j++)
        {
            q_channels[j] = 
                fmax((a_channels[j] * b_channels[j]) -
                (c_channels[j] * c_channels[j]) -
                (alpha * ((a_channels[j] + b_channels[j])*
                                 (a_channels[j] + b_channels[j]))), 0);
        }
        q_pixels_rg[i] = 0;
        q_pixels_ba[i] = 0;
        long_pack(q_channels, &q_pixels_rg[i], &q_pixels_ba[i]);
    }
    for (int i = 0; i < s->width * s->height; i++)
    {
        u32 q_channels[4];
        long_unpack(q_channels, &q_pixels_rg[i], &q_pixels_ba[i]);
        int cond = check_max_pixel_large(q_pixels_rg, q_pixels_ba, s->width, s->height, i % s->width, i / s->width);
        int cond2 = 0;
        for (int j = 0; j < 3; j++)
        {
            cond2 |= (q_channels[j] >= threshold);
        }
        if (cond && cond2)
        {
            fill_circle(s, i % s->width, i / s->width, 5, RED);
            //point(s, i % s->width, i / s->width, RED);
        }
    }
    printf("%d\n",check_max_pixel_large(q_pixels_rg, q_pixels_ba, s->width, s->height, 
                s->width/2 + 50, s->height/2));
    printf("%lx, %lx\n",
            (q_pixels_rg[s->height/2 * s->width + s->width/2]),
            (q_pixels_ba[s->height/2 * s->width + s->width/2]));

}

int check_max_pixel(u32* pixels, int w, int h, int x, int y)
{

    int low_x = (-1) * (x != 0);
    int high_x = (x != (w-1));

    int low_y = (-1) * (y != 0);
    int high_y = (y != h - 1);

    int cond = 1;
    u8 main_channels[4];
    unpack(main_channels, &pixels[y * w + x]);

    int temp_cond = 0;
    for (int yy = low_y; yy < high_y + 1; yy++)
    {
        for (int xx = low_x; xx < high_x + 1; xx++)
        {
            temp_cond = 0;
            u8 temp_channels[4];
            unpack(temp_channels, &pixels[(y + yy) * w + (x + xx)]);
            for (int j = 0; j < 3; j++)
            {
               temp_cond |= (main_channels[j] >= temp_channels[j]);
            }
            cond &= temp_cond;
            if (cond == 0) return 0;
        }
    }
    return 1;
}

int check_max_pixel_large(u64* rg, u64* ba, int w, int h, int x, int y)
{

    int low_x = (-1) * (x != 0);
    int high_x = (x != (w-1));

    int low_y = (-1) * (y != 0);
    int high_y = (y != h - 1);

    int cond = 1;
    u32 main_channels[4];
    long_unpack(main_channels, &rg[y * w + x], &ba[y * w + x]);

    int temp_cond = 0;
    for (int yy = low_y; yy < high_y + 1; yy++)
    {
        for (int xx = low_x; xx < high_x + 1; xx++)
        {
            temp_cond = 0;
            u32 temp_channels[4];
            long_unpack(temp_channels, &rg[(y + yy) * w + (x + xx)], &ba[(y + yy) * w + (x + xx)]);
            for (int j = 0; j < 3; j++)
            {
               temp_cond |= ((u16)main_channels[j] >= (u16)temp_channels[j]);
            }
            cond &= temp_cond;
            if (cond == 0) return 0;
        }
    }
    return 1;
}

void dilate(Canvas* s, i32* dilate_kernel, u32 k_width)
{
    i64* signed_copy = malloc(sizeof(i64) * s->width * s->height);
    for (int i = 0; i < s->width * s->height; i++)
    {
        unsigned_to_signed(&signed_copy[i], &s->pixels[i]);
    }
    s_general_2d_generic_operator_filter(signed_copy, s->width, s->height,
            dilate_kernel, k_width, -INT_MAX, 1, p_s_generic_inplace_max);

    for (int i = 0; i < s->width * s->height; i++)
    {
        signed_to_unsigned(&signed_copy[i], &s->pixels[i]);
    }
}

void erode(Canvas* s, i32* erode_kernel, u32 k_width)
{

    i64* signed_copy = malloc(sizeof(i64) * s->width * s->height);
    for (int i = 0; i < s->width * s->height; i++)
    {
        unsigned_to_signed(&signed_copy[i], &s->pixels[i]);
    }
    s_general_2d_generic_operator_filter(signed_copy, s->width, s->height,
            erode_kernel, k_width, INT_MAX, 1, p_s_generic_inplace_min);

    for (int i = 0; i < s->width * s->height; i++)
    {
        signed_to_unsigned(&signed_copy[i], &s->pixels[i]);
    }
}


