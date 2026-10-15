#ifndef CAMERA_H
#define CAMERA_H
#include "../math/vector3d.h"
#include "../math/point2d.h"

typedef struct CameraData
{
   Vector3d eye;
   Vector3d up;
   Vector3d look_at;
   void* extra;
} CameraData;

typedef struct Camera
{
    CameraData* data;
    Vector3d (*ray_direction)(CameraData* data, Point2D* p);
} Camera;

#endif
