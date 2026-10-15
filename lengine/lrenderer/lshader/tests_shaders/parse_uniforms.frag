#version 460

layout (

        binding = 5 ) uniform TIME
{
    vec2 dim;
    float time;
    float rand;
} stuff;

layout(

        push_constant


        ) uniform pushConstant
{
    vec2 dim;
    float time;
} uniforms;

void main()
{
}
