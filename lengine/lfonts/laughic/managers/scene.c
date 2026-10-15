#include "./scene.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include "../math/sphere.h"

void simple_tracer(SceneData* scene, MaterialData* material)
{
    scene->sampler->generate_samples(scene->sampler->data);
    float x, y;
    float* means = malloc((scene->traceable_count + 1) * sizeof(float));
    u32* cols = malloc((scene->traceable_count + 1) * sizeof(u32));

    for (int i = 0; i < scene->traceable_count; i++)
    {
        cols[i] = scene->traceables[i]->col;
        scene->traceables[i]->id = i;
    }
    cols[scene->traceable_count] = scene->default_colour;

    int samples = scene->sampler->data->sample_count;

    for (int j = 0; j < scene->canvas->height; j++)
    {
        for (int i = 0; i < scene->canvas->width; i++)
        {
            for (int k = 0; k < scene->traceable_count; k++) means[k] = 0;

            for (int k = 0; k < samples * samples; k++)
            {
                Point2D next_sample = scene->sampler->get_next(scene->sampler->data);
                x = scene->pixelSize * 
                    (i - 0.5 * (scene->canvas->width - 1) + next_sample.x);
                y = scene->pixelSize * 
                    (j - 0.5 * (scene->canvas->height - 1) + next_sample.y);
                scene->ray->base.x = x;
                scene->ray->base.y = y;
                int currMin = 0;
                float currMinVal = FLT_MAX;
                for (int i = 0; i < scene->traceable_count; i++)
                {
                    float t = scene->traceables[i]->intersects
                            (scene->ray, scene->traceables[i]->data, NULL);
                    if (t >= 0)
                    {
                        printf("Value: %f\n ", t);
                        currMin = i * (t < currMinVal) + currMin * !(t < currMinVal);
                        currMinVal = t * (t < currMinVal) + currMinVal * !(t < currMinVal);
                    }
                }
                 means[currMin]++; 
            }
            float sum = 0;
            for (int i = 0; i < scene->traceable_count; i++)
            {
                means[i] /= (samples * samples);
                sum += means[i];
            }
            means[scene->traceable_count] = 1 - sum;
            u32 col = weighted_sum(&cols[0], &means[0], scene->traceable_count + 1);
            fill_rect(scene->canvas, i, j, scene->pixelSize, scene->pixelSize, col);
        }
    }
    free(cols);
    free(means);
}


void perspective_tracer(SceneData *scene, MaterialData* material)
{
    switch(scene->optimized)
    {
        case 0 :
            {
                unoptimized_perspective_tracer(scene, material);
            }break;
        case 1:
            {
                optimized_perspective_tracer(scene, material);
            }break;
    }
}

void optimized_perspective_tracer(SceneData* scene, MaterialData* material)
{
    struct BVH* bvh = malloc(sizeof(struct BVH));
    if (bvh == NULL)
    {
        printf("BVH Error\n");
        exit(1);
    }
    scene->sampler->generate_samples(scene->sampler->data);
    float* means = malloc((scene->traceable_count + 1) * sizeof(float));
    u32* cols = malloc((scene->traceable_count + 1) * sizeof(u32));

    for (int i = 0; i < scene->traceable_count; i++)
    {
        cols[i] = scene->traceables[i]->col;
        scene->traceables[i]->id = i;
    }
    construct_bvh(scene, bvh);
    cols[scene->traceable_count] = scene->default_colour;
    Point2D pp;

    scene->ray->base = scene->cam->data->eye;
    int samples = scene->sampler->data->sample_count;

    struct BVH** stack = 
        malloc(scene->traceable_count * sizeof(struct BVH*));

    for (int j = 0; j < scene->canvas->height; j++)
    {
        for (int i = 0; i < scene->canvas->width; i++)
        {
            for (int k = 0; k < scene->traceable_count; k++) means[k] = 0;

            for (int k = 0; k < samples * samples; k++)
            {
                Point2D next_sample = scene->sampler->get_next(scene->sampler->data);
                pp.x = scene->pixelSize * 
                    (i - 0.5 * (scene->canvas->width - 1) + next_sample.x);
                pp.y = scene->pixelSize * 
                    (j - 0.5 * (scene->canvas->height - 1) + next_sample.y);
                scene->ray->direction =
                    scene->cam->ray_direction(scene->cam->data, &pp);

                // DFS
                struct BVH* current = bvh;
                int count = -1;
                stack[0] = current;
                count++;
                int currMin = -1;
                float currMinVal = FLT_MAX;
                float inter_param = -1.0f;
                while (count > -1)
                {
                    current = stack[count--];
                    inter_param = current->root->intersects
                            (scene->ray, current->root->data, NULL);
                    int cond = (inter_param >= 0 && inter_param < currMinVal);
                    if (cond)
                    {
                        if (current->left == NULL && current->right == NULL)
                        {
                            if (inter_param < currMinVal)
                            {
                                currMin = current->root->id;
                                currMinVal = inter_param;
                            }
                           // break;
                        }
                        else 
                        { 
                            stack[++count] = current->right;
                            stack[++count] = current->left;
                        }
                    }
                    else {
                    }
                }
                if (currMin > -1)
                {
                means[currMin]++; 
                }
            }
            float sum = 0;
            for (int k = 0; k < scene->traceable_count; k++)
            {
                means[k] /= (samples * samples);
                sum += means[k];
            }
            means[scene->traceable_count] = 1 - sum;
            u32 col = weighted_sum(&cols[0], &means[0], scene->traceable_count + 1);
            fill_rect(scene->canvas, i, j, scene->pixelSize, scene->pixelSize, col);
        }
    }
    free(stack);
    free(cols);
    free(means);
    free(bvh);
}

static u32 ray_march(Traceable** traceables, u32 traceable_count, Ray* ray, Vector3d* normal,
        Vector3d* point, Vector3d* uv)
{
    float min_dist = 0.0001f;
    u32 max_iter = 100000;
    Ray* new_ray = malloc(sizeof(Ray));
    new_ray->base = ray->base;
    new_ray->direction = ray->direction;

    int curr_min_idx = traceable_count;
    float curr_min_inter = FLT_MAX;
    f32 last_step = FLT_MAX;
    u32 current_iter = 0;
    Vector3d move_ray;
    while (last_step > min_dist && current_iter < max_iter)
    {
        curr_min_idx = traceable_count;
        curr_min_inter = FLT_MAX;
        for (int i = 0; i < traceable_count; i++)
        {
            float t = traceables[i]->sdf(traceables[i], &new_ray->base);
            curr_min_idx = i * (t < curr_min_inter) + curr_min_idx * !(t < curr_min_inter);
            curr_min_inter = t * (t < curr_min_inter) + curr_min_inter * !(t < curr_min_inter);
        }
        last_step = curr_min_inter;
        move_ray = scale(&new_ray->direction, last_step);
        new_ray->base = add(&new_ray->base, &move_ray);
        current_iter++;
    }
    /*
     *   const float eps = 0.0001; // or some other value
    const vec2 h = vec2(eps,0);
    return normalize( vec3(f(p+h.xyy) - f(p-h.xyy),
                           f(p+h.yxy) - f(p-h.yxy),
                           f(p+h.yyx) - f(p-h.yyx) ) );
     */
    if (current_iter >= max_iter || curr_min_idx == traceable_count) 
    {
        free(new_ray);
        normal = NULL;
        return traceable_count;
    }

#if 1
    Sphere* sdata = (Sphere*)(traceables[curr_min_idx]->data);
    float r = sdata->r;
    Vector3d osinter = sub(&new_ray->base, &(sdata->center));
    osinter = normalise(&osinter);

        float theta = -atan2(osinter.z,osinter.x);
        float circle_fac = 0.75f;
        uv->x = -theta/(2.0f * M_PI * circle_fac) * (theta < 0) + 
            (1 - theta/(2.0f * M_PI * circle_fac)) * (theta >= 0);
        float phi = asin(osinter.y/r);
        uv->y = phi/M_PI + 0.5f;
        if (uv->y == 0 || uv->y == 1)
        {
            uv->x = 0.5f;
        }
        uv->z = 0;
#endif

    if (normal != NULL)
    {
    f32 eps = 0.0001f;
    Vector3d hx = {eps, 0, 0};
    Vector3d hy = {0, eps, 0};
    Vector3d hz = {0, 0, eps};
    Vector3d old_base = new_ray->base;
    Vector3d curr = add(&old_base, &hx);
    Vector3d curr2 = sub(&old_base, &hx);
    normal->x  =
    -(traceables[curr_min_idx]->sdf(traceables[curr_min_idx],
                &curr) - 
    traceables[curr_min_idx]->sdf(traceables[curr_min_idx],
                &curr2));
    curr = add(&old_base, &hy);
    curr2 = sub(&old_base, &hy);
    normal->y  =
    -(traceables[curr_min_idx]->sdf(traceables[curr_min_idx],
                &curr) - 
    traceables[curr_min_idx]->sdf(traceables[curr_min_idx],
                &curr2));
    curr = add(&old_base, &hz);
    curr2 = sub(&old_base, &hz);
    normal->z  =
    -(traceables[curr_min_idx]->sdf(traceables[curr_min_idx],
                &curr) - 
    traceables[curr_min_idx]->sdf(traceables[curr_min_idx],
                &curr2));

    *normal = normalise(normal);
    }

    if (point != NULL)
    {
    *point = new_ray->base;
    }

    free(new_ray);
    return curr_min_idx;
}
static u32 get_light(Traceable** traceables, 
        u32 traceable_count,
        u32 idx, Vector3d* point, Vector3d* normal, Vector3d* cam_dir, MaterialData* material)
{
    u32 col = BLACK;
    u32 ambient_colour = material->ambient_light->get_radiance(NULL, 
            material->ambient_light->data, 0);
    u8 ambient_comp[4];
    unpack(ambient_comp, &ambient_colour);
    u32 amb = traceables[idx]->ambient.rho(
            traceables[idx]->ambient.data, NULL);
    u8 material_amb_comp[4];
    unpack(material_amb_comp, &amb);

    u32 l_colour;
    Ray light_ray;
    f32 light_distance;
    u8 diffuse_comps[4] = {0, 0, 0, 0xff};
    u8 specular_comps[4] = {0, 0, 0, 0xff};
    u8 l_comps[4] = {0, 0, 0, 0xff};
    u8 c_comps[4] = {0, 0, 0, 0xff};
    for (int l_index = 0; l_index < material->light_count; l_index++)
    {
        l_colour = BLACK;
        u32 diffuse = BLACK;
        u32 specular = BLACK;
        u32 count = 0;

        // todo: cone sample
        light_ray = material->lights[l_index].get_rays(point, 
                material->lights[l_index].data, &count)[0];
        u32 new_idx = ray_march(traceables, traceable_count, &light_ray, NULL, NULL, NULL);
        float ndotw = dot(&light_ray.direction, normal);
        if (new_idx == idx && ndotw >= 0.00f) 
        {

        Vector3d temp = sub(point, &light_ray.base);
        light_distance = magnitude(&temp);

        l_colour =
            material->lights[l_index].get_radiance(point,
                    material->lights[l_index].data, light_distance);
        diffuse = 
            traceables[idx]->
            diffuse.f(traceables[idx]->diffuse.data,
                    cam_dir, &light_ray.direction);
        GlossySpecularData* sd = 
            (GlossySpecularData*) 
            traceables[idx]->specular.data;
        sd->latest_normal = normal;

        specular = 
            traceables[idx]->
            specular.f(traceables[idx]->specular.data,
                    cam_dir, &light_ray.direction);
        }
        unpack(diffuse_comps, &diffuse); 
        unpack(specular_comps, &specular); 
        unpack(l_comps, &l_colour);
        for (int cc = 0; cc < 3; cc++)
        {
            float fac = (f32)l_comps[cc] / 0xff * 
                ((f32)diffuse_comps[cc] / 0xff 
                 + (f32)specular_comps[cc] / 0xff) * ndotw;
            fac += ((f32)ambient_comp[cc] / 0xff) * 
                (f32)material_amb_comp[cc] / 0xff;

            c_comps[cc] = fmin(0xff, (u16)c_comps[cc] + (u16)(fac * 0xff));
        }

    }
    c_comps[3] = 0xff;
    col = 0;
    pack(c_comps, &col);
    return col;
}
void fix_image(i16* img,  u32 width, u32 height)
{
    /*
    int x_grad = 0;
    int correction_mode = 0;

    i16* new_img;
    for (int j = 0; j < height; j++)
    {
        i16 prev = img[j * width];
        i16 curr_max = prev;
        for (int i = 1; i < width; i++)
        {
            i16 curr = img[i];
            x_grad = (curr > prev) - (curr < prev);
            correction_mode = (x_grad < 0) || (curr < curr_max);
            curr_max = curr_max * (curr < curr_max) + curr * !(curr < curr_max);
            if (correction_mode)
            {
               new_img[j * width + i] = curr - curr_max;
            }
            else 
            {
                new_img[j * width + i] = 0;
            }
        }
    }
    */
}
void bake_vdm(SceneData* scene, MaterialData* material)
{

    fill(scene->canvas, scene->default_colour);
    scene->sampler->generate_samples(scene->sampler->data);
     float* means = malloc((scene->traceable_count + 1) * sizeof(float));
    u32* cols = malloc((scene->traceable_count + 1) * sizeof(u32));

    for (int i = 0; i < scene->traceable_count; i++)
    {
        cols[i] = scene->traceables[i]->col;
        scene->traceables[i]->id = i;
    }
    cols[scene->traceable_count] = scene->default_colour;

    int samples = scene->sampler->data->sample_count;

    scene->ray->base = scene->cam->data->eye;
    Point2D pp;
    Vector3d normal;
    Vector3d inter_point;
    Vector3d uv;
   // f32 furthest = 1.0f;
    for (int j = 0; j < scene->canvas->height; j++)
    {
        for (int i = 0; i < scene->canvas->width; i++)
        {
            int currMin = scene->traceable_count;
              for (int k = 0; k < scene->traceable_count; k++) means[k] = 0;

             for (int k = 0; k < samples * samples; k++)
            {
                Point2D next_sample = scene->sampler->get_next(scene->sampler->data);
                pp.x = scene->pixelSize * 
                    (i - 0.5 * (scene->canvas->width - 1) + next_sample.x);
                pp.y = scene->pixelSize * 
                    (j - 0.5 * (scene->canvas->height - 1) + next_sample.y);

                scene->ray->direction =
                    scene->cam->ray_direction(scene->cam->data, &pp);
                scene->ray->direction = (Vector3d){0,0,-1};
                scene->ray->base = (Vector3d)
                {
                    pp.x/(scene->canvas->width-1),
                     pp.y/(scene->canvas->height-1),
                      scene->cam->data->eye.z
                };
                
                currMin = ray_march(scene->traceables, scene->traceable_count, scene->ray, &normal,
                        &inter_point, &uv);

                    means[currMin]++; 
                    if (currMin != scene->traceable_count)
                    {
                        u8 comps[4]; 
                        unpack(comps, &scene->canvas->pixels[j * scene->canvas->width + i]);
                        Vector3d d = sub(&inter_point, &uv);
                        //float dd = magnitude(&d);
                        comps[0] = fmax(0.0f, d.x) * 0xff;
                        comps[1] = fmax(0.0f, d.y) * 0xff;
                        comps[2] = fmax(0.0f, d.z) * 0xff;
                        cols[currMin] = 0;
                        pack(comps, &cols[currMin]);
                    }
            }
            float sum = 0;
              for (int i = 0; i < scene->traceable_count; i++)
              {
                  means[i] /= (samples * samples);
                  sum += means[i];
              }
              means[scene->traceable_count] = 1 - sum;
              
            u32 col = weighted_sum(&cols[0], &means[0], scene->traceable_count + 1);
            //u32 col = BLACK;
            mix_colour(&(scene->canvas->pixels[j * scene->canvas->width + i]), col);
        }
    }
    free(cols);
    // free(means);
}
void simple_marcher(SceneData* scene, MaterialData* material)
{
    scene->sampler->generate_samples(scene->sampler->data);
 //   float x, y;
     float* means = malloc((scene->traceable_count + 1) * sizeof(float));
    u32* cols = malloc((scene->traceable_count + 1) * sizeof(u32));

    for (int i = 0; i < scene->traceable_count; i++)
    {
        cols[i] = scene->traceables[i]->col;
        scene->traceables[i]->id = i;
    }
    cols[scene->traceable_count] = scene->default_colour;

    int samples = scene->sampler->data->sample_count;

    scene->ray->base = scene->cam->data->eye;
    Point2D pp;
    Vector3d normal;
    Vector3d inter_point;
    for (int j = 0; j < scene->canvas->height; j++)
    {
        for (int i = 0; i < scene->canvas->width; i++)
        {
            int currMin = scene->traceable_count;
              for (int k = 0; k < scene->traceable_count; k++) means[k] = 0;

             for (int k = 0; k < samples * samples; k++)
            {
                Point2D next_sample = scene->sampler->get_next(scene->sampler->data);
                pp.x = scene->pixelSize * 
                    (i - 0.5 * (scene->canvas->width - 1) + next_sample.x);
                pp.y = scene->pixelSize * 
                    (j - 0.5 * (scene->canvas->height - 1) + next_sample.y);

                scene->ray->direction =
                    scene->cam->ray_direction(scene->cam->data, &pp);
                currMin = ray_march(scene->traceables, scene->traceable_count, scene->ray, &normal,
                        &inter_point, NULL);

                    means[currMin]++; 
                    if (currMin != scene->traceable_count)
                    {
                        cols[currMin] = get_light(
                                scene->traceables,
                                scene->traceable_count,
                                currMin, &inter_point, &normal, &scene->ray->direction, 
                                material);
                    }
            }
            float sum = 0;
              for (int i = 0; i < scene->traceable_count; i++)
              {
                  means[i] /= (samples * samples);
                  sum += means[i];
              }
              means[scene->traceable_count] = 1 - sum;
              
            u32 col = weighted_sum(&cols[0], &means[0], scene->traceable_count + 1);
            //u32 col = BLACK;
            fill_rect(scene->canvas, i, j, scene->pixelSize, scene->pixelSize, col);
        }
    }
    free(cols);
    // free(means);
}
void unoptimized_perspective_tracer(SceneData* scene, MaterialData* material)
{
    scene->sampler->generate_samples(scene->sampler->data);
    float* means = malloc((scene->traceable_count + 1) * sizeof(float));
    u32* cols = malloc((scene->traceable_count + 1) * sizeof(u32));

    for (int i = 0; i < scene->traceable_count; i++)
    {
        cols[i] = scene->traceables[i]->col;
    }
    cols[scene->traceable_count] = scene->default_colour;
    Point2D pp;

    scene->ray->base = scene->cam->data->eye;
    int samples = scene->sampler->data->sample_count;

    for (int j = 0; j < scene->canvas->height; j++)
    {
        for (int i = 0; i < scene->canvas->width; i++)
        {
            for (int k = 0; k < scene->traceable_count; k++) means[k] = 0;

            for (int k = 0; k < samples * samples; k++)
            {
                Point2D next_sample = scene->sampler->get_next(scene->sampler->data);
                pp.x = scene->pixelSize * 
                    (i - 0.5 * (scene->canvas->width - 1) + next_sample.x);
                pp.y = scene->pixelSize * 
                    (j - 0.5 * (scene->canvas->height - 1) + next_sample.y);
                scene->ray->direction =
                    scene->cam->ray_direction(scene->cam->data, &pp);
                int currMin = -1;
                float currMinVal = FLT_MAX;
                float inter_param = -1.0f;
                for (int tr = 0; tr < scene->traceable_count; tr++)
                {
                    inter_param = scene->traceables[tr]->intersects
                            (scene->ray, scene->traceables[tr]->data, NULL);
                    int cond = (inter_param >= 0 && inter_param < currMinVal);
                    if (cond)
                    {
                        currMin = tr; 
                        currMinVal = inter_param;
                    }
                }
                if (currMin > -1)
                {
                    means[currMin]++;
                }
            }
            float sum = 0;
            for (int tr = 0; tr < scene->traceable_count; tr++)
            {
                means[tr] /= (samples * samples);
                sum += means[tr];
            }
            means[scene->traceable_count] = 1 - sum;
            u32 col = weighted_sum(&cols[0], &means[0], scene->traceable_count + 1);
            fill_rect(scene->canvas, i, j, scene->pixelSize, scene->pixelSize, col);
        }
    }
    free(cols);
    free(means);
}

void print_bvh(struct BVH* bv)
{
    struct BVH* b = bv;
    if (b == NULL) return;
    Vector3d curr_min;
    Vector3d curr_max;
    b->root->get_bounding_extents(b->root, &curr_min, &curr_max);
    printf("CURRID: %s\n", b->root->name);
    printf("CURRBOUND: ");
    print_vector(&curr_min);
    printf(", ");
    print_vector(&curr_max);
    printf("\n");
    printf("LEFT: \n{\n\t");
    print_bvh(b->left);
    printf("\n}");
    printf("RIGHT: \n{\n\t");
    print_bvh(b->right);
    printf("\n}");
}

void construct_bvh(SceneData* sc, struct BVH* bv)
{
    struct BVH* bvhs = malloc(sc->traceable_count * sizeof(struct BVH));
    for (int i = 0; i < sc->traceable_count; i++)
    {
        sc->traceables[i]->get_bvh(sc->traceables[i], &bvhs[i]);
    }
    int power = 1;
    int n = sc->traceable_count;
    int i;
    int final = 0;
    while (n > power)
    {
        for (i = 0; i < n; i += power<<1)
        {
            if (i < n-power)
            {
                combine_bvh(&bvhs[i], &bvhs[i + power]);
                final = i + 1;
            }
            else 
            {
                combine_bvh(&bvhs[0], &bvhs[i]);
                final = (i - (power<<1)) + 1;
            }
        }
        n = final;
        power <<= 1;
    }
    *bv = bvhs[0];
}

void combine_bvh(struct BVH* b0, struct BVH* b1)
{
    static int new_id = 10;
    if (b1 == NULL || b0 == NULL) {
        printf("SERIOUSERROR!\n");
        exit(1);
    };
    struct BVH* newLeft = malloc(sizeof(struct BVH));
    AABB leftBoundingBox; 
    b0->root->get_bounding_extents(b0->root, 
            &leftBoundingBox.min_coord, &leftBoundingBox.max_coord);
    AABB rightBoundingBox; 
    b1->root->get_bounding_extents(b1->root, 
            &rightBoundingBox.min_coord, &rightBoundingBox.max_coord);

    Vector3d new_min = 
    {
        fmin(leftBoundingBox.min_coord.x, rightBoundingBox.min_coord.x),
        fmin(leftBoundingBox.min_coord.y, rightBoundingBox.min_coord.y),
        fmin(leftBoundingBox.min_coord.z, rightBoundingBox.min_coord.z),
    };

    Vector3d new_max = 
    {
        fmax(leftBoundingBox.max_coord.x, rightBoundingBox.max_coord.x),
        fmax(leftBoundingBox.max_coord.y, rightBoundingBox.max_coord.y),
        fmax(leftBoundingBox.max_coord.z, rightBoundingBox.max_coord.z),
    };

    AABB* bounding_box = malloc(sizeof(AABB));
    bounding_box->max_coord = new_max;
    bounding_box->min_coord = new_min;
    bounding_box->bounding = 1;

    Traceable* bounding = malloc(sizeof(Traceable));
    char* name = malloc(30 * sizeof(char));
    sprintf(name, "bounding: %d, %d", b0->root->id, b1->root->id);
    *bounding = AABB_TRACE(bounding_box, WHITE, name);
    bounding->id = new_id++;

    newLeft->root = b0->root;
    newLeft->left = b0->left;
    newLeft->right =b0->right;

    b0->left = newLeft;
    b0->right = b1;
    b0->root = bounding;
}

void check_bvh_intersection(struct BVH* root, Ray* ray, 
        u32 trace_count, 
        i32* currMin, 
        f32* currMinVal,
        Vector3d* curr_normal)
{
    struct BVH** stack = 
        malloc(10 * trace_count * sizeof(struct BVH*));
    *currMin = -1;
    *currMinVal = FLT_MAX;
    struct BVH* current = root;
    int count = -1;
    stack[0] = current;
    count++;
    float inter_param = -1.0f;
    Vector3d min_normal;
    while (count > -1)
    {
        current = stack[count--];
        inter_param = current->root->intersects
            (ray, current->root->data, curr_normal);
        int cond = (inter_param > 0 && inter_param < *currMinVal);
        if (cond)
        {
            if (current->left == NULL && current->right == NULL)
            {
                *currMin = current->root->id;
                *currMinVal = inter_param;
                min_normal = *curr_normal;
            }
            else 
            {
                stack[++count] = current->right;
                stack[++count] = current->left;
            }
        }
    }
    if (*currMin > -1)
    {
    *curr_normal = min_normal;
    }
    free(stack);
}
void ligh_tracer(SceneData* scene, MaterialData* material)
{
    const float nudge_val = 0.001f;
    u32 ambient_colour = material->ambient_light->get_radiance(NULL, 
            material->ambient_light->data, 0);
    u8 ambient_comp[4];
    unpack(ambient_comp, &ambient_colour);

    struct BVH* bvh = malloc(sizeof(struct BVH));
    if (bvh == NULL)
    {
        printf("BVH Error\n");
        exit(1);
    }
    scene->sampler->generate_samples(scene->sampler->data);
    float* means = malloc((scene->traceable_count + 1) * sizeof(float));
    u32* cols = malloc((scene->traceable_count + 1) * sizeof(u32));
    u32* temp_cols = malloc((scene->traceable_count + 1) * sizeof(u32));

    for (int i = 0; i < scene->traceable_count; i++)
    {
        cols[i] = scene->traceables[i]->col;
        scene->traceables[i]->id = i;
    }
    construct_bvh(scene, bvh);
    cols[scene->traceable_count] = scene->default_colour;
    Point2D pp;

    scene->ray->base = scene->cam->data->eye;
    int samples = scene->sampler->data->sample_count;

    struct BVH** stack = 
        malloc(scene->traceable_count * sizeof(struct BVH*));
    int curr_mirror_depth = 0;

    for (int j = 0; j < scene->canvas->height; j++)
    {
        for (int i = 0; i < scene->canvas->width; i++)
        {
            for (int k = 0; k < scene->traceable_count; k++) 
            {
                means[k] = 0;
                temp_cols[k] = 0;
            }
            temp_cols[scene->traceable_count] = scene->default_colour;

                int currMin = -1;
                float currMinVal = FLT_MAX;
                Vector3d curr_pos;
                Vector3d curr_normal = {0,0,0};
                //printf("IN: %d\n", currMin);
            for (int k = 0; k < samples * samples; k++)
            {
                curr_mirror_depth = 0;
                Point2D next_sample = scene->sampler->get_next(scene->sampler->data);
                pp.x = scene->pixelSize * 
                    (i - 0.5 * (scene->canvas->width - 1) + next_sample.x);
                pp.y = scene->pixelSize * 
                    (j - 0.5 * (scene->canvas->height - 1) + next_sample.y);
                scene->ray->direction =
                    scene->cam->ray_direction(scene->cam->data, &pp);
                check_bvh_intersection(
                        bvh, scene->ray, scene->traceable_count, 
                        &currMin, &currMinVal, &curr_normal);
                currMinVal -= nudge_val;
                
                if (currMin > -1)
                {
                    curr_pos = scale(&scene->ray->direction, currMinVal);
                    curr_pos = add(&curr_pos, &scene->ray->base);
                    Ray new_ray = *scene->ray;
                    while (scene->traceables[currMin]->is_mirror)
                    {
                        if (curr_mirror_depth > scene->max_mirror_depth)
                        {
                            goto endSample;
                        }

                        curr_mirror_depth++;
                        new_ray.base = curr_pos;
                        Vector3d wi = scale(&new_ray.direction, -1);
                        Vector3d swi = scale(&wi, -1);
                        Vector3d scaled_normal = scale(&curr_normal, 
                                2 * dot(&curr_normal, &wi));
                        new_ray.direction = add(&swi, &scaled_normal);
                        new_ray.direction = normalise(&new_ray.direction);

                        check_bvh_intersection(bvh, &new_ray, scene->traceable_count, 
                               &currMin, &currMinVal, &curr_normal);

                        currMinVal -= nudge_val;
                        if (currMin > -1)
                        {
                            curr_pos = scale(&new_ray.direction, currMinVal);
                            curr_pos = add(&curr_pos, &new_ray.base);
                        }
                        else 
                        {
                            goto endSample;
                        }
                    }
                    means[currMin]++; 
                    i32 new_min = currMin;
                    u32 l_colour;
                    u8 l_comps[4];
                    u8 c_comps[4] = {0, 0, 0, 0};
                    u8 diffuse_comps[4];
                    u8 specular_comps[4];
                    //u8 temp_c_comps[4];
                    u32 count = 0;
                    Ray ligh_ray;
                    Vector3d light_normal;

                    u32 amb = scene->traceables[currMin]->ambient.rho(
                            scene->traceables[currMin]->ambient.data, NULL);
                    u8 material_amb_comp[4];
                    unpack(material_amb_comp, &amb);
                    int curr_light_depth = 0;
                    for (int l_index = 0; l_index < material->light_count; l_index++)
                    {
                        curr_light_depth = 0;
                        l_colour = BLACK;
                        u32 diffuse = BLACK;
                        u32 specular = BLACK;

                        // todo: cone sample
                        ligh_ray = material->lights[l_index].get_rays(&curr_pos, 
                                material->lights[l_index].data, &count)[0];
                        check_bvh_intersection(bvh, &ligh_ray, 
                                scene->traceable_count, 
                                &new_min, &currMinVal, &light_normal);
                        currMinVal -= nudge_val;

                        Vector3d l_curr_pos;

                        float curr_light_dist = 0.0f;
                    while (new_min != -1 
                            && scene->traceables[new_min]->is_mirror
                            && curr_light_depth <= scene->max_mirror_depth)
                    {
                        l_curr_pos = scale(&ligh_ray.direction, currMinVal);
                        l_curr_pos = add(&l_curr_pos, &ligh_ray.base);

                        Vector3d diff = sub(&l_curr_pos, &ligh_ray.base);
                        curr_light_dist += magnitude(&diff);

                        curr_light_depth++;
                        //Ray new_ray;
                        ligh_ray.base = l_curr_pos;

                        Vector3d wi = scale(&ligh_ray.direction, -1);

                        Vector3d swi = scale(&wi, -1);
                        Vector3d scaled_normal = scale(&light_normal, 
                                2 * dot(&light_normal, &wi));
                        ligh_ray.direction = add(&swi, &scaled_normal);
                        ligh_ray.direction = normalise(&ligh_ray.direction);

                        check_bvh_intersection(bvh, &ligh_ray, scene->traceable_count, 
                               &new_min, &currMinVal, &light_normal);
                        currMinVal -= nudge_val;
                    }

                        float ndotw = dot(&ligh_ray.direction, &curr_normal);
                        if (ndotw < 0 && new_min == currMin)
                        {
                            l_colour =
                                material->lights[l_index].get_radiance(&curr_pos,
                                        material->lights[l_index].data, curr_light_dist);
                            diffuse = 
                                scene->traceables[currMin]->
                                diffuse.f(scene->traceables[currMin]->diffuse.data,
                                        &new_ray.direction, &ligh_ray.direction);
                            GlossySpecularData* sd = 
                                (GlossySpecularData*) 
                                scene->traceables[currMin]->specular.data;
                            sd->latest_normal = &curr_normal;

                            specular = 
                                scene->traceables[currMin]->
                                specular.f(scene->traceables[currMin]->specular.data,
                                        &new_ray.direction, &ligh_ray.direction);
                        }

                        unpack(diffuse_comps, &diffuse); 
                        unpack(specular_comps, &specular); 
                        unpack(l_comps, &l_colour);
                        //unpack(temp_c_comps, &cols[currMin]);
                        for (int cc = 0; cc < 3; cc++)
                        {
                            float fac = (f32)l_comps[cc] / 0xff * 
                                ((f32)diffuse_comps[cc] / 0xff 
                                 + (f32)specular_comps[cc] / 0xff) * -ndotw;
                            fac += ((f32)ambient_comp[cc] / 0xff) * 
                                (f32)material_amb_comp[cc] / 0xff;

                            //c_comps[cc] = fmin(0xff, (u16)c_comps[cc] + (u16)(fac * 0xff));
                            c_comps[cc] = fmin(0xff, (u16)c_comps[cc] + (u16)(fac * 0xff));
                        }
                    }
                    c_comps[3] = 0xff;
                    temp_cols[currMin] = 0;
                    pack(c_comps, &temp_cols[currMin]);
                }
endSample:
                {}
            }
            float sum = 0;
            for (int k = 0; k < scene->traceable_count; k++)
            {
                means[k] /= (samples * samples);
                sum += means[k];
            }
            means[scene->traceable_count] = 1.0f - sum;
            u32 col = weighted_sum(&temp_cols[0], &means[0], scene->traceable_count + 1);
            fill_rect(scene->canvas, i, j, scene->pixelSize, scene->pixelSize, col);
        }
    }
    free(stack);
    free(cols);
    free(means);
    free(bvh);
}
