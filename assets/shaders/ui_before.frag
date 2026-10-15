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

layout(binding={BINDING}) uniform LSHADER_UI_THEME
{
    vec4 bg;
    vec4 component_bg;
    vec4 shadow_col;
    vec4 click_col;
    vec4 hover_col;
    vec4 fg;
} lshader_ui_theme;
layout(binding={BINDING}+1) uniform LSHADER_RES
{
    vec4 res;
} lshader_res;

layout(push_constant) uniform UI_SELECTED_IDX
{
    vec2 mouse_pos;
    int clicked;
} lshader_ui_selected;
