#include "graphics/camera.h"

using namespace matrices;

void Camera::Update()
{
  if (camera_view == LOOK_AT)
  {
    view_vector = lookat - position;
  }
    
  view_vector = normalized(view_vector);
  camera_space = view_matrix(
      to_vec4(position, 1.0f), 
      to_vec4(view_vector, 0.0f), 
      to_vec4(up_vector, 0.0f));
}

glm::mat4 Camera::Camera_View()
{
  if (camera_view == LOOK_FREE) {
    return view_matrix(
        to_vec4(position, 1.0f), 
        to_vec4(view_vector, 0.0f), 
        to_vec4(up_vector, 0.0f));
  }
  else {
    return view_matrix(
        to_vec4(position, 1.0f), 
        to_vec4(normalized(lookat - position), 0.0f), 
        to_vec4(up_vector, 0.0f));
  }
}

glm::mat4 Camera::Camera_Projection()
{
  if (projection == PERSPECTIVE) 
  {
    return perspective(h_fov, v_fov, screen_ratio, nearplane, farplane);
  }
  else {
    float t = 1.5f * distance / 2.5f;
    float b = -t;
    float r = t*screen_ratio;
    float l = -r;
    return ortho(l, r, b, t, nearplane, farplane);
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
  view_vector = rotate_y(total_yaw) * rotate_x(total_pitch) * FORWARD4;
}

void Camera::Rotate(glm::vec3 axis, float angle)
{
  view_vector = rotate(angle, axis) * to_vec4(view_vector, 0.0f);
  up_vector = rotate(angle, axis) * to_vec4(up_vector, 0.0f);
}

void Camera::Move(glm::vec3 move)
{
  // PrintVec3(move);
  position += move;
}
