#version 450 core

in vec4 world_position;
in vec4 normal;
in vec4 vColor;
in vec4 flatNormal;
in vec4 flatColor;

uniform mat4 view;
uniform vec4 color;
uniform int shading_mode;
uniform int lighting_mode;

out vec4 fColor;

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

vec4 phong_shading(vec4 n) {
  int lighting = lighting_mode;

  vec4 specular_term = vec4(0.0);
  if (lighting >= SPECULAR_LIGHT) {
    lighting -= SPECULAR_LIGHT;
    specular_term = specular_light(n);
  }

  vec4 diffuse_term = vec4(0.0);
  if (lighting >= DIFFUSE_LIGHT) {
    lighting -= DIFFUSE_LIGHT;
    diffuse_term = diffuse_light(n);
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
  vec4 c;
  switch (shading_mode)
  {
    case FLAT_SHADING:
      c = flatColor;
      break;

    case PHONG_SHADING:
      c = phong_shading(normal);
      break;

    case FLAT_PHONG_SHADING:
      c = phong_shading(flatNormal);
      break;

    case GOURAUD_SHADING:
    case NO_SHADING:
    default:
      c = vColor;
  }

  fColor = pow(c, vec4(1.0,1.0,1.0, 1.0)/2.2);
}
