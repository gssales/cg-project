
#version 400 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;

uniform mat4 model_view_proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = model_view_proj * model_coefficients;
}
