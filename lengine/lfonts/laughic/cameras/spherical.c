#include "./spherical.h"
Vector3d spherical_ray_direction(CameraData* data, Point2D* p)
{
    Vector3d w = sub(&data->eye, &data->look_at);
    w = normalise(&w);
    Vector3d u = cross(&data->up, &w);
    u = normalise(&u);
    Vector3d v = cross(&w, &u);

    float width = ((SphericalData*)(data->extra))->width;
    float height = ((SphericalData*)(data->extra))->height;
    float pixel_size = ((SphericalData*)(data->extra))->pixel_size;
    float max_psi = ((SphericalData*)(data->extra))->max_psi;
    float max_lambda = ((SphericalData*)(data->extra))->max_lambda;

    Point2D pn = 
    {
        .x = 2.0 / (pixel_size * width) * p->x,
        .y = 2.0 / (pixel_size * height) * p->y
    };

    float lambda = pn.x * max_lambda * (PI / 180);
    float psi = pn.y * max_psi * (PI / 180);
    float phi = PI - lambda; 
    float theta = 0.5 * PI - psi;
    Vector3d scaled_u = scale(&u, sin(phi) * sin(theta));
    Vector3d scaled_v = scale(&v, cos(theta));
    Vector3d scaled_w = scale(&w, cos(phi) * sin(theta));
    Vector3d add_1 = add(&scaled_u, &scaled_v);
    return add(&add_1, &scaled_w);
}
