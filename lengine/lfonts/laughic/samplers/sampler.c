#include "sampler.h"
#include <stdio.h>
Point2D get_next_general(SamplerData *data)
{
    int total_samples = data->sample_count * data->sample_count;
   if (data->used_count % (total_samples) == 0)
       data->init_index = (rand_int() % data->set_count) * total_samples; 
   return data->samples[data->init_index + data->used_count++ % (total_samples)];
}

void shuffle_general(SamplerData *data)
{

}

void set_disk_samples(SamplerData *data)
{
    int size = data->sample_count * data->sample_count * data->set_count;
    data->disk_samples = malloc(size * sizeof(Point2D));

    Point2D dp;
    float r, theta;
    for (int i = 0; i < size; i++)
    {
        dp.x = 2 * data->samples[i].x - 1;
        dp.y = 2 * data->samples[i].y - 1;
        r = 
            (dp.x > -dp.y) * 
            (dp.x * (dp.x > dp.y) + dp.y * !(dp.x > dp.y)) +
            !(dp.x > -dp.y) * 
            (-dp.x * (dp.x < dp.y) - dp.y * !(dp.x < dp.y));

        theta = (dp.x > -dp.y) * 
            (dp.y / dp.x * (dp.x > dp.y) + (2 - dp.x / dp.y) * !(dp.x > dp.y)) + 
            !(dp.x > -dp.y) * 
            ((4 + dp.y / dp.x) * (dp.x < dp.y) + 
             !(dp.x < dp.y) * (6 - dp.x / dp.y) * (dp.y != 0));
        theta *= (PI / 4);

        data->disk_samples[i] = (Point2D)
        {
            .x = r * cos(theta),
            .y = r * sin(theta)
        };
    }
}

void set_hemisphere_samples(SamplerData *data, float e)
{
   int size = data->sample_count * data->sample_count * data->set_count; 
   data->hemisphere_samples = malloc(size * sizeof(Vector3d));

   for (int i = 0; i < size; i++)
   {
        float cos_phi = cos(2 * PI * data->samples[i].x);
        float sin_phi = sin(2 * PI * data->samples[i].x);
        float cos_theta = pow((1 - data->samples[i].y), 1 / (e + 1));
        float sin_theta = sqrt(1 - cos_theta * cos_theta);
        data->hemisphere_samples[i] = (Vector3d)
        {
            .x = sin_theta * cos_phi,
            .y = sin_theta * sin_phi,
            .z = cos_theta
        };
   }
}
