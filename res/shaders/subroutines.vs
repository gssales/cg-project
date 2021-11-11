#version 450 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;

uniform mat4 model_view_proj;
uniform mat4 model;
uniform vec4 color;
uniform mat4 view;
uniform bool lighting;

out vec4 world_position;
out vec4 normal;
out vec4 vColor;

subroutine vec4 shading(); 

layout(index=0) subroutine(shading) vec4 gouraud_ad() {
  vec4 light_position = inverse(view) * vec4(2.0,2.0,2.0,1.0);

  // vec4 ambient_term = color * 0.2;
  vec4 ambient_term = vec4(1.0, 0.0, 0.0, 1.0) * 0.2;

  vec4 n = normalize(normal);
  vec4 l = normalize(light_position - world_position);
  vec4 lambert_diffuse_term = vec4(0.0);
  if (lighting)
    lambert_diffuse_term = color * max(0, dot(n, l));

  return ambient_term + lambert_diffuse_term;
}

layout(index=1) subroutine(shading) vec4 gouraud_ads() {
  vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 camera_position = inverse(view) * origin;
  vec4 light_position = inverse(view) * vec4(2.0,2.0,2.0,1.0);

  // vec4 ambient_term = color * 0.2;
  vec4 ambient_term = vec4(0.0, 1.0, 0.0, 1.0) * 0.2;

  vec4 n = normalize(normal);
  vec4 l = normalize(light_position - world_position);
  vec4 lambert_diffuse_term = vec4(0.0);
  if (lighting)
    lambert_diffuse_term = color * max(0, dot(n, l));

  vec4 Ks = vec4(0.5, 0.5, 0.5, 1.0);
  float q = 80.0;
  vec4 reflex = normalize(2 * n * dot(l,n) -l);
  vec4 specular_term = vec4(0.0);
  if (lighting)
    specular_term  = Ks * pow(max(0, dot(n, reflex)), q);

  return ambient_term + lambert_diffuse_term + specular_term;
}

layout(index=2) subroutine(shading) vec4 phong() {
  return color;
}

layout(index=3) subroutine(shading) vec4 no_shading() {
  return color;
}

subroutine uniform shading shading_mode;

void main()
{
  gl_Position = model_view_proj * model_coefficients;

  world_position = model * model_coefficients;

  normal = inverse(transpose(model)) * normal_coefficients;
  normal.w = 0.0;

  vColor = shading_mode();
}