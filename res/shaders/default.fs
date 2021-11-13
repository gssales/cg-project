#version 450 core

in vec4 world_position;
in vec4 normal;
in vec4 vColor;

uniform mat4 view;
uniform vec4 color;
uniform int shading_mode;
uniform bool lighting;

out vec4 fColor;

const int NO_SHADING = 0;
const int GOURAUD_AD_SHADING = 1;
const int GOURAUD_ADS_SHADING = 2;
const int PHONG_SHADING = 3;

vec4 gouraud_ad_shading() {
  return vColor;
}

vec4 gouraud_ads_shading() {
  return vColor;
}

vec4 phong_shading() {
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
  float q = 240.0;
  vec4 r = normalize(2 * n * dot(l,n) -l);
  vec4 h = normalize(l + v);
  vec4 specular_term = vec4(0.0);
  if (lighting)
    specular_term  = Ks * pow(max(0, dot(h, r)), q);

  return ambient_term + lambert_diffuse_term + specular_term;
}

vec4 no_shading() {
  return color;
}

void main()
{
  vec4 c;
  switch (shading_mode)
  {
    case GOURAUD_AD_SHADING:
      c = gouraud_ad_shading();
      break;

    case GOURAUD_ADS_SHADING:
      c = gouraud_ads_shading();
      break;

    case PHONG_SHADING:
      c = phong_shading();
      break;

    case NO_SHADING:
    default:
      c = no_shading();
  }

  fColor = pow(c, vec4(1.0,1.0,1.0, 1.0)/2.2);
}
