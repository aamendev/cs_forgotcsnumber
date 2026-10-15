#include "noise_texture.h"
#include "../math/utils.h"
#include <float.h>
#include <math.h>

#define TABSIZE (256)
static const int  perm[TABSIZE] =  {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7,
    225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134,
    139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
    105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80,
    73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
    147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
    28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181,
    199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236,
    205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

#define TABMASK (TABSIZE - 1)
#define PERM(x) perm[(x) & (TABMASK&0xff)]
#define INDEX(ix, iy, iz) PERM(ix + PERM(iy + PERM(iz)))
#define RANDMASK 0xffffffff
#define RANDNBR ((float)(rand() & RANDMASK) / RANDMASK) 

static float gradTable[TABSIZE*3];
static float exponentTable[MAX_OCTAVES];
static const float mulFactor = 0.1f;
int abs_floor(float x)
{
    int correction;
    if (x < 0 && x != (int)x)
    {
        correction = 1;
    }
    else
    {
        correction = 0;
    }
    return (int)x - correction; 
}
void initWorley(int randPoints, Vector3d* points, int w, int h, int d)
{
    for (int i = 0; i < randPoints; i++)
    {
        points[i] =  
            (Vector3d){
                (f32)(rand() % w), 
                (f32)(rand() % h), 
                (f32)(rand() % d), 
            };
    }
}

    void worley(int n, float* data, int w, int h)
    {
        Vector3d currentPoint;
        Vector3d* points = malloc(10 * sizeof(Vector3d));
        initWorley(10, points, w,h , 1);
        float* distances = malloc(10 * sizeof(float));
       // for (int z = 0; z < mDepth; z++)
        {
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++)
                {
                    currentPoint = (Vector3d){(f32)x, (f32)y, (f32)0};

                    for (int i = 0; i < 10; i++)
                    {
                        distances[i] = distance_squared(&points[i], &currentPoint);
                    }
                    qsort(distances, 10, sizeof(f32), float_compare);
                    f32 val = distances[n]; 
                    //currentOut = perPointWarp(currentPoint, h, lac, octaves, dist); 
                    data[y * w + x] = 
                        (val);
                }
            }
        }
        normalize(data, w * h);
    }


void init_noise()
{
    initGradientTable();
}
void initGradientTable()
{
    float z, r, theta;
    int i;
    r = 1.0f;
    for (i = 0; i < TABSIZE; i++)
    {
        z = 1.0 - 2.0 * RANDNBR;
        r = sqrt(fabs(1 - z*z));
        theta = 2 * M_PI * RANDNBR;
        gradTable[(i * 3)] = r * cos(theta);
        gradTable[(i * 3) + 1] = r * sin(theta);
        gradTable[(i * 3) + 2] = z;
    }
}
f32 glattice(int ix, int iy, int iz, f32 fx, f32 fy, f32 fz)
{
    int pos = (INDEX(ix, iy, iz)) * 3;
    float temp = gradTable[pos] * fx + gradTable[pos + 1] * fy + gradTable[pos + 2] * fz;
    return temp;
}
f32 gnoise(f32 x, f32 y, f32 z)
{
    int ix, iy, iz;
    float fx, fy, fz;
    float wx, wy, wz;
    float vx0, vx1, vy0, vy1, vz0, vz1;

    ix = abs_floor(x);
    fx = x - ix;
    wx = SMOOTH(fx);

    iy = abs_floor(y);
    fy = y - iy;
    wy = SMOOTH(fy);

    iz = abs_floor(z);
    fz = z - iz;
    wz = SMOOTH(fz);

    vx0 = glattice(ix, iy, iz, fx, fy, fz);
    vx1 = glattice(ix + 1, iy, iz, fx - 1, fy, fz);
    vy0 = LERP(wx, vx0, vx1);

    vx0 = glattice(ix, iy + 1, iz, fx, fy - 1, fz);
    vx1 = glattice(ix + 1, iy + 1, iz, fx - 1, fy - 1, fz);
    vy1 = LERP(wx, vx0, vx1);
    vz0 = LERP(wy, vy0, vy1);

    vx0 = glattice(ix, iy, iz + 1, fx, fy, fz-1);
    vx1 = glattice(ix + 1, iy, iz + 1, fx-1, fy, fz-1);
    vy0 = LERP(wx, vx0, vx1);

    vx0 = glattice(ix, iy + 1, iz + 1, fx, fy-1, fz-1);
    vx1 = glattice(ix + 1, iy + 1, iz + 1, fx-1, fy-1, fz-1);
    vy1 = LERP(wx, vx0, vx1);
    vz1 = LERP(wy, vy0, vy1);

    float ret = LERP(wz, vz0, vz1);
    return ret;
}

void init_fbm(float h, float lac)
{
    float frequency;
    int i;
    frequency = 1.0;
    for (i=0; i<MAX_OCTAVES; i++) 
    {
        exponentTable[i] = pow(frequency, -h);
        frequency *= lac;
    }
}

f32 perPointFbm(Vector3d point, f32 h, f32 lac, f32 octaves)
{
    float value, remainder;
    int i;
    value = 0.0;
    for (i=0; i< octaves; i++) {
        value += gnoise(point.x, point.y, point.z) * exponentTable[i];
        point.x *= lac;
        point.y *= lac;
        point.z *= lac;
    }
    remainder = octaves - abs_floor(octaves);
    value += (remainder > 0) * remainder * gnoise(point.x, point.y, point.z) * exponentTable[i];
    return(value);
}

f32 perPointWarp(Vector3d p, f32 h, f32 lac, f32 octaves, f32 dist)
{
    Vector3d temp = p;
    Vector3d distortedPoint;
    distortedPoint.x = perPointFbm(temp, h, lac, octaves);
        temp.x += 10.5f;
    //temp.x += 0.0f;
    distortedPoint.y = perPointFbm(temp, h, lac, octaves);
    temp.y += 10.5f;
    //temp.y += 0.0f;
    distortedPoint.z = perPointFbm(temp, h, lac, octaves);
    p.x += dist * distortedPoint.x;
    p.y += dist * distortedPoint.y;
    p.z += dist * distortedPoint.z;

    return perPointFbm(p, h, lac, octaves);
}

f32 perPointHybridMultiFractal(Vector3d p, 
        f32 h, f32 lac, f32 octaves, f32 offset)
{
    f32 value, remainder, signal, weight;
    value = (gnoise(p.x, p.y, p.z) + offset) * exponentTable[0];
    weight = value;
    p.x *= lac;
    p.y *= lac;
    p.z *= lac;

    for (int i = 1; i < octaves; i++)
    {
        weight = weight * !(weight > 1.0) + 1.0 * (weight > 1.0);
        signal = (gnoise(p.x, p.y, p.z) + offset) * exponentTable[i];
        value += weight * signal;
        weight *= signal;
        p.x *= lac;
        p.y *= lac;
        p.z *= lac;
    }
    remainder = octaves - abs_floor(octaves);
    value += (remainder > 0) * remainder * gnoise(p.x, p.y, p.z) * exponentTable[(int)octaves];
    return value;
}
void normalize(float* data, u32 size)
{
    f32 min = FLT_MAX;
    f32 max = -FLT_MAX;
    f32 current;
    for (int i = 0; i < size; i++)
    {
        current = data[i];
        min = min * (min < current) + current * !(min < current);
        max = max * (max > current) + current * !(max > current);
    }
    for (int i = 0; i < size; i++)
    {
        // (x - min) / (max-min) = y/(1-0)
        data[i] = (data[i] - min) / (max - min);
    }
}

void fbm(f32 h, f32 lac, f32 octaves, f32* data, i32 width, i32 height)
{
    init_fbm(h, lac);
    Vector3d currentPoint;
    f32 currentOut;
    //for (int z = 0; z < mDepth; z++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                currentPoint.x = x;
                currentPoint.y = y;
                currentPoint.z = 0;
                currentOut = perPointFbm(currentPoint, h, lac, octaves); 
                data[y * width + x] = 
                    (currentOut * mulFactor);
            }
        }
    }
    normalize(data, width * height);
}
void warp(f32 h, f32 lac, f32 octaves, f32 dist, f32 *data, int width, int height)
{
    init_fbm(h, lac);
    Vector3d currentPoint;
    f32 currentOut;
    //for (int z = 0; z < mDepth; z++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                currentPoint.x = x; 
                currentPoint.y = y; 
                currentPoint.z = 0; 
                currentOut = perPointWarp(currentPoint, h, lac, octaves, dist); 
                data[y * width + x] = 
                    (currentOut * mulFactor);
            }
        }
    }
    normalize((float*)data, width * height);
}
