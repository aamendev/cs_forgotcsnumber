#ifndef RAY_H
#define RAY_H
#include "vector3d.h"
typedef struct Ray
{
    Vector3d base;
    Vector3d direction;
} Ray;
float distance(Ray* ray, Vector3d* v);
#endif
