#include "vector3d.h"
#include <math.h>

float dot(Vector3d* v1, Vector3d* v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}
float squared_magnitude(Vector3d* v1)
{
    return dot(v1, v1);
}
float magnitude(Vector3d* v1)
{
    return sqrt(dot(v1, v1));
}
Vector3d cross(Vector3d* v1, Vector3d* v2)
{
    return (Vector3d){
        .x = v1->y * v2->z - v1->z * v2->y,
        .y = v1->z * v2->x - v1->x * v2->z,
        .z = v1->x * v2->y - v1->y * v2->x,
    };
}
Vector3d project(Vector3d* v1, Vector3d* v2)
{
    return scale(v2, dot(v1, v2) / squared_magnitude(v2));
}
Vector3d reject(Vector3d* v1, Vector3d* v2)
{
    Vector3d projected = project(v1, v2);
    return sub(v1, &projected);
}
Vector3d add(Vector3d* v1, Vector3d* v2)
{
    return (Vector3d){
        .x = v1->x + v2->x,
        .y = v1->y + v2->y,
        .z = v1->z + v2->z,
    };
}
int vector_equal(Vector3d* v1, Vector3d* v2)
{
    return 
        v1->x == v2->x && 
        v1->y == v2->y && 
        v1->z == v2->z; 
}
int is_zero(Vector3d* v1)
{
    return 
        v1->x == 0 && 
        v1->y == 0 && 
        v1->z == 0; 
}
Vector3d sub(Vector3d* v1, Vector3d* v2)
{
    return (Vector3d){
        .x = v1->x - v2->x,
        .y = v1->y - v2->y,
        .z = v1->z - v2->z,
    };
}
Vector3d scale(Vector3d* v, float factor)
{
    return (Vector3d){
        .x = v->x * factor,
        .y = v->y * factor,
        .z = v->z * factor,
    };
}
Vector3d scale_vector(Vector3d* v, Vector3d* s)
{
    return (Vector3d){
        .x = v->x * s->x,
        .y = v->y * s->y,
        .z = v->z * s->z,
    };
}
float get_theta(Vector3d* v1, Vector3d* v2)
{
    return dot(v1, v2) / (magnitude(v1) * magnitude(v2));
}
Vector3d normalise(Vector3d* v)
{
    if (squared_magnitude(v) == 0) return *v;
    return scale(v, 1 / magnitude(v));
}
void print_vector(Vector3d* v)
{
    printf("(%f, %f, %f)", v->x, v->y, v->z);
}
Vector3d mid_point(Vector3d* v1, Vector3d* v2)
{
    return 
    (Vector3d)
    {
        .x = (v1->x + v2->x) /2,    
        .y = (v1->y + v2->y) /2,    
        .z = (v1->z + v2->z) /2,    
    };
}

float distance_squared(Vector3d* v1, Vector3d* v2)
{
    Vector3d temp = 
    (Vector3d)
    {
        .x = (v1->x - v2->x),    
        .y = (v1->y - v2->y),    
        .z = (v1->z - v2->z),    
    };
    return squared_magnitude(&temp);
}
Vector3d to_unit_cube(Vector3d* v1, Vector3d* v2)
{
    return 
    (Vector3d)
    {
        .x = fabs(v1->x - v2->x) /2,    
        .y = fabs(v1->y - v2->y) /2,    
        .z = fabs(v1->z - v2->z) /2,    
    };
}
