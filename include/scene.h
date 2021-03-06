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
#include "graphics/texture.h"
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
  int   lighting_mode = AMBIENT_LIGHT + DIFFUSE_LIGHT + SPECULAR_LIGHT;
  bool  use_calculated_normals = true;
  bool  use_raw_normals = false;
  bool  enable_texture = false;
  int   texture_filter = GL_NEAREST;
  int   filter_level = 0;

  float gui_object_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};

  bool debug_colors = false;
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
  glm::vec4 ccs_position;
  glm::vec4 ccs_normal;
  glm::vec4 color;
  glm::vec2 texture_coords;
  float ww = 1.0;

  glm::vec4 vColorAmbient;
  glm::vec4 vColorDiffuse;
  glm::vec4 vColorSpecular;

  glm::vec4 flatColorAmbient;
  glm::vec4 flatColorDiffuse;
  glm::vec4 flatColorSpecular;

  glm::vec4 flatColor; // this attribute is not being interpolated
  glm::vec4 flatCcsNormal; // this attribute is not being interpolated
} interpolating_attr_t;

typedef struct
{
  glm::vec4 vertices[3];        // Homogeneous Clipping Space
  glm::vec4 mapped_vertices[3]; // Viewport
  glm::vec4 normals[3];
  glm::vec4 face_normal;
  interpolating_attr_t attrs[3];
} triangle_t;

typedef struct
{
  glm::vec4 vertex_top;
  glm::vec4 vertex_bottom;
  glm::vec4 vertex_delta;
  float inc_x, inc_z;
  float min_x, max_x;
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
  GLuint vbo_surface_normal_id;
  GLuint vbo_texture_coords_id;
  GLuint texture_id = 0;

  void LoadModelToScene(scene_state_t state, model_t model);
  void LoadTextureToScene(scene_state_t state, texture_t tex);
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
  std::vector<triangle_t> triangles;

  int buffer_size;
  rgba_t *color_buffer;
  float  *depth_buffer;

  texture_t *mipmaps;
  glm::vec2 delta_tex;

  void LoadTrianglesToScene();
  void Enable(scene_state_t state);
  void Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix);
  void New_Frame();
  void SetModel(model_t model);
  void SetMipmap(texture_t *mipmaps);
  void ResizeBuffers(scene_state_t state);

private:
  void TransformModel(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::mat4 viewport_matrix);
  void Rasterize(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::mat4 viewport_matrix);
  void RasterScanline(scene_state_t state, scanline_t line, glm::mat4 view_matrix);
  void ChangeBuffer(scene_state_t state, int x, int y, float z, rgba_t color);
  glm::vec4 ProcessFragment(scene_state_t state, interpolating_attr_t *attr, int x, int y, interpolating_attr_t flatAttr);
  glm::vec4 GetTextureColor(scene_state_t state, glm::vec2 texture_coord, glm::vec2 delta_tex);
  glm::vec4 Nearest(glm::vec2 texture_coord, int level);
  glm::vec4 Bilinear(glm::vec2 texture_coord, int level);
  glm::vec4 Trilinear(glm::vec2 texture_coord, glm::vec2 delta_tex);
};

rgba_t vec4_to_rgba(glm::vec4 vec);
void EraseTriangleWithVertex(std::vector<model_triangle_t> *triangles, int index);
bool FaceCulling(glm::vec4 *vertices, int face_orientation);
edge_t FindEdge(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr);
scanline_t FindScanline(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr);
edge_t* OrderEdges(edge_t* edges);
interpolating_attr_t Interpolate(interpolating_attr_t attr_0, interpolating_attr_t attr_1, float min, float max, float value);
glm::vec4 AmbientLighting(glm::vec4 color);
glm::vec4 DiffuseLighting(glm::vec4 color, glm::vec4 ccs_normal, glm::vec4 ccs_position);
glm::vec4 SpecularLighting(glm::vec4 ccs_normal, glm::vec4 ccs_position);
glm::vec4 Lighting(scene_state_t state, interpolating_attr_t attr, glm::vec4 normal);
glm::vec4 LightingWithTextureMapping(scene_state_t state, interpolating_attr_t attr, glm::vec4 color, glm::vec4 normal);
void Shading(scene_state_t state, interpolating_attr_t *attr);

void PrintTriangle(triangle_t t);
void PrintEdge(edge_t e);

#endif // _SCENE_H
