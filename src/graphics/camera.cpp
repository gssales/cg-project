#include "graphics/camera.h"

void Camera::MoveLockedCamera()
{
  if (!free)
  {
    float r = distance;
    float y = r*sin(pitch);
    float z = r*cos(pitch)*cos(yaw);
    float x = r*cos(pitch)*sin(yaw);

    position = lookat + glm::vec3(x, y, z);
  }
}

glm::mat4 Camera::Camera_View()
{
  if (free) {
    glm::mat4 view = glm::rotate(glm::mat4(), yaw, glm::vec3(1, 0, 0));
    view = glm::rotate(view, pitch, up_vector);
    return glm::inverse(view);
  }
  else {
    return glm::lookAt(position, lookat, up_vector);
  }
}

glm::mat4 Camera::Camera_Projection()
{
  if (perspective) {
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
  return Camera_View() * Camera_Projection();
}
