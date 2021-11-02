#ifndef _SCENE_H
#define _SCENE_H

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <map>

#include "matrices.h"
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

  bool debug = false;
} scene_state_t;

typedef struct
{
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;
} rgba_t;

const rgba_t black = { 0.0, 0.0, 0.0, 1.0 };

typedef struct
{
  glm::vec4 vertices[3];
  glm::vec4 normals[3];
  glm::vec4 face_normal;
} triangle_t;

typedef struct
{
  glm::vec4 vertex_p;
  glm::vec4 vertex_ccs;
  glm::vec4 normal;
  glm::vec4 color_rgba;
  float ww = 1.0;
} interpolating_attr_t;

typedef struct
{
  glm::vec4 vertex_top;
  glm::vec4 vertex_bottom;
  glm::vec3 vertex_delta;
  float inc_x, inc_z;
  interpolating_attr_t top;
  interpolating_attr_t bottom;
} edge_t;

typedef edge_t scanline_t;

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
  
  model_t model;
  std::vector<triangle_t> ccs_model;
  std::vector<triangle_t> triangles;

  int buffer_size;
  rgba_t *color_buffer;
  float  *depth_buffer;

  void LoadTrianglesToScene();
  void Enable(scene_state_t state);
  void Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix);
  void New_Frame();

  void SetModel(model_t model);
  void ResizeBuffers(scene_state_t state);
  void TransformModel(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix);
  void Rasterize(scene_state_t state, glm::mat4 projection_matrix);
  edge_t FindEdge(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr);
  scanline_t FindScanline(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr);
  edge_t* OrderEdges(edge_t* edges);
  interpolating_attr_t Interpolate(interpolating_attr_t attr_0, interpolating_attr_t attr_1, float min, float max, float value);
  void RasterScanline(scene_state_t state, scanline_t line);
  void ChangeBuffer(scene_state_t state, int x, int y, float z, rgba_t color);
};

bool FaceCulling(glm::vec4 *vertices, int face_orientation);
rgba_t vec4_to_rgba(glm::vec4 vec);

void PrintTriangle(triangle_t t);
void PrintEdge(edge_t e);

#endif // _SCENE_H
