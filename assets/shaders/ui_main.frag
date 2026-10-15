vec2 in_uv = (2.0*(gl_FragCoord.xy - lshader_res.res.xy) -lshader_res.res.zw)/lshader_res.res.zw;
vec4 col;
int idx = 0;
float v;
{INJECT}
float gv = v;
int gidx = idx;
in_uv = lshader_ui_selected.mouse_pos;
idx = 0;
v = 1.;
{INJECT}
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
