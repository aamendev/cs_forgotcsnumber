#ifndef JITTERED_H
#define JITTERED_H
#include "sampler.h"
#include "../math/point2d.h"
#include "../logic_util.h"
void generate_jittered_samples(SamplerData* data);

#define JITTERED_SAMPLER(d) ((Sampler)\
        {\
        .data = &d,\
        .get_next = get_next_general,\
        .generate_samples = generate_jittered_samples,\
        .shuffle = shuffle_general\
        })
#endif
