#include "jittered.h"

void generate_jittered_samples(SamplerData *data)
{
    int total_count = data->set_count * data->sample_count * data->sample_count;
    data->samples = malloc(total_count * sizeof(Point2D));
    for (int i = 0; i < data->set_count; i++)
    {
        int set_index = i * data->sample_count * data->sample_count;
        for (int samplesy = 0; samplesy < data->sample_count; samplesy++)
        {
            int yindex = samplesy * data->sample_count;
            for (int samplesx = 0; samplesx < data->sample_count; samplesx++)
            {
                Point2D new_sample = (Point2D)
                {
                    .x = (samplesx + rand_float()) / data->sample_count,
                    .y = (samplesy + rand_float()) / data->sample_count
                }; 
                data->samples[set_index + yindex + samplesx] = new_sample;
            }
        }
    }
}
