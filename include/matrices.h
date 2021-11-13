#ifndef _MATRICES_H
#define _MATRICES_H

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace matrices
{
  glm::vec4 to_vec4(glm::vec3 v, float w);
  
  float length(glm::vec3 v);
  float dot(glm::vec3 u, glm::vec3 v);
  glm::vec3 cross(glm::vec3 u, glm::vec3 v);
  glm::vec3 normalized(glm::vec3 v);

  glm::vec3 cw_surface_normal(glm::vec3 u, glm::vec3 v);
  glm::vec4 cw_surface_normal(glm::vec4 u, glm::vec4 v);

  glm::mat4 mat4(
    float m00, float m01, float m02, float m03, // LINHA 1
    float m10, float m11, float m12, float m13, // LINHA 2
    float m20, float m21, float m22, float m23, // LINHA 3
    float m30, float m31, float m32, float m33  // LINHA 4
  );

  glm::mat4 translate(float dx, float dy, float dz);
  glm::mat4 translate(glm::vec3 d_vec);

  glm::mat4 scale(float sx, float sy, float sz);
  glm::mat4 scale(glm::vec3 s_vec);
  glm::mat4 scale(float s);

  glm::mat4 rotate_x(float angle);
  glm::mat4 rotate_y(float angle);
  glm::mat4 rotate_z(float angle);
  glm::mat4 rotate(float angle, glm::vec3 axis);

  glm::mat4 view_matrix(glm::vec4 position_c, glm::vec4 view_vector,
                        glm::vec4 up_vector);
  glm::mat4 ortho(float l, float r, float b, float t, float n,
                  float f);
  glm::mat4 perspective(float field_of_view, float aspect, float n,
                        float f);
  glm::mat4 perspective(float h_fov, float v_fov, float aspect, float n,
                        float f);
  glm::mat4 viewport(float lv, float rv, float bv, float tv);
                        
}

void PrintMatrix(glm::mat4 M);
void PrintVec3(glm::vec3 v);
void PrintVec4(glm::vec4 v);

#endif // _MATRICES_H