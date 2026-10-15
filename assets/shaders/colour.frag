#version 450
layout(location=0) out vec4 frag_col;
layout(binding=1) uniform LSHADER_COL
{
vec4 col;

} lshader_col;
void main()
{
frag_col = vec4(255./255., 186./255., 222./255., 1.);
frag_col = lshader_col.col;

}