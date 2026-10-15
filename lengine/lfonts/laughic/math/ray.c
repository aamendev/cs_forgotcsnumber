#include "ray.h"
#include "vector3d.h"
float distance(Ray *ray, Vector3d *p)
{
    Vector3d base_p = sub(p, &(ray->base));
   if (dot(&base_p, &(ray->direction)) < 0) 
       return magnitude(&base_p);
    Vector3d rejection = reject(&base_p, &(ray->direction));
    return magnitude(&rejection);
}
