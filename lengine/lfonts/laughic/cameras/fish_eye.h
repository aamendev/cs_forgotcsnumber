#ifndef FISH_EYE_H
#define FISH_EYE_H
#include "camera.h"
#include "../logic_util.h"
typedef struct FishEyeData
{
    float width;
    float height;
    float pixel_size;
    float max_psi;
} FishEyeData;

Vector3d fish_eye_ray_direction(CameraData* data, Point2D* p);

#define FISH_EYE(d) ((Camera)\
        {\
        .data = &d,\
        .ray_direction = fish_eye_ray_direction,\
        })
#endif
