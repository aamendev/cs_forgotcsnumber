#include "sample_scene.h"
#include "../lights/point_light.h"
#include "../lights/ambient_light.h"
#include "../../raytracer/brdf/lambertain.h"
#include "../../raytracer/brdf/glossy_specular.h"

void rect_scene(Canvas* canvas)
{

    Vector3d rayDirection = {.x = 0, .y = 0, .z = -1};
    Vector3d initPosition = {.x = 0, .y = 0, .z = 100};
    Ray r = {.direction = rayDirection, .base = initPosition};

    Sphere s = {.center = {.x = 50, .y = -30, .z = 25}, .r = 20.0f};
    Sphere mirror_sphere = {.center = {.x = 150, .y = 0, .z = -120}, .r = 100.0f};
    Sphere mirror_sphere2 = {.center = {.x = -100, .y = 20, .z = -60}, .r = 50.0f};
    AABB floor = 
    {
        .min_coord = {.x = -200, .y = 100, .z = -200},
        .max_coord = {.x = 200, .y = 101, .z = 200},
        .bounding = 0,
    };
    AABB mirroraabb = 
    {
        .min_coord = {.x = -100, .y = -150, .z = -150}, 
        .max_coord = {.x = -99, .y = 50, .z = 100},
        .bounding = 0,
    };
    AABB distant = 
    {
        .min_coord = {.x = 200, .y = -100, .z = -100}, 
        .max_coord = {.x = 201, .y = 100, .z = 100},
        .bounding = 0,
    };
    AABB front = 
    { .min_coord = {.x = 0, .y = 0, .z = 50}, 
        .max_coord = {.x = 100, .y = 100, .z = 51},
        .bounding = 0,
    };
    AABB right = 
    {
        .min_coord = {.x = 100, .y = 0, .z = 0},
        .max_coord = {.x = 101, .y = 100, .z = 50}, 
        .bounding = 0,
    };
    AABB back = 
    {
        .min_coord = {.x = 0, .y = 0, .z = 0}, 
        .max_coord = {.x = 100, .y = 100, .z = 1},
        .bounding = 0,
    };

    AABB left = 
    {
        .min_coord = {.x = 0, .y = 0, .z = 0},
        .max_coord = {.x = 1, .y = 100, .z = 50}, 
        .bounding = 0,
    };

    LambertainData distant_diffuse_data = 
    {
        .kd = 0.8f,
        .cd = BG
    };
    BRDF distant_diffuse = LAMBERTAIN_BRDF(&distant_diffuse_data, "distant_diffuse"); 
    LambertainData distant_ambient_data = 
    {
        .kd = 0.2f, .cd = BG
    };

    BRDF distant_ambient = LAMBERTAIN_BRDF(&distant_ambient_data, "distant_ambient"); 
    GlossySpecularData distant_specular_data = 
    {
        .ks = 0.3f,
        .cs = BG,
        .exp = 10,
    };
    BRDF distant_specular = GLOSSY_SPECULAR_BRDF(&distant_specular_data, "distant_specular"); 

    Traceable distantTrace = AABB_TRACE(&distant, YELLOW, "bf");
    distantTrace.specular = distant_specular;
    distantTrace.diffuse = distant_diffuse;
    distantTrace.ambient = distant_ambient;

    LambertainData front_diffuse_data = 
    {
        .kd = 0.8f,
        .cd = YELLOW
    };
    BRDF front_diffuse = LAMBERTAIN_BRDF(&front_diffuse_data, "front_diffuse"); 
    LambertainData front_ambient_data = 
    {
        .kd = 0.2f, .cd = YELLOW
    };

    BRDF front_ambient = LAMBERTAIN_BRDF(&front_ambient_data, "front_ambient"); 

    GlossySpecularData front_specular_data = 
    {
        .ks = 0.3f,
        .cs = YELLOW,
        .exp = 10,
    };
    BRDF front_specular = GLOSSY_SPECULAR_BRDF(&front_specular_data, "front_specular"); 

    Traceable frontTrace = AABB_TRACE(&front, YELLOW, "front");
    frontTrace.specular = front_specular;
    frontTrace.diffuse = front_diffuse;
    frontTrace.ambient = front_ambient;

    LambertainData right_diffuse_data = 
    {
        .kd = 0.5f,
        .cd = RED
    };
    BRDF right_diffuse = LAMBERTAIN_BRDF(&right_diffuse_data, "right_diffuse"); 
    LambertainData right_ambient_data = 
    {
        .kd = 0.2f,
        .cd = RED
    };

    BRDF right_ambient = LAMBERTAIN_BRDF(&right_ambient_data, "right_ambient"); 
    GlossySpecularData right_specular_data = 
    {
        .ks = 0.9f,
        .cs = RED,
        .exp = 10,
    };
    BRDF right_specular = GLOSSY_SPECULAR_BRDF(&right_specular_data, "right_specular"); 

    Traceable rightTrace = AABB_TRACE(&right, RED, "right");
    rightTrace.specular = right_specular;
    rightTrace.diffuse = right_diffuse;
    rightTrace.ambient = right_ambient;

    LambertainData left_diffuse_data = 
    {
        .kd = 0.6f,
        .cd = BLUE,
    };
    BRDF left_diffuse = LAMBERTAIN_BRDF(&left_diffuse_data, "left_diffuse"); 
    LambertainData left_ambient_data = 
    {
        .kd = 0.2f,
        .cd = BLUE
    };

    BRDF left_ambient = LAMBERTAIN_BRDF(&left_ambient_data, "left_ambient"); 
    GlossySpecularData left_specular_data = 
    {
        .ks = 0.7f,
        .cs = BLUE,
        .exp = 10,
    };
    BRDF left_specular = GLOSSY_SPECULAR_BRDF(&left_specular_data, "left_specular"); 

    Traceable leftTrace = AABB_TRACE(&left, BLUE, "left");
    leftTrace.specular = left_specular;
    leftTrace.diffuse = left_diffuse;
    leftTrace.ambient = left_ambient;

    LambertainData back_diffuse_data = 
    {
        .kd = 0.5f,
        .cd = GREEN
    };
    BRDF back_diffuse = LAMBERTAIN_BRDF(&back_diffuse_data, "back_diffuse"); 

    LambertainData back_ambient_data = 
    {
        .kd = 0.2f,
        .cd = GREEN
    };

    BRDF back_ambient = LAMBERTAIN_BRDF(&back_ambient_data, "back_ambient"); 

    GlossySpecularData back_specular_data = 
    {
        .ks = 0.5f,
        .cs = GREEN,
        .exp = 10,
    };
    BRDF back_specular = GLOSSY_SPECULAR_BRDF(&back_specular_data, "back_specular"); 

    Traceable backTrace = AABB_TRACE(&back, GREEN, "back");
    backTrace.specular = back_specular;
    backTrace.diffuse = back_diffuse;
    backTrace.ambient = back_ambient;

    LambertainData floor_diffuse_data = 
    {
        .kd = 0.9f,
        .cd = WHITE
    };
    BRDF floor_diffuse = LAMBERTAIN_BRDF(&floor_diffuse_data, "floor_diffuse"); 

    LambertainData floor_ambient_data = 
    {
        .kd = 0.0f,
        .cd = WHITE
    };

    BRDF floor_ambient = LAMBERTAIN_BRDF(&floor_ambient_data, "floor_ambient"); 
    GlossySpecularData floor_specular_data = 
    {
        .ks = 0.0f,
        .cs = WHITE,
        .exp = 1,
    };
    BRDF floor_specular = GLOSSY_SPECULAR_BRDF(&floor_specular_data, "floor_specular"); 

    Traceable floorTrace = AABB_TRACE(&floor, WHITE, "floor");
    floorTrace.specular = floor_specular;
    floorTrace.diffuse = floor_diffuse;
    floorTrace.ambient = floor_ambient;

    Traceable planeMirrorTrace = AABB_TRACE(&mirroraabb, GREEN, "mirror");
    planeMirrorTrace.is_mirror = 1;

    LambertainData ball_diffuse_data = 
    {
        .kd = 0.7f,
        .cd = CYAN
    };
    BRDF ball_diffuse = LAMBERTAIN_BRDF(&ball_diffuse_data, "ball_diffuse"); 

    LambertainData ball_ambient_data = 
    {
        .kd = 0.0f,
        .cd = WHITE
    };

    BRDF ball_ambient = LAMBERTAIN_BRDF(&ball_ambient_data, "ball_ambient"); 

    GlossySpecularData ball_specular_data = 
    {
        .ks = 0.7f,
        .cs = CYAN,
        .exp = 20,
    };
    BRDF ball_specular = GLOSSY_SPECULAR_BRDF(&ball_specular_data, "ball_specular"); 

    Traceable ballTrace = SPHERE_TRACE(&s, CYAN, "ball");
    ballTrace.specular = ball_specular;
    ballTrace.diffuse = ball_diffuse;
    ballTrace.ambient = ball_ambient;

    Traceable mirrorTrace = SPHERE_TRACE(&mirror_sphere, CYAN, "ball mirror");
    mirrorTrace.is_mirror = 1;

    Traceable mirrorTrace2 = SPHERE_TRACE(&mirror_sphere2, CYAN, "ball mirror");
    mirrorTrace2.is_mirror = 1;
    (void)mirrorTrace;
    (void)mirrorTrace2;
    (void)planeMirrorTrace;
    (void)distantTrace;


    Traceable* traceables[20] = 
    {
        &rightTrace, 
        &frontTrace, 
        &mirrorTrace,
        &backTrace, 
        &leftTrace, 
        &floorTrace, 
        &ballTrace, 
        &planeMirrorTrace, 
    };
    int trace_count = 8;
   // (void)ball;
    PointLight pl = 
    {
        .colour = WHITE,
        .ls = 1.0e5f,
        .pos = {100, -350, -50}
    };

    AmbientLight amb = 
    {
        .colour = WHITE,
        .ls = 0.2f
    };
    Light first_pl = POINT_LIGHT(&pl, "fpl");
    Light first_amb = AMBIENT_LIGHT(&amb, "famb");
    Light lights[20] = {first_pl, first_amb};
    int light_count = 1;

    SamplerData data = (SamplerData){
        .sample_count = 5,
        .set_count = 4,
        .used_count = 0,
        .init_index = 0,
        .samples = NULL,
        .shuffled_indices = NULL
    };

    PinholeData p_data = {.d = 100.0};

    FishEyeData f_data = 
    {
        .height = canvas->height,
        .width = canvas->width,
        .pixel_size = 1,
        .max_psi = 360
    };

    SphericalData s_data = 
    {
        .height = canvas->height,
        .width = canvas->width,
        .pixel_size = 1,
        .max_lambda = 90,
        .max_psi = 90
    };

    CameraData cam_data = (CameraData)
    {
        .eye = {.x =  200, .y = -200, .z = 120},
        .look_at = {.x = 180, .y = -200, .z = 0},
        .up = {.x = 0, .y = 1, .z = 0},
        .extra = &p_data,
    };
    /*Vector3d temp = sub(&cam_data.eye, &cam_data.look_at);
    temp = normalise(&temp);
    temp = scale(&temp, 15);
    cam_data.eye = add(&cam_data.eye, &temp);
    */

    Camera pinhole_cam = PINHOLE(cam_data);
    Camera fisheye_cam = FISH_EYE(cam_data);
    Camera sherical_cam = SPHERICAL_CAM(cam_data);
    (void)p_data;
    (void)pinhole_cam;
    (void)f_data;
    (void)fisheye_cam;
    (void)s_data;
    (void)sherical_cam;

    Sampler jittered = JITTERED_SAMPLER(data);
    SceneData scene_data = {
        .canvas = canvas, 
        .default_colour = BLACK,
        .sampler = &jittered,
        .pixelSize = 1,
        .ray = &r,
        .traceables = &traceables[0], 
        .traceable_count = trace_count,
        .cam = &pinhole_cam,
        .optimized = 1,
        .max_mirror_depth = 10,
    };
    MaterialData mat_data = 
    {
        .ambient_light = &first_amb,
        .lights = &lights[0],
        .light_count = light_count
    };

    Scene scene = {
        .scene_data = &scene_data,
        .ray_trace = ligh_tracer
    };

    scene.ray_trace(&scene_data, &mat_data);
}

void test_scene(Canvas* canvas)
{

    Vector3d rayDirection = {.x = 0, .y = 0, .z = -1};
    Vector3d initPosition = {.x = 0, .y = 0, .z = 100};
    Ray r = {.direction = rayDirection, .base = initPosition};

    Sphere s = {.center = {.x = 50, .y = -30, .z = 25}, .r = 20.0f};
    LambertainData ball_diffuse_data = 
    {
        .kd = 1.0f,
        .cd = CYAN
    };
    BRDF ball_diffuse = LAMBERTAIN_BRDF(&ball_diffuse_data, "ball_diffuse"); 

    LambertainData ball_ambient_data = 
    {
        .kd = 0.5f, .cd = CYAN
    };
    BRDF ball_ambient = LAMBERTAIN_BRDF(&ball_ambient_data, "ball_ambient"); 

    GlossySpecularData ball_specular_data = 
    {
        .ks = 0.7f,
        .cs = CYAN,
        .exp = 20,
    };
    BRDF ball_specular = GLOSSY_SPECULAR_BRDF(&ball_specular_data, "ball_specular"); 

    Traceable ballTrace = SPHERE_TRACE(&s, CYAN, "br");

    Vector3d min, max;
    ballTrace.get_bounding_extents(&ballTrace, &min, &max);
    AABB bounding = {.min_coord = min, .max_coord = max};
    Traceable aabtrace = AABB_TRACE(&bounding, RED, "cr");
    ballTrace.specular = ball_specular;
    ballTrace.diffuse = ball_diffuse;
    ballTrace.ambient = ball_ambient;
    aabtrace.specular = ball_specular;
    aabtrace.diffuse = ball_diffuse;
    aabtrace.ambient = ball_ambient;

    (void)aabtrace;
    (void)ballTrace;
    Traceable* traceables[20] = 
    {
        &aabtrace,
    };
    int trace_count = 1;
   // (void)ball;
    PointLight pl = 
    {
        .colour = WHITE,
        .ls = 1.0e5f,
        .pos = {100, -350, 50}
    };

    AmbientLight amb = 
    {
        .colour = WHITE,
        .ls = 1.0f
    };
    Light first_pl = POINT_LIGHT(&pl, "fpl");
    Light first_amb = AMBIENT_LIGHT(&amb, "famb");
    Light lights[20] = {first_pl, first_amb};
    int light_count = 1;

    SamplerData data = (SamplerData){
        .sample_count = 4,
        .set_count = 4,
        .used_count = 0,
        .init_index = 0,
        .samples = NULL,
        .shuffled_indices = NULL
    };

    PinholeData p_data = {.d = 100.0};

    CameraData cam_data = (CameraData)
    {
        .eye = {.x =  s.center.x, .y = s.center.y, .z = s.center.z - s.r * 0.99f},
        .look_at = {.x = s.center.x, .y = s.center.y, .z = s.center.z},
        .up = {.x = 0, .y = 1, .z = 0},
        .extra = &p_data,
    };
    Camera pinhole_cam = PINHOLE(cam_data);
    (void)p_data;
    (void)pinhole_cam;

    Sampler jittered = JITTERED_SAMPLER(data);
    SceneData scene_data = {
        .canvas = canvas, 
        .default_colour = BLACK,
        .sampler = &jittered,
        .pixelSize = 1,
        .ray = &r,
        .traceables = &traceables[0], 
        .traceable_count = trace_count,
        .cam = &pinhole_cam,
        .optimized = 1,
        .max_mirror_depth = 0,
    };
    MaterialData mat_data = 
    {
        .ambient_light = &lights[1],
        .lights = &lights[0],
        .light_count = light_count
    };

    Scene scene = {
        .scene_data = &scene_data,
        .ray_trace = ligh_tracer
    };

    scene.ray_trace(&scene_data, &mat_data);
}

void march_test_scene(Canvas* c)
{

    LambertainData ball_diffuse_data = 
    {
        .kd = 1.0f,
        .cd = BLUE
    };
    BRDF ball_diffuse = LAMBERTAIN_BRDF(&ball_diffuse_data, "ball_diffuse"); 

    LambertainData ball_ambient_data = 
    {
        .kd = 0.0f, .cd = CYAN
    };
    BRDF ball_ambient = LAMBERTAIN_BRDF(&ball_ambient_data, "ball_ambient"); 

    GlossySpecularData ball_specular_data = 
    {
        .ks = 0.0f,
        .cs = CYAN,
        .exp = 30,
    };
    BRDF ball_specular = GLOSSY_SPECULAR_BRDF(&ball_specular_data, "ball_specular"); 

    Vector3d rayDirection = {.x = 0, .y = 0, .z = -1};
    Vector3d initPosition = {.x = 0, .y = 0, .z = 100};
    Ray r = {.direction = rayDirection, .base = initPosition};

    PointLight pl = 
    {
        .colour = WHITE,
        .ls = 5.0f,
        .pos = {0, 0, 2.0f}
    };

    AmbientLight amb = 
    {
        .colour = WHITE,
        .ls = 0.2f
    };
    Light first_pl = POINT_LIGHT(&pl, "fpl");
    Light first_amb = AMBIENT_LIGHT(&amb, "famb");
    Light lights[20] = {first_pl, first_amb};
    int light_count = 1;


    SamplerData data = (SamplerData){
        .sample_count = 1,
            .set_count = 1,
            .used_count = 0,
            .init_index = 0,
            .samples = NULL,
            .shuffled_indices = NULL
    };

    Sampler jittered = JITTERED_SAMPLER(data);


    MaterialData mat_data = 
    {
        .ambient_light = &lights[1],
        .lights = &lights[0],
        .light_count = light_count
    };
    
    // SET geometry

    Sphere s = {.center = {0 ,0, 0 + 0.5}, .r = 0.48f};
    Sphere s2 = {.center = {0.0 ,-0.24, 0.24 + 0.5}, .r = 0.3f};
    Sphere s3 = {.center = {0.2 ,0, 0.48f + 0.5}, .r = 0.1f};
    Sphere s4 = {.center = {-0.2 ,0.0, 0.48 + 0.5}, .r = 0.1f};

    Traceable ballTrace = SPHERE_TRACE(&s, BLUE, "br");
    Traceable ball2Trace = SPHERE_TRACE(&s2, BLUE, "br");
    Traceable ball3Trace = SPHERE_TRACE(&s3, BLUE, "br");
    Traceable ball4Trace = SPHERE_TRACE(&s4, BLUE, "br");

    Vector3d min;
    Vector3d max;
    ballTrace.get_bounding_extents(&ballTrace, &min, &max);
    min.z -= 0.48f;
    max.z -= 0.48f;
    AABB bounding = {.min_coord = min, .max_coord = max};
    Traceable aabbTraceable = AABB_TRACE(&bounding, BLUE, "aabb");

     Traceable diff = combine_traceable(&ball2Trace,&ballTrace, diff_sdf);
     Traceable uni = combine_traceable(&diff,&aabbTraceable, union_sdf);
    diff.col = BLUE;
    ballTrace.specular = ball_specular;
    ballTrace.diffuse = ball_diffuse;
    ballTrace.ambient = ball_ambient;
    ball2Trace.specular = ball_specular;
    ball2Trace.diffuse = ball_diffuse;
    ball2Trace.ambient = ball_ambient;
    ball3Trace.specular = ball_specular;
    ball3Trace.diffuse = ball_diffuse;
    ball3Trace.ambient = ball_ambient;
    ball4Trace.specular = ball_specular;
    ball4Trace.diffuse = ball_diffuse;
    ball4Trace.ambient = ball_ambient;
    diff.specular = ball_specular;
    diff.diffuse = ball_diffuse;
    diff.ambient = ball_ambient;
    uni.specular = ball_specular;
    uni.diffuse = ball_diffuse;
    uni.ambient = ball_ambient;
    

    (void)ballTrace;
    (void)ball2Trace;
    (void)ball3Trace;
    (void)ball4Trace;
    (void)diff;
    Traceable* traceables[20] = 
    {
        &uni,
    };
    int trace_count = 1;
   
    // END geometry
    

    PinholeData p_data = {.d = 600};
    OrthographicData o_data = {.dir = {0,0,-1.0}};

    CameraData cam_data = (CameraData)
    {
        .eye = {0, 0, 2.0f},
        .look_at = {0, 0, 0},
        .up = {.x = 0, .y = 1, .z = 0},
        .extra = &p_data,
    };
    Camera pinhole_cam = PINHOLE(cam_data);
    Camera ortho_cam = ORTHO(cam_data);

    (void)p_data;
    (void)pinhole_cam;
    (void)o_data;
    (void)ortho_cam;

    

    SceneData scene_data = {
        .canvas = c, 
        .default_colour = BLACK,
        .sampler = &jittered,
        .pixelSize = 1,
        .ray = &r,
        .traceables = &traceables[0], 
        .traceable_count = trace_count,
        .cam = &pinhole_cam,
        .optimized = 0,
        .max_mirror_depth = 0,
    };
    
    Scene scene = {
        .scene_data = &scene_data,
        .ray_trace = simple_marcher
    };

    init_fbm(0.8f, 2.2f);
    scene.ray_trace(&scene_data, &mat_data);
}
void bake_test(Canvas* c)
{

    LambertainData ball_diffuse_data = 
    {
        .kd = 1.0f,
        .cd = BLUE
    };
    BRDF ball_diffuse = LAMBERTAIN_BRDF(&ball_diffuse_data, "ball_diffuse"); 

    LambertainData ball_ambient_data = 
    {
        .kd = 0.0f, .cd = CYAN
    };
    BRDF ball_ambient = LAMBERTAIN_BRDF(&ball_ambient_data, "ball_ambient"); 

    GlossySpecularData ball_specular_data = 
    {
        .ks = 0.0f,
        .cs = CYAN,
        .exp = 30,
    };
    BRDF ball_specular = GLOSSY_SPECULAR_BRDF(&ball_specular_data, "ball_specular"); 

    Vector3d rayDirection = {.x = 0, .y = 0, .z = -1};
    Vector3d initPosition = {.x = 0, .y = 0, .z = 100};
    Ray r = {.direction = rayDirection, .base = initPosition};

    PointLight pl = 
    {
        .colour = WHITE,
        .ls = 100.0f,
        .pos = {0, 0, 2.0f}
    };

    AmbientLight amb = 
    {
        .colour = WHITE,
        .ls = 0.2f
    };
    Light first_pl = POINT_LIGHT(&pl, "fpl");
    Light first_amb = AMBIENT_LIGHT(&amb, "famb");
    Light lights[20] = {first_pl, first_amb};
    int light_count = 1;


    SamplerData data = (SamplerData){
        .sample_count = 1,
            .set_count = 1,
            .used_count = 0,
            .init_index = 0,
            .samples = NULL,
            .shuffled_indices = NULL
    };

    Sampler jittered = JITTERED_SAMPLER(data);


    MaterialData mat_data = 
    {
        .ambient_light = &lights[1],
        .lights = &lights[0],
        .light_count = light_count
    };
    
    // SET geometry

    f32 shift = -0.1f;
    f32 sr = 0.5f;
    Sphere s = {.center = {0 ,0, 0 + shift}, .r = sr};
    Sphere s2 = {.center = {0.0 ,-sr/2, sr/2 + shift}, .r = sr/2};
    Sphere s3 = {.center = {0.2 ,0, sr + shift}, .r = sr/5};
    Sphere s4 = {.center = {-0.2 ,0.0, sr + shift}, .r = sr/5};

    Traceable ballTrace = SPHERE_TRACE(&s, BLUE, "br");
    Traceable ball2Trace = SPHERE_TRACE(&s2, BLUE, "br");
    Traceable ball3Trace = SPHERE_TRACE(&s3, BLUE, "br");
    Traceable ball4Trace = SPHERE_TRACE(&s4, BLUE, "br");

    Vector3d min;
    Vector3d max;
    ballTrace.get_bounding_extents(&ballTrace, &min, &max);
    min.z -= sr;
    max.z -= sr;
    AABB bounding = {.min_coord = min, .max_coord = max};
    Traceable aabbTraceable = AABB_TRACE(&bounding, BLUE, "aabb");

     Traceable diff = combine_traceable(&ball2Trace,&ballTrace, diff_sdf);
     Traceable uni = combine_traceable(&diff,&aabbTraceable, union_sdf);
    diff.col = BLUE;
    ballTrace.specular = ball_specular;
    ballTrace.diffuse = ball_diffuse;
    ballTrace.ambient = ball_ambient;
    ball2Trace.specular = ball_specular;
    ball2Trace.diffuse = ball_diffuse;
    ball2Trace.ambient = ball_ambient;
    ball3Trace.specular = ball_specular;
    ball3Trace.diffuse = ball_diffuse;
    ball3Trace.ambient = ball_ambient;
    ball4Trace.specular = ball_specular;
    ball4Trace.diffuse = ball_diffuse;
    ball4Trace.ambient = ball_ambient;
    diff.specular = ball_specular;
    diff.diffuse = ball_diffuse;
    diff.ambient = ball_ambient;
    uni.specular = ball_specular;
    uni.diffuse = ball_diffuse;
    uni.ambient = ball_ambient;
    

    (void)ballTrace;
    (void)ball2Trace;
    (void)ball3Trace;
    (void)ball4Trace;
    (void)diff;
    (void)uni;
    Traceable* traceables[20] = 
    {
        &ballTrace,
    };
    int trace_count = 1;
   
    // END geometry
    

    PinholeData p_data = {.d = 600};
    OrthographicData o_data = {.dir = {0,0,-1.0}};

    CameraData cam_data = (CameraData)
    {
        .eye = {0, 0, 1.0f},
        .look_at = {0, 0, 0},
        .up = {.x = 0, .y = 1, .z = 0},
        .extra = &o_data,
    };
    Camera pinhole_cam = PINHOLE(cam_data);
    Camera ortho_cam = ORTHO(cam_data);

    (void)p_data;
    (void)pinhole_cam;
    (void)o_data;
    (void)ortho_cam;

    

    SceneData scene_data = {
        .canvas = c, 
        .default_colour = BLACK,
        .sampler = &jittered,
        .pixelSize = 1,
        .ray = &r,
        .traceables = &traceables[0], 
        .traceable_count = trace_count,
        .cam = &ortho_cam,
        .optimized = 0,
        .max_mirror_depth = 0,
    };
    
    Scene scene = {
        .scene_data = &scene_data,
        .ray_trace = bake_vdm
    };

    init_fbm(0.8f, 2.2f);
    scene.ray_trace(&scene_data, &mat_data);
}
