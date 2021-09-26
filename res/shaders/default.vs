
#version 400 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec4 projected_coefficients;

uniform mat4 model_view_proj;
uniform mat4 model;
uniform mat4 view;
uniform vec4 color;
uniform int shading_mode;
uniform bool lighting;
uniform int close2gl;

out vec4 world_position;
out vec4 normal;
out vec4 vColor;

const int NO_SHADING = 0;
const int GOURAUD_AD_SHADING = 1;
const int GOURAUD_ADS_SHADING = 2;
const int PHONG_SHADING = 3;

const int USE_OPENGL = 0;
const int USE_CLOSE2GL = 1;

vec4 gouraud_ad_shading(vec4 normal)
{
  vec4 n = normalize(normal);

  vec4 Kd = color; // Refletância difusa
  vec4 Ka = color/2.0; // Refletância ambiente

  // Espectro da luz ambiente
  vec4 l = normalize(vec4(0.2,1.0,-0.5,0.0));
  vec4 Ia = vec4(0.2, 0.2, 0.2, 1.0);
  vec4 I;
  if (lighting)
    I = vec4(1.0, 1.0, 1.0, 1.0);
  else
    I = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 lambert_diffuse_term = Kd * I * max(0, dot(n, l));
    
  // Termo ambiente
  vec4 ambient_term = Ka * Ia;

  return ambient_term + lambert_diffuse_term;
}

vec4 gouraud_ads_shading(vec4 normal, vec4 world_position)
{
  vec4 ad_shading = gouraud_ad_shading(normal);

  vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 camera_position = inverse(view) * origin;
  vec4 p = world_position;

  vec4 n = normalize(normal);

  vec4 Ks = vec4(0.5, 0.5, 0.5, 1.0);
  float q = 80.0;

  vec4 l = normalize(vec4(0.2,1.0,-0.5,0.0)); // Vetor da luz
  // Espectro da luz ambiente
  vec4 I;
  if (lighting)
    I = vec4(1.0, 1.0, 1.0, 1.0);
  else
    I = vec4(0.0, 0.0, 0.0, 1.0);
  
  vec4 r = normalize(2 * n * dot(l,n) -l); // Vetor que define o sentido da reflexão especular ideal.
  // Termo especular utilizando o modelo de iluminação de Phong
  vec4 phong_specular_term  = Ks * I * pow(max(0, dot(n, r)), q);

  return ad_shading + phong_specular_term;
}

void main()
{
  if (close2gl == USE_CLOSE2GL)
    gl_Position = projected_coefficients;
  else
    gl_Position = model_view_proj * model_coefficients;

  world_position = model * model_coefficients;

  normal = inverse(transpose(model)) * normal_coefficients;
  normal.w = 0.0;

  switch (shading_mode)
  {
  case GOURAUD_AD_SHADING:
    vColor = pow(gouraud_ad_shading(normal), vec4(1.0,1.0,1.0, 1.0)/2.2);
    break;
  case GOURAUD_ADS_SHADING:
    vColor = pow(gouraud_ads_shading(normal, world_position), vec4(1.0,1.0,1.0, 1.0)/2.2);
    break;
  case NO_SHADING:
  case PHONG_SHADING:
  default:
    vColor = color;
  }
}
