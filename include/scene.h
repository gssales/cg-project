#ifndef _SCENE_H
#define _SCENE_H

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#include "graphics/model.h"

typedef struct
{
  glm::mat4 model_space = glm::mat4(1.0f);
  glm::vec3    bounding_box_min;
  glm::vec3    bounding_box_max;
  std::string  name;        // Nome do objeto
  size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
  size_t       vertex_count; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
  GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
  GLuint       vao_id; // ID do VAO onde estão armazenados os atributos do modelo
  GLuint       vbo_model_id; // ID do VAO onde estão armazenados os atributos do modelo
  GLuint       vbo_normal_id; // ID do VAO onde estão armazenados os atributos do modelo
  GLuint       vbo_projected_id; // ID do VAO onde estão armazenados os atributos do modelo
} scene_object_t;

void AddModelToScene(scene_object_t *obj, model_t model);
void DrawVirtualObject(scene_object_t obj, GLuint program_id);

void UseOpenGL(scene_object_t *obj, model_t model);

#endif // _SCENE_H
