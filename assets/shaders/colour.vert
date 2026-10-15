#version 450
layout(location=0)in vec4 lshader_in_position;
layout(push_constant) uniform SHADER_MODEL_VIEW
{
    mat4 model;
    mat4 view;
} shader_model_view;
layout(binding=0) uniform SHADER_PROJECTION
{
    mat4 proj;
} shader_proj;

void main()
{
gl_Position = (shader_proj.proj * shader_model_view.view * shader_model_view.model) * lshader_in_position;

}