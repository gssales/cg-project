#version 400 core

layout (location = 0) in vec4 vertex_coefficients;
layout (location = 1) in vec2 texture_coefficients;

out vec2 texture_coords;

void main()
{
  gl_Position = vertex_coefficients;
  texture_coords = texture_coefficients;
}
