#version 450

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColour;
layout(location = 0) out vec3 frag_col;

layout(push_constant) uniform pushConstant
{
    layout  (offset   =    16)
    mat4 model;
    mat4 view_proj;
} ps;

void main() {
    gl_Position = ps.view_proj * ps.model * inPosition;
    frag_col = inColour;
}

