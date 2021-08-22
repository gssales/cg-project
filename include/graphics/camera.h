#ifndef _CAMERA_H
#define _CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Camera
{
public:
  glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
  float yaw = 0.0f, pitch = 0.0f;

  bool free = true; 

  glm::vec3 lookat = glm::vec3(0.0f,0.0f,0.0f);;
  float distance = 1.0f;

  glm::vec3 up_vector = glm::vec3(0.0f,1.0f,0.0f);

  bool perspective = true;
  float field_of_view = 3.141592 / 3.0f;
  float nearplane = -0.1f, farplane = -100.0f;
  float screen_ratio = 1;

  glm::mat4 Camera_ViewProj();
  glm::mat4 Camera_View();
  glm::mat4 Camera_Projection();
  void MoveLockedCamera();
};


#endif // _CAMERA_H
