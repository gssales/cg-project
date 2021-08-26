#include "graphics/camera.h"

void Camera::Update()
{
  glm::vec3 target;
  if (camera_view == LOOK_FREE) 
  {
    target = position + view_vector;
  }
  if (camera_view == LOOK_AT)
  {
    target = lookat;
  }
    
  camera_space = glm::lookAt(position, target, up_vector);
}

glm::mat4 Camera::Camera_View()
{
  if (camera_view == LOOK_FREE) {
    return glm::lookAt(position, position + view_vector, up_vector);
  }
  else {
    return glm::lookAt(position, lookat, up_vector);
  }
}

glm::mat4 Camera::Camera_Projection()
{
  if (projection == PERSPECTIVE) 
  {
    return glm::perspective(field_of_view, screen_ratio, nearplane, farplane);
  }
  else {
    float t = 1.5f * distance / 2.5f;
    float b = -t;
    float r = t*screen_ratio;
    float l = -r;
    return glm::ortho(l, r, b, t);
  }
}

glm::mat4 Camera::Camera_ViewProj()
{
  return Camera_Projection() * Camera_View();
}

void Camera::Rotate(float pitch, float yaw)
{
  total_pitch = glm::clamp(total_pitch + pitch, min_pitch, max_pitch);
  total_yaw += yaw;
  view_vector = glm::rotate(total_yaw, Y_AXIS) * glm::rotate(total_pitch, X_AXIS) * FORWARD4;
}

void Camera::Move(glm::vec3 move)
{
  position += move;
}
