#ifndef PINHOLE_CAMERA_H
#define PINHOLE_CAMERA_H
#include "./camera.h"

typedef struct PinholeData
{
    float d;
} PinholeData;
Vector3d pinhole_ray_direction(CameraData* data, Point2D* p);

#define PINHOLE(d) ((Camera)\
        {\
            .data = &d,\
            .ray_direction = pinhole_ray_direction\
        })
#endif
