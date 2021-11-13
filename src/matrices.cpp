#include "matrices.h"

glm::vec4 matrices::to_vec4(glm::vec3 v, float w)
{
  return glm::vec4(v.x, v.y, v.z, w);
}

float matrices::length(glm::vec3 v)
{
  return sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
}

float matrices::dot(glm::vec3 u, glm::vec3 v)
{
  return u.x*v.x + u.y*v.y + u.z*v.z;
}

glm::vec3 matrices::cross(glm::vec3 u, glm::vec3 v)
{
  return glm::vec3(
    u.y * v.z - u.z * v.y,
    u.z * v.x - u.x * v.z,
    u.x * v.y - u.y * v.x
  );
}

glm::vec3 matrices::normalized(glm::vec3 v)
{
  if (length(v) > 0)
    return v / length(v);
  else 
    return glm::vec3(0.0f);
}

glm::vec3 matrices::cw_surface_normal(glm::vec3 u, glm::vec3 v)
{
  glm::vec3 n;
  n.x = u.y*v.z - u.z*v.y;
  n.y = u.z*v.x - u.x*v.z;
  n.z = u.x*v.y - u.y*v.x;
  return n;
}

glm::vec4 matrices::cw_surface_normal(glm::vec4 u, glm::vec4 v)
{
  glm::vec4 n;
  n.x = u.y*v.z - u.z*v.y;
  n.y = u.z*v.x - u.x*v.z;
  n.z = u.x*v.y - u.y*v.x;
  n.w = 0.0;
  return n;
}

glm::mat4 matrices::mat4(
    float m00, float m01, float m02, float m03, // LINHA 1
    float m10, float m11, float m12, float m13, // LINHA 2
    float m20, float m21, float m22, float m23, // LINHA 3
    float m30, float m31, float m32, float m33  // LINHA 4
)
{
    return glm::mat4(
        m00, m10, m20, m30, // COLUNA 1
        m01, m11, m21, m31, // COLUNA 2
        m02, m12, m22, m32, // COLUNA 3
        m03, m13, m23, m33  // COLUNA 4
    );
}

glm::mat4 matrices::translate(float dx, float dy, float dz)
{
  return mat4(
    1.0f , 0.0f , 0.0f , dx ,
    0.0f , 1.0f , 0.0f , dy ,
    0.0f , 0.0f , 1.0f , dz ,
    0.0f , 0.0f , 0.0f , 1.0f
  );
}
glm::mat4 matrices::translate(glm::vec3 d_vec)
{
  return translate(d_vec.x, d_vec.y, d_vec.z);
}

glm::mat4 matrices::scale(float sx, float sy, float sz)
{
  return mat4(
      sx , 0.0f , 0.0f , 0.0f ,
    0.0f ,   sy , 0.0f , 0.0f ,
    0.0f , 0.0f ,   sz , 0.0f ,
    0.0f , 0.0f , 0.0f , 1.0f
  );
}
glm::mat4 matrices::scale(glm::vec3 s_vec)
{
  return scale(s_vec.x, s_vec.y, s_vec.z);
}
glm::mat4 matrices::scale(float s)
{
  return scale(s, s, s);
}

glm::mat4 matrices::rotate_x(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  return mat4(
    1.0f , 0.0f , 0.0f , 0.0f ,
    0.0f ,    c ,   -s , 0.0f ,
    0.0f ,    s ,    c , 0.0f ,
    0.0f , 0.0f , 0.0f , 1.0f
  );
}
glm::mat4 matrices::rotate_y(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  return mat4(
        c , 0.0f ,    s , 0.0f ,
    0.0f , 1.0f , 0.0f , 0.0f ,
      -s , 0.0f ,    c , 0.0f ,
    0.0f , 0.0f , 0.0f , 1.0f
  );
}
glm::mat4 matrices::rotate_z(float angle)
{
  float c = cos(angle);
  float s = sin(angle);
  return mat4(
        c ,   -s , 0.0f , 0.0f ,
        s ,    c , 0.0f , 0.0f ,
    0.0f , 0.0f , 1.0f , 0.0f ,
    0.0f , 0.0f , 0.0f , 1.0f
  );
}
glm::mat4 matrices::rotate(float angle, glm::vec3 axis)
{
  float c = cos(angle);
  float s = sin(angle);

  glm::vec3 v = normalized(axis);

  return mat4(
    v.x*v.x*(1-c) +c     , v.x*v.y*(1-c) -v.z*s , v.x*v.z*(1-c) +v.y*s , 0.0f ,
    v.x*v.y*(1-c) +v.z*s , v.y*v.y*(1-c) +c     , v.y*v.z*(1-c) -v.x*s , 0.0f ,
    v.x*v.z*(1-c) -v.y*s , v.y*v.z*(1-c) +v.x*s , v.z*v.z*(1-c) +c     , 0.0f ,
    0.0f                 , 0.0f                 , 0.0f                 , 1.0f
  );
}

glm::mat4 matrices::view_matrix(glm::vec4 position_c, glm::vec4 view_vector,
                      glm::vec4 up_vector)
{
  glm::vec3 w = -view_vector;
  glm::vec3 u = cross(up_vector, w);

  w = w / length(w);
  u = u / length(u);

  glm::vec3 v = cross(w,u);

  glm::vec4 origin_o = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec3 vec_c = position_c - origin_o;

  return mat4(
    u.x , u.y , u.z , dot(-u, vec_c) ,
    v.x , v.y , v.z , dot(-v, vec_c) ,
    w.x , w.y , w.z , dot(-w, vec_c) ,
    0.0f , 0.0f , 0.0f , 1.0f
  );
}
glm::mat4 matrices::ortho(float l, float r, float b, float t, float n,
                float f)
{
  return mat4(
    2/(r-l) , 0.0f    , 0.0f    , -(r+l) / (r-l) ,
    0.0f    , 2/(t-b) , 0.0f    , -(t+b) / (t-b) ,
    0.0f    , 0.0f    , 2/(f-n) , -(f+n) / (f-n) ,
    0.0f    , 0.0f    , 0.0f    , 1.0f
  );
}
glm::mat4 matrices::perspective(float field_of_view, float aspect, float n,
                      float f)
{
  float t = fabs(n) * tanf(field_of_view / 2.0f);
  float b = -t;
  float r = t * aspect;
  float l = -r;

  return mat4(
    (2.0f*n)/(r-l), 0.0f,          (r+l)/(r-l),   0.0f,
     0.0f,         (2.0f*n)/(t-b), (t+b)/(t-b),   0.0f,
     0.0f,          0.0f,         -(f+n)/(f-n), -(2.0f*f*n)/(f-n),
     0.0f,          0.0f,          -1.0f,         0.0f
  );
}
glm::mat4 matrices::perspective(float h_fov, float v_fov, float aspect, float n,
                      float f)
{
  float t = fabs(n) * tanf(v_fov / 2.0f);
  float b = -t;
  float r = fabs(n) * tanf(h_fov / 2.0f) * aspect;
  float l = -r;

  return mat4(
    (2.0f*n)/(r-l), 0.0f,          (r+l)/(r-l),   0.0f,
     0.0f,         (2.0f*n)/(t-b), (t+b)/(t-b),   0.0f,
     0.0f,          0.0f,         -(f+n)/(f-n), -(2.0f*f*n)/(f-n),
     0.0f,          0.0f,          -1.0f,         0.0f
  );
}

glm::mat4 matrices::viewport(float lv, float tv, float rv, float bv)
{
  return mat4(
    (rv-lv)/2.0f, 0.0f,       0.0f, (rv+tv)/2.0f,
     0.0f,     (tv-bv)/2.0f, 0.0f, (tv+bv)/2.0f,
     0.0f,      0.0f,       1.0f, 0.0f,
     0.0f,      0.0f,       0.0f, 1.0f
  );  
}

void PrintMatrix(glm::mat4 M)
{
  printf("\n");
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][0], M[1][0], M[2][0], M[3][0]);
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][1], M[1][1], M[2][1], M[3][1]);
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][2], M[1][2], M[2][2], M[3][2]);
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][3], M[1][3], M[2][3], M[3][3]);
}

void PrintVec3(glm::vec3 v)
{
  printf("\n");
  printf("[ %+0.2f  %+0.2f  %+0.2f ]\n", v.x, v.y, v.z);
}

void PrintVec4(glm::vec4 v)
{
  printf("\n");
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n", v.x, v.y, v.z, v.w);
}