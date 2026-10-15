#ifndef ORTHOGRAPHIC_CAMERA_H
#define ORTHOGRAPHIC_CAMERA_H
#include "./camera.h"

typedef struct OrthographicData
{
    Vector3d dir;
} OrthographicData;
Vector3d orthographics_ray_direction(CameraData* data, Point2D* p);

#define ORTHO(d) ((Camera)\
        {\
            .data = &d,\
            .ray_direction = orthographics_ray_direction\
        })
#endif
