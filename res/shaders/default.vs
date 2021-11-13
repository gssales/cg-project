#version 450 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;

uniform mat4 model_view_proj;
uniform mat4 model;
uniform vec4 color;
uniform mat4 view;
uniform int shading_mode;
uniform bool lighting;

out vec4 world_position;
out vec4 normal;
out vec4 vColor;

const int NO_SHADING = 0;
const int GOURAUD_AD_SHADING = 1;
const int GOURAUD_ADS_SHADING = 2;
const int PHONG_SHADING = 3;

vec4 gouraud_ad_shading() {
  vec4 light_position = inverse(view) * vec4(2.0,2.0,2.0,1.0);

  vec4 ambient_term = color * 0.2;

  vec4 n = normalize(normal);
  vec4 l = normalize(light_position - world_position);
  vec4 lambert_diffuse_term = vec4(0.0);
  if (lighting)
    lambert_diffuse_term = color * max(0, dot(n, l));

  return ambient_term + lambert_diffuse_term;
}

vec4 gouraud_ads_shading() {
  vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 camera_position = inverse(view) * origin;
  vec4 light_position = inverse(view) * vec4(2.0,2.0,2.0,1.0);

  vec4 ambient_term = color * 0.2;

  vec4 n = normalize(normal);
  vec4 l = normalize(light_position - world_position);
  vec4 v = normalize(camera_position - world_position);
  vec4 lambert_diffuse_term = vec4(0.0);
  if (lighting)
    lambert_diffuse_term = color * max(0, dot(n, l));

  vec4 Ks = vec4(0.5, 0.5, 0.5, 1.0);
  float q = 80.0;
  vec4 r = normalize(2 * n * dot(l,n) -l);
  vec4 h = normalize(l + v);
  vec4 specular_term = vec4(0.0);
  if (lighting)
    specular_term  = Ks * pow(max(0, dot(h, r)), q);

  return ambient_term + lambert_diffuse_term + specular_term;
}

vec4 phong_shading() {
  return color;
}

vec4 no_shading() {
  return color;
}

void main()
{
  gl_Position = model_view_proj * model_coefficients;

  world_position = model * model_coefficients;

  normal = inverse(transpose(model)) * normal_coefficients;
  normal.w = 0.0;

  switch (shading_mode)
  {
    case GOURAUD_AD_SHADING:
      vColor = gouraud_ad_shading();
      break;

    case GOURAUD_ADS_SHADING:
      vColor = gouraud_ads_shading();
      break;

    case PHONG_SHADING:
      vColor = phong_shading();
      break;

    case NO_SHADING:
    default:
      vColor = no_shading();
  }
}