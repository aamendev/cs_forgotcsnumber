#ifndef SAMPLER_H
#define SAMPLER_H
#include "../math/point2d.h"
#include "../logic_util.h"
#include "../math/vector3d.h"
typedef struct SamplerData
{
    int sample_count;
    int set_count;
    Point2D* samples;
    Point2D* disk_samples;
    Vector3d* hemisphere_samples;
    int init_index;
    int used_count;
    int* shuffled_indices;
} SamplerData;

typedef struct Sampler
{
    SamplerData* data;
    Point2D (*get_next)(SamplerData* data);
    void (*generate_samples)(SamplerData* data);
    void (*shuffle)(SamplerData* data);
    void (*map_samples_to_disk)(SamplerData* data);
    void (*map_samples_to_hemisphere)(SamplerData* data, float e);
} Sampler;

Point2D get_next_general(SamplerData* data);
void shuffle_general(SamplerData* data);
void set_disk_samples(SamplerData* data);
void set_hemisphere_samples(SamplerData* data, float e);

#endif
