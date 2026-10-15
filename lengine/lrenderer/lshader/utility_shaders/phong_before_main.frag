#define DIRECTIONAL (1)
#define POINT (0)
#define SPOT (2)
layout(location={IN_LOC}) in vec3 lshader_cam_pos;
layout(location={IN_LOC}+1) in vec3 lshader_frag_pos;
layout(location={IN_LOC}+2) in vec3 lshader_frag_normal;

struct Light
{
    vec3 pos;
    float p1;
    vec3 dir;
    float p2;
    vec3 color;
    float p3;
    vec3 attenuation_func;
    float p4;
    int type;
    float angle;
    float penumbra;
};

layout(binding={BINDING}) uniform LSHADER_LIGHT_SETTNGS 
{
    float ka;
    float kd;
    float ks;
    int light_count;
    Light lights[8];
} lshader_light_settings;

layout(binding={BINDING}+1) LSHADER_PHONG_MATERIAL
{
    vec4 ambient_col;
    vec4 diffuse_col;
    vec4 specular_col;
    float shin;
} lshader_phong_material;
