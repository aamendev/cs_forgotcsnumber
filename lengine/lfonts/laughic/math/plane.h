#ifndef PLANE_H
#define PLANE_H
#include "ray.h"
#include "vector3d.h"
#include "../logic_util.h"

typedef struct Plane
{
    Vector3d normal;
    float d;
} Plane;
float plane_intersects(Ray* ray, void* data);
#endif
