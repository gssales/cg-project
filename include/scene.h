#ifndef _SCENE_H
#define _SCENE_H

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <map>

#include "graphics/model.h"

typedef struct
{
  std::string  name;        // Nome do objeto
  size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
  size_t       vertex_count; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
  GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
  GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
} scene_object_t;

void AddModelToScene(scene_object_t *obj, model_t model);
void DrawVirtualObject(scene_object_t obj, GLuint program_id);

#endif // _SCENE_H
