#ifndef _SCENE_H
#define _SCENE_H

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#include "graphics/model.h"
#include "graphics/gpu_program.h"

typedef struct
{
  int screen_width, screen_height;
  float screen_ratio;
  bool  model_loaded = false;
  int   polygon_mode = GL_FILL;
  bool  face_culling = true;
  int   front_face   = GL_CCW;
  int   shading_mode = PHONG_SHADING;
  bool  lights_on    = true;
  float gui_object_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};

} scene_state_t;

struct rgba
{
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;
};

class SuperScene
{
public:
  glm::mat4    model_matrix     = glm::mat4(1.0f);
  glm::vec3    bounding_box_min = glm::vec3(0.0f);
  glm::vec3    bounding_box_max = glm::vec3(1.0f);
  std::string  name;
  size_t       first_index = 0;
  size_t       vertex_count;
  GLenum       rendering_mode;
  GLuint       vao_id = 0;

  virtual void Enable(scene_state_t state) = 0;
  virtual void Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix) = 0;
  virtual void New_Frame() = 0;
  void DrawScene();
};

class OpenGL_Scene: public SuperScene
{
public:
  OpenGL_GpuProgram shader;
  GLuint vbo_model_id;
  GLuint vbo_normal_id;
  GLuint vbo_projected_id;

  void LoadModelToScene(model_t model);
  void Enable(scene_state_t state);
  void Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix);
  void New_Frame();
};

class Close2GL_Scene: public SuperScene
{
public:
  Close2GL_GpuProgram shader;
  GLuint vbo_vertex_id;
  GLuint vbo_texture_coords_id;
  GLuint texture_id;
  
  rgba  *color_buffer;
  float *depth_buffer;

  void LoadTrianglesToScene();
  void Enable(scene_state_t state);
  void Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix);
  void New_Frame();

  void ResizeBuffers(scene_state_t state);
};

#endif // _SCENE_H
