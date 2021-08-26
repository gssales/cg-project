#ifndef _CAMERA_H
#define _CAMERA_H

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

enum Projection { PERSPECTIVE, ORTHOGRAPHIC };
enum CameraView { LOOK_FREE, LOOK_AT };
enum CameraMovement { MOVE_FREE, LOCKED };

#define X_AXIS    glm::vec3( 1.0f, 0.0f, 0.0f)
#define Y_AXIS    glm::vec3( 0.0f, 1.0f, 0.0f)
#define Z_AXIS    glm::vec3( 0.0f, 0.0f, 1.0f)
#define UP        glm::vec3( 0.0f, 1.0f, 0.0f)
#define DOWN      glm::vec3( 0.0f,-1.0f, 0.0f)
#define FORWARD   glm::vec3( 0.0f, 0.0f,-1.0f)
#define BACKWARD  glm::vec3( 0.0f, 0.0f, 1.0f)
#define RIGHT     glm::vec3( 1.0f, 0.0f, 0.0f)
#define LEFT      glm::vec3(-1.0f, 0.0f, 0.0f)
#define ORIGIN    glm::vec3( 0.0f, 0.0f, 0.0f)
#define UP4        glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f)
#define DOWN4      glm::vec4( 0.0f,-1.0f, 0.0f, 0.0f)
#define FORWARD4   glm::vec4( 0.0f, 0.0f,-1.0f, 0.0f)
#define BACKWARD4  glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f)
#define RIGHT4     glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f)
#define LEFT4      glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f)
#define ORIGIN4    glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f)
#define PI 3.141592f

class Camera
{
public:
  glm::mat4 camera_space = glm::mat4x4(1.0f);
  glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);
  glm::vec3 up_vector = UP;
  glm::vec3 view_vector = FORWARD;
  float total_pitch = 0.0f, total_yaw = 0.0f;
  float max_pitch = PI/2, min_pitch = -PI/2;

  CameraView camera_view = LOOK_FREE; 
  glm::vec3 lookat = glm::vec3(0.0f,0.0f,0.0f);

  CameraMovement camera_movement = MOVE_FREE; 
  float distance = 1.0f;

  Projection projection = PERSPECTIVE; 
  float field_of_view = 3.141592 / 3.0f;
  float nearplane = 0.1f, farplane = 100.0f;
  float screen_ratio = 1;

  void Update();
  glm::mat4 Camera_ViewProj();
  glm::mat4 Camera_View();
  glm::mat4 Camera_Projection();
  void Rotate(float pitch, float yaw);
  void Move(glm::vec3 move);
};


#endif // _CAMERA_H
