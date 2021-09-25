#version 450 core

in vec4 world_position;
in vec4 normal;
in vec4 vColor;

uniform mat4 view;
uniform vec4 color;
uniform int shading_mode;
uniform bool lighting;
uniform int close2gl;

uniform int n_lights;
uniform vec4 light_positions[128];
uniform vec3 light_colors[128];

out vec4 fColor;

const int NO_SHADING = 0;
const int GOURAUD_AD_SHADING = 1;
const int GOURAUD_ADS_SHADING = 2;
const int PHONG_SHADING = 3;

const int USE_OPENGL = 0;
const int USE_CLOSE2GL = 1;

const float M_PI = 3.14159265358979323846;
const float M_PI_2 = 1.57079632679489661923;

vec4 phong_shading()
{
  vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 camera_position = inverse(view) * origin;
  vec4 light_position = inverse(view) * vec4(2.0,2.0,2.0,1.0);
  vec4 p = world_position;
  vec4 n = normalize(normal);

  vec4 Kd = color; // Refletância difusa
  vec4 Ka = color/2.0; // Refletância ambiente
  vec4 Ks = vec4(0.5, 0.5, 0.5, 1.0);
  float q = 80.0;

  vec4 l = normalize(light_position - world_position);
  // Espectro da luz ambiente
  vec4 Ia = vec4(0.2, 0.2, 0.2, 1.0);
  vec4 I;
  if (lighting)
    I = vec4(1.0, 1.0, 1.0, 1.0);
  else
    I = vec4(0.0, 0.0, 0.0, 1.0);

  vec4 lambert_diffuse_term = Kd * I * max(0, dot(n, l));
    
  // Termo ambiente
  vec4 ambient_term = Ka * Ia;
  
  vec4 r = normalize(2 * n * dot(l,n) -l); // Vetor que define o sentido da reflexão especular ideal.
  // Termo especular utilizando o modelo de iluminação de Phong
  vec4 phong_specular_term  = Ks * I * pow(max(0, dot(n, r)), q);

  return ambient_term + lambert_diffuse_term + phong_specular_term;
}

void main()
{
  switch (shading_mode)
  {
  case NO_SHADING:
    fColor = color;
    break;
  case PHONG_SHADING:
    fColor = pow(phong_shading(), vec4(1.0,1.0,1.0, 1.0)/2.2);
    break;
  case GOURAUD_AD_SHADING:
  case GOURAUD_ADS_SHADING:
    fColor = vColor;
    break;
  default:
    fColor = color;
  }
}
