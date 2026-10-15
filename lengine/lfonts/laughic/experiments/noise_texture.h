#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H 
#include "../types.h"
#include "../math/vector3d.h"
#include <stdio.h>
#include <stdlib.h>
#define LERP(t, x0, x1) ((x0) + (t) * ((x1) - (x0)))
#define SMOOTH(x) ((x) * (x) * (x) * ((x) * ((x) * 6 - 15) + 10));

#define MAX_OCTAVES (16)


void init_noise();
void initGradientTable();
f32 glattice(int ix, int iy, int iz, f32 fx, f32 fy, f32 fz);
f32 gnoise(f32 x, f32 y, f32 z);
void init_fbm(float H, float lac);
f32 perPointFbm(Vector3d, f32 h, f32 lac, f32 octaves);
f32 perPointHybridMultiFractal(Vector3d p, 
        f32 h, f32 lac, f32 octaves, f32 offset);
f32 perPointWarp(Vector3d p, f32 h, f32 lac, f32 octaves, f32 dist);
int abs_floor(float x);
void warp(f32 h, f32 lac, f32 octaves, f32 dist, f32* data, i32 width, i32 height);
void normalize(float* data, u32 size);
void fbm(f32 h, f32 lac, f32 octaves, f32* data, i32 width, i32 height);
void worley(int n, float* data, int w, int h);
#endif
