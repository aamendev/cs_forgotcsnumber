#include "orthographic.h"
Vector3d orthographics_ray_direction(CameraData* data, Point2D* p)
{
    return ((OrthographicData*)(data->extra))->dir;
}
