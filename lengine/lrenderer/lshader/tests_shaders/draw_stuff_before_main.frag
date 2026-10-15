float sdCircle(vec2 p, float r, vec2 offset)
{
    return length(p-offset)-r;
}
float sdBox(vec2 p, vec2 b)
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}
vec4 draw_circle(vec2 p, vec2 center, float r)
{
    float v = sdCircle(p, r, center);
    {DRAW}
}
vec4 draw_box(vec2 p, vec2 center, float r)
{
    float v = sdBox(p - center, r);
    {DRAW}
}
