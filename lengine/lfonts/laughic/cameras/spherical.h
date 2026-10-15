#ifndef SPHERICAL_CAMERA_H
#define SPHERICAL_CAMERA_H
#include "./camera.h"
#include "../logic_util.h"
typedef struct SphericalData
{
    float width;
    float height;
    float pixel_size;
    float max_psi;
    float max_lambda;
} SphericalData;

Vector3d spherical_ray_direction(CameraData* data, Point2D* p);

#define SPHERICAL_CAM(d) ((Camera)\
        {\
            .data = &d,\
            .ray_direction = spherical_ray_direction\
        })
#endif
