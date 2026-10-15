#version 450
#extension GL_EXT_scalar_block_layout : require
layout(location=0) out vec4 frag_col;

int flt_cmp(float f1, float f2)
{
    float eps = 0.0001f;
    if (abs(f1 - f2) > eps)
    {
        if (f1 > f2)
        {
            return 1;
        }
        else 
        {
            return -1;
        }
    }
    return 0;
}
int flt_cmp_tol(float f1, float f2, float eps)
{
    if (abs(f1 - f2) > eps)
    {
        if (f1 > f2)
        {
            return 1;
        }
        else 
        {
            return -1;
        }
    }
    return 0;
}
float sd_circle(vec2 p, float r, vec2 orig)
{
    return length(p-orig)-r;
}

float sd_box(vec2 p, vec2 orig, vec2 b)
{
    vec2 d = abs(p - orig)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float op_round(in float curr_sdf, in float r)
{
  return curr_sdf - r;
}

float op_union(float a, float b, inout int idx, int idx2)
{
    if (b < a)
    {
        idx = idx2;
        return b;
    }
    return a;
}

layout(binding=0) uniform LSHADER_UI_THEME
{
    vec4 bg;
    vec4 component_bg;
    vec4 shadow_col;
    vec4 click_col;
    vec4 hover_col;
    vec4 fg;
} lshader_ui_theme;
layout(binding=0+1) uniform LSHADER_RES
{
    vec4 res;
} lshader_res;

layout(push_constant) uniform UI_SELECTED_IDX
{
    vec2 mouse_pos;
    int clicked;
} lshader_ui_selected;

layout(std430, binding=2) uniform ui_LSHADER_GLOBAL_ARR
{
float[12] data;
};

void main()
{
vec2 in_uv = (2.0*(gl_FragCoord.xy - lshader_res.res.xy) -lshader_res.res.zw)/lshader_res.res.zw;
vec4 col;
int idx = 0;
float v;
v = 
op_union(
op_union(
op_round(sd_box(in_uv, vec2(data[0+0], data[1+0]), vec2(data[2+0], data[3+0])), 0.04)
,
op_round(sd_box(in_uv, vec2(data[0+4], data[1+4]), vec2(data[2+4], data[3+4])), 0.04)
, idx, 1
)
,
op_round(sd_box(in_uv, vec2(data[0+8], data[1+8]), vec2(data[2+8], data[3+8])), 0.04)
, idx, 2
)
;

float gv = v;
int gidx = idx;
in_uv = lshader_ui_selected.mouse_pos;
idx = 0;
v = 1.;
v = 
op_union(
op_union(
op_round(sd_box(in_uv, vec2(data[0+0], data[1+0]), vec2(data[2+0], data[3+0])), 0.04)
,
op_round(sd_box(in_uv, vec2(data[0+4], data[1+4]), vec2(data[2+4], data[3+4])), 0.04)
, idx, 1
)
,
op_round(sd_box(in_uv, vec2(data[0+8], data[1+8]), vec2(data[2+8], data[3+8])), 0.04)
, idx, 2
)
;

float tol = 0.005f;
int cond = flt_cmp_tol(gv, 0., tol);
if (cond == -1)
{
    if (gidx == idx && flt_cmp_tol(v, 0., tol) == -1)
    {
        if (lshader_ui_selected.clicked == 1)
        {
            col = lshader_ui_theme.click_col;
        }
        else 
        {
            col = lshader_ui_theme.hover_col;
        }
    }
    else 
    {
        col = lshader_ui_theme.component_bg;
    }
}
else if (cond == 0)
{
    float t = abs(gv)/tol;
    col = (1.-t) * lshader_ui_theme.shadow_col + t * lshader_ui_theme.bg;
}
else 
{
    col = lshader_ui_theme.bg;
}
frag_col = col;

}