#version 450 core

uniform sampler2D TextureImage0;

in vec2 texture_coords;
out vec4 fColor;

void main()
{
  fColor = texture(TextureImage0, texture_coords).rgba;
}
