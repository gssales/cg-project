#version 450 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec4 surface_normal_coefficients;
layout (location = 3) in vec2 texture_coefficients;

uniform mat4 model_view_proj;
uniform mat4 model;
uniform mat4 view;
uniform vec4 color;
uniform int shading_mode;
uniform int lighting_mode;
uniform bool has_texture;

out vec4 world_position;
out vec4 normal;
out vec4 vColor;
flat out vec4 flatNormal;
flat out vec4 flatColor;

flat out vec4 flatAmbientColor;
flat out vec4 flatDiffuseColor;
flat out vec4 flatSpecularColor;
out vec4 vColorAmbient;
out vec4 vColorDiffuse;
out vec4 vColorSpecular;

out vec2 texture_coords;

const int NO_SHADING = 0;
const int FLAT_SHADING = 1;
const int GOURAUD_SHADING = 2;
const int PHONG_SHADING = 3;
const int FLAT_PHONG_SHADING = 4;

const int AMBIENT_LIGHT = 1;
const int DIFFUSE_LIGHT = 2;
const int SPECULAR_LIGHT = 4;

vec4 ambient_light() {
  return color * 0.2;
}

vec4 diffuse_light(vec4 normal) {
  vec4 light_position = inverse(view) * vec4(2.0,2.0,2.0,1.0);

  vec4 n = normalize(normal);
  vec4 l = normalize(light_position - world_position);
  return color * max(0, dot(l, n));
}

vec4 specular_light(vec4 normal) {
  vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 camera_position = inverse(view) * origin;
  vec4 light_position = inverse(view) * vec4(2.0,2.0,2.0,1.0);

  vec4 Ks = vec4(0.5, 0.5, 0.5, 1.0);
  float q = 80.0;

  vec4 n = normalize(normal);
  vec4 l = normalize(light_position - world_position);
  vec4 r = normalize(2 * n * dot(l,n) -l);

  vec4 v = normalize(camera_position - world_position);
  vec4 h = normalize(l + v);
  return Ks * pow(max(0, dot(h, r)), q);
}

void flat_shading_with_texture_mapping() {
  int lighting = lighting_mode;
  vec4 normal = inverse(transpose(model)) * surface_normal_coefficients;
  normal.w = 0.0;

  vec4 specular_term = vec4(0.0);
  if (lighting >= SPECULAR_LIGHT) {
    lighting -= SPECULAR_LIGHT;
    specular_term = specular_light(normal);
  }

  vec4 diffuse_term = vec4(0.0);
  if (lighting >= DIFFUSE_LIGHT) {
    lighting -= DIFFUSE_LIGHT;
    diffuse_term = diffuse_light(normal)/color;
  }
  
  vec4 ambient_term = vec4(0.0);
  if (lighting >= AMBIENT_LIGHT) {
    lighting -= AMBIENT_LIGHT;
    ambient_term = vec4(0.2);
  }

  flatAmbientColor = ambient_term;
  flatDiffuseColor = diffuse_term;
  flatSpecularColor = specular_term;
}

void gouraud_shading_with_texture_mapping() {
  int lighting = lighting_mode;
  vec4 normal = inverse(transpose(model)) * normal_coefficients;
  normal.w = 0.0;

  vec4 specular_term = vec4(0.0);
  if (lighting >= SPECULAR_LIGHT) {
    lighting -= SPECULAR_LIGHT;
    specular_term = specular_light(normal);
  }

  vec4 diffuse_term = vec4(0.0);
  if (lighting >= DIFFUSE_LIGHT) {
    lighting -= DIFFUSE_LIGHT;
    diffuse_term = diffuse_light(normal)/color;
  }
  
  vec4 ambient_term = vec4(0.0);
  if (lighting >= AMBIENT_LIGHT) {
    lighting -= AMBIENT_LIGHT;
    ambient_term = vec4(0.2);
  }

  vColorAmbient = ambient_term;
  vColorDiffuse = diffuse_term;
  vColorSpecular = specular_term;
}

vec4 flat_shading() {
  int lighting = lighting_mode;
  vec4 normal = inverse(transpose(model)) * surface_normal_coefficients;
  normal.w = 0.0;

  vec4 specular_term = vec4(0.0);
  if (lighting >= SPECULAR_LIGHT) {
    lighting -= SPECULAR_LIGHT;
    specular_term = specular_light(normal);
  }

  vec4 diffuse_term = vec4(0.0);
  if (lighting >= DIFFUSE_LIGHT) {
    lighting -= DIFFUSE_LIGHT;
    diffuse_term = diffuse_light(normal);
  }
  
  vec4 ambient_term = vec4(0.0);
  if (lighting >= AMBIENT_LIGHT) {
    lighting -= AMBIENT_LIGHT;
    ambient_term = ambient_light();
  }

  return ambient_term + diffuse_term + specular_term;
}

vec4 gouraud_shading() {
  int lighting = lighting_mode;
  vec4 normal = inverse(transpose(model)) * normal_coefficients;
  normal.w = 0.0;

  vec4 specular_term = vec4(0.0);
  if (lighting >= SPECULAR_LIGHT) {
    lighting -= SPECULAR_LIGHT;
    specular_term = specular_light(normal);
  }

  vec4 diffuse_term = vec4(0.0);
  if (lighting >= DIFFUSE_LIGHT) {
    lighting -= DIFFUSE_LIGHT;
    diffuse_term = diffuse_light(normal);
  }
  
  vec4 ambient_term = vec4(0.0);
  if (lighting >= AMBIENT_LIGHT) {
    lighting -= AMBIENT_LIGHT;
    ambient_term = ambient_light();
  }

  return ambient_term + diffuse_term + specular_term;
}

void main()
{
  gl_Position = model_view_proj * model_coefficients;

  world_position = model * model_coefficients;

  normal = inverse(transpose(model)) * normal_coefficients;
  normal.w = 0.0;

  flatNormal = inverse(transpose(model)) * surface_normal_coefficients;
  flatNormal.w = 0.0;

  if (has_texture)
    texture_coords = texture_coefficients;

  switch (shading_mode)
  {
    case FLAT_SHADING:
      if (has_texture)
        flat_shading_with_texture_mapping();
      else
        flatColor = flat_shading();
      break;

    case GOURAUD_SHADING:
      if (has_texture)
        gouraud_shading_with_texture_mapping();
      else
        vColor = gouraud_shading();
      break;

    case PHONG_SHADING:
    case FLAT_PHONG_SHADING:
    case NO_SHADING:
    default:
      vColor = color;
  }
}