#include "./graphics_util.h"
#include "core/tree.h"
#include "logic_util.h"


void signed_unpack(i16* comp, i64* c)
{
    u64 uc = (u64)*c;
    for (int i = 0; i < 4; i++)
    {
        comp[i] = (i16)((uc >> (16 * i)) & 0xFFFFu);
    }
}

void signed_pack(i16* comp, i64* c)
{
    u64 u = 0;
    for (int i = 0; i < 4; i++)
    {
        u |= ((u64)(u16)comp[i]<<(16 * i));
    }
    *c = u;
}

void long_pack(u32* comp, u64* rg, u64* ba)
{
    for (int i = 0; i < 2; i++)
    {
        *rg |= (comp[i]<<(32 * i));
    }

    for (int i = 2; i < 4; i++)
    {
        *ba |= (comp[i]<<(32 * i));
    }
}
void long_unpack(u32* comp, u64* rg, u64* ba)
{

    for (int i = 0; i < 2; i++)
    {
        comp[i] = (*rg >> (32 * i)) &0xffffffff;
    }
    for (int i = 2; i < 4; i++)
    {
        comp[i] = (*ba >> (32 * i)) &0xffffffff;
    }
}


void signed_to_unsigned(i64* sgn, u32* un)
{
    i16 sgn_comp[4] = {0,0,0,0}; 
    u8 un_comp[4] = {0, 0, 0 ,0};
    signed_unpack(sgn_comp, sgn);
    for (int i = 0; i < 4; i++)
    {
        un_comp[i] = (u8)sgn_comp[i] * (sgn_comp[i] > -1 && sgn_comp[i] < 256)
            + 255 * (sgn_comp[i] > 255); 
    }
    *un = 0;
    pack(un_comp, un);
}

void unsigned_to_signed(i64* sgn, u32* un)
{
    i16 sgn_comp[4] = {0,0,0,0}; 
    u8 un_comp[4] = {0, 0, 0 ,0};
    unpack(un_comp, un);
    for (int i = 0; i < 4; i++)
    {
        sgn_comp[i] = un_comp[i]; 
        //printf("Old: %x, New: %x\n",un_comp[i], sgn_comp[i]);
    }
    *sgn = 0;
    signed_pack(sgn_comp, sgn);
    //printf("Old: %x, New: %lx\n",*un, *sgn);
}
void unpack(u8* comp, u32* c)
{
    comp[0] = *c&0XFF;
    comp[1] = (*c>>8) & 0XFF;
    comp[2] = (*c >> (8 * 2)) & 0xFF;
    comp[3] = (*c >> (8 * 3)) & 0xFF;
}

void pack(u8* comp, u32* c)
{
    for (int i = 0; i < 4; i++)
    {
        *c |= (u64)comp[i]<<(8 * i);
    }
}

void mix_component(u8* c1, u8 c2, u8 a)
{
    *c1 =  *c1 + (c2 - *c1) * a / 255;
}

void mix_colour(u32* c1, u32 c2)
{
    u8 comp1[4];
    unpack(comp1, c1);
    *c1 = 0;
    u8 comp2[4];
    unpack(comp2, &c2);
    for (int i = 0; i < 4; i++)
    {
        mix_component(&comp1[i], comp2[i], comp2[3]);
        *c1 |= comp1[i]<<(8 * i);
    }
}
u32 complement(u32 colour)
{
    u32 c = 0;
    u8 r = 0xFF - ((colour)&0xFF);
    u8 g = 0xFF - ((colour>>8)&0xFF);
    u8 b = 0xFF - ((colour>>(8 * 2))&0xFF);
    u8 a = (colour>>(8 * 3))&0xFF;
    c |= (r | g<<8 | b<<(8 * 2) | a<<(8 * 3));
    return c;
}

u32 weighted_sum(u32* cols, float* means, u32 count)
{
    u32 current = 0;
    u8 comps[4] = {0, 0, 0, 0};
    u8 out[4] = {0, 0 , 0 , 0};
    //float sum = 0;
    for (u32 i = 0; i < count; i++)
    {
        unpack(comps, &cols[i]);
        for (int j = 0; j < 4; j++)
            out[j] = fmin(0xff, (u16)out[j] + (comps[j] * means[i]));
       // sum += means[i];
    }
    pack(out, &current);
    return current;
}
void downscale_canvas(Canvas* src, Canvas* dst)
{
    u32 hn = dst->height;
    u32 wn = dst->width;
    if (hn >= src->height)
    {
        hn = src->height;
    }
    if (wn >= src->width)
    {
        wn = src->width;
    }
    for (u32 j = 0; j < hn; j++)
    {
        for (u32 i = 0; i < wn; i++)
        {
            dst->pixels[j * wn + i] = src->pixels[j * (src->height/hn) * src->width + i * (src->width/wn)];
        }
    }
}
int is_grey(u32 col)
{

    return get_channel(&col, ChannelRed) ==
        get_channel(&col, ChannelGreen)
        &&
        get_channel(&col, ChannelRed) ==
        get_channel(&col, ChannelBlue)
        &&
        get_channel(&col, ChannelBlue) ==
        get_channel(&col, ChannelGreen);
}
void print_channels(u32 col)
{
    printf("%u, %u, %u\n", 
            get_channel(&col, ChannelRed),
            get_channel(&col, ChannelGreen),
            get_channel(&col, ChannelBlue)
          );
}
void resize(Texture* src, u32 wn, u32 hn, u32** dst)
{
    *dst = (u32*)malloc(wn * hn * sizeof(u32));
    for (u32 j = 0; j < hn; j++)
    {
        for (u32 i = 0; i < wn; i++)
        {
            dst[0][j * wn + i] = src->data[j * src->height/hn * src->width + i * src->width/ wn];
        }
    }
}

void mandelbrot(Canvas* canvas, u16 iterations, u32 base_colour)
{
    //z_n+1 = zn^2 + c
    for (int i = 0; i < canvas->width * canvas->height; i++)
    {
        float x0 = 2.47 * (i % (canvas->width)) / canvas->width - 2;
        float y0 = 2.24 * (i / (canvas->width)) / canvas->height - 1.12;
        float currentX = 0;
        float currentY = 0;
        const float bound  = 4;
        int iter = 0;
        (void)x0;
        (void)y0;
        while (iter < iterations && currentX * currentX + currentY * currentY < bound)
        {
            float tempx = currentX * currentX - currentY * currentY;
            currentY = 2 * currentX * currentY + y0;
            currentX = tempx + x0;
            iter++;
        }
        mix_colour(&canvas->pixels[i] , (1 - (float)iter / iterations) *  base_colour);
    }
}

void julia(Canvas* canvas, u16 iterations, float zx, float zy, u32 base_colour)
{
    //z_n+1 = zn^2 + c
    float shift = 1.5 * 3.1415;
    const float r = 1.7; 
    for (int i = 0; i < canvas->width * canvas->height; i++)
    {

        float x0 = 2 * r * (i % (canvas->width)) / canvas->width - r;
        float y0 = 2 * r * (i / (canvas->width)) / canvas->height - r; 
        int iter = 0;
        while (iter < iterations && x0 * x0 + y0 * y0 < r * r)
        {
            float tempx = x0 * x0 - y0 * y0;
            y0 = 2 * x0 * y0 + zy;
            x0 = tempx + zx;
            iter++;
        }
        u8 comp[4] = {0,0,0,0};
        float freq[3] = {0.03, 0.018, 0.019};
        float phase[3] = {0.0, 0.0, 0.0};
        float continousFactor = iter + 1 - 1 /(zx * zx + zy * zy);
        base_colour = 0xff000000;
        for (int i = 0; i < 3; i++)
        {
            comp[i] = (0.5 * sin(freq[i] * continousFactor + phase[i] + shift) + 0.5) * 255;
            base_colour |= (comp[i]<<8*i);
        }
        mix_colour(&canvas->pixels[i], base_colour);
    }
}

void newton(Canvas* canvas, u16 iterations, u32 base_colour)
{
    //z = z^3 - 1
    float roots[6] = {1, 0, -0.5f, sqrt(3) / 2, -0.5, - sqrt(3) / 2};
    for (int i = 0; i < canvas->width * canvas-> height; i++)
    {
        float x0 = 4.0f * (i % (canvas->width)) / canvas->width - 2;
        float y0 = 4.0f * ((i / canvas->width)) / canvas->width - 2;
        int iter = 0;
        while (iter < iterations)
        {
            //z^2 = x^2 - y^2 +2xyi
            float derix = 3 * (x0 * x0 - y0 * y0);
            float deriy = 3 * 2 * x0 * y0;
            // z -= z ^ 3 - 1 / deri
            //z ^ 3 =(x + yi) ^ 3 = x^3 + 3x^2yi  - 3xy^2 - y^3i 
            float z3x = x0 * x0 * x0 - 3 * x0 * y0 * y0 - 1;
            float z3y = 3 * x0 * x0 * y0 - y0 * y0 * y0;

            x0 -= (z3x * derix + deriy * z3y) / (derix * derix + deriy * deriy);
            y0 -= (z3y * derix - deriy * z3x) / (derix * derix + deriy * deriy);

            float tol = 0.000001f;
            for (int i = 0; i < 3; i++)
            {
                float difx = x0 - roots[2 * i];
                float dify = y0 - roots[2 * i + 1];
                difx = difx * ((difx > 0) - !(difx > 0));
                dify = dify * ((dify > 0) - !(dify > 0));
                if (difx < tol && dify < tol)
                {
                    goto process;
                }
            }
            iter++;
        }
process:
        mix_colour(&canvas->pixels[i] , (1 - (float)iter / iterations) *  base_colour);
    }
}
void newton2(Canvas *canvas, u16 iterations, void (*func)(float, float, float *, float *), void (*deri)(float, float, float *, float *), float* roots, u16 nRoots)
{
    u32 base_colour = 0xff000000;
    float derix = 0.0f;
    float deriy = 0.0f;
    float zx = 0.0f;
    float zy = 0.0f;

    for (int i = 0; i < canvas->width * canvas-> height; i++)
    {
        float x0 = 4.0f * (i % (canvas->width)) / canvas->width - 2;
        float y0 = 4.0f * ((i / canvas->width)) / canvas->width - 2;
        int iter = 0;
        while (iter < iterations)
        {
            func(x0, y0, &zx, &zy);
            deri(x0, y0, &derix, &deriy);

            x0 -= (zx * derix + deriy * zy) / (derix * derix + deriy * deriy);
            y0 -= (zy * derix - deriy * zx) / (derix * derix + deriy * deriy);

            float tol = 0.000001f;
            for (int i = 0; i < nRoots; i++)
            {
                float difx = x0 - roots[2 * i];
                float dify = y0 - roots[2 * i + 1];
                difx = difx * ((difx > 0) - !(difx > 0));
                dify = dify * ((dify > 0) - !(dify > 0));
                if (difx < tol && dify < tol)
                {
                    goto process;
                }
            }
            iter++;
        }
process:
        mix_colour(&canvas->pixels[i] , (1 - (float)iter / iterations) *  base_colour);
    }
}

i8 compare_with_comps(u32 c, u8 t)
{
    u8 comp[4] = {0,0,0,0};
    unpack(comp, &c);
    u8 notEqual = 0;
    for (int i = 0; i < 3; i++)
    {
        if (comp[i] < t) return -1;
        else if (comp[i] > t)
        {
            notEqual = 1;
        }
    }
    return notEqual;
}


void set_channel(u32* col, u8 val, Channel channel)
{
    u8 comps[4] = {0, 0, 0, 0};
    unpack(comps, col);
    comps[channel] = val;
    *col = 0;
    pack(comps, col);
}


void set_opacity(u32 *col, u8 val)
{
    set_channel(col, val, ChannelAlpha);
}
u8 get_channel(u32* col, Channel channel)
{
    u8 comps[4];
    unpack(comps, col);
    return comps[channel];
}

void get_line_normal(Line* l, ParametricLine* normal)
{

    normal->x0 = (l->x0 + l->x1) / 2;
    normal->y0 = (l->y1 + l->y0) / 2;
    normal->dy = l->x1 - l->x0;
    normal->dx = -(l->y1 - l->y0);
    normal->t = 1;
}
void line_to_parametric(Line* l, ParametricLine* pl)
{
    f32 m = ((f32)(l->y1 - l->y0) / (l->x1 - l->x1));
    pl->x0 = l->x0;
    pl->y0 = l->x1;
    pl->dx = sqrt(1 / (1 + m * m));
    pl->dy = m * pl->dx;
}

void parametric_to_line(ParametricLine* pl, Line* l)
{
    l->x0 = pl->x0;
    l->y0 = pl->y0;

    l->x1 = l->x0 + pl->t * pl->dx;
    l->y1 = l->y0 + pl->t * pl->dy;
}


void read_to_canvas(Canvas* c, char* path)
{
    int w, h, channels;
    stbi_uc* data = stbi_load(path, &w, &h, &channels, STBI_rgb_alpha);
    u32* colour = (u32*)malloc(w * h * sizeof(u32));
    memcpy(colour, data, w * h * sizeof(u32));
    stbi_image_free(data);

    c->pixels = colour;
    c->width = w;
    c->height = h;
}

void read_texture(Texture* tex)
{
    int w, h, channels;
    stbi_uc* data = stbi_load(tex->path, &w, &h, &channels, STBI_rgb_alpha);
    u32* colour = (u32*)malloc(w * h * sizeof(u32));
    memcpy(colour, data, w * h * sizeof(u32));
    stbi_image_free(data);

    tex->data = colour;
    tex->width = w;
    tex->height = h;
}

int save_to_img(Canvas* canvas, Format format, char* dst)
{
    switch (format)
    {
        case PNG:
            {
                return stbi_write_png(dst, canvas->width, canvas->height, 4, canvas->pixels, canvas->width * 4);
            }
            break;
        case JPG:
            {
                return stbi_write_jpg(dst, canvas->width, canvas->height, 4, canvas->pixels, canvas->width * 4);
            }
        default:
            return -1;
    }
}

u32 get_cols(Canvas* c, u32* cols, u32 size)
{
    int count = 0;
    for (int i = 0; i < c->width * c->height && (count < size); i++)
    {
        int j;
        for (j = 0; j < count; j++)
        {
            if (cols[j] == c->pixels[i])
            {
                break;
            }
        }
        cols[j] = cols[j] * (j < count) + c->pixels[i] * (j == count);
        count += (j == count);
    }
    return count;
}

u32 get_cols_histogram(Canvas* c, ColourHistogramBar* bar, u32 size)
{
    int count = 0;
    for (int i = 0; i < c->width * c->height * 10/100 && (count < size); i++)
    {
        int idx = rand_int_bound(0, c->width * c->height);
        int j;
        for (j = 0; j < count; j++)
        {
            if (bar[j].col == c->pixels[idx])
            {
                bar[j].count++;
                break;
            }
        }
        if (j == count)
        {
            bar[j] = (ColourHistogramBar){.col = c->pixels[idx], .count = 1};
            count++;
        }
    }
    return count;
}

f32 colour_distance(u32 col1, u32 col2)
{
    return 
        (get_channel(&col1, ChannelRed) - 
         get_channel(&col2, ChannelRed)) *
        (get_channel(&col1, ChannelRed) - 
         get_channel(&col2, ChannelRed)) +
        (get_channel(&col1, ChannelBlue) - 
         get_channel(&col2, ChannelBlue)) *
        (get_channel(&col1, ChannelBlue) - 
         get_channel(&col2, ChannelBlue)) +
        (get_channel(&col1, ChannelGreen) - 
         get_channel(&col2, ChannelGreen)) *
        (get_channel(&col1, ChannelGreen) - 
         get_channel(&col2, ChannelGreen));
}
