#version 450 core

in vec4 normal;

uniform mat4 view;
uniform vec4 color;

out vec4 fColor;

void main()
{
  vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 camera_position = inverse(view) * origin;
  
  vec4 n = normalize(normal);

  vec4 Kd = color; // Refletância difusa
  vec4 Ka = color/2.0; // Refletância ambiente
  
  // Espectro da luz ambiente
  vec4 l = vec4(0.7,-1.0,0.3,0.0);
  vec4 Ia = vec4(0.2, 0.2, 0.2, 1.0);
  vec4 I = vec4(0.5, 0.5, 0.5, 1.0);
  vec4 lambert_diffuse_term = Kd * I * max(0, dot(n, l));
    
  // Termo ambiente
  vec4 ambient_term = Ka * Ia;
  
  fColor = lambert_diffuse_term + ambient_term;

  fColor = pow(fColor, vec4(1.0,1.0,1.0, 1.0)/2.2);
}
