#include "scene.h"

void AddModelToScene(scene_object_t *obj, model_t model)
{
  GLuint vertex_array_object_id;
  glGenVertexArrays(1, &vertex_array_object_id);
  glBindVertexArray(vertex_array_object_id);
    
  obj->model_space = glm::mat4(1.0f);
  obj->bounding_box_max = model.bounding_box_max;
  obj->bounding_box_min = model.bounding_box_min;
  obj->name           = model.model_name;
  obj->first_index    = 0; // Primeiro índice
  obj->vertex_count    = model.vertices.size() / 4; // Número de indices
  obj->rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.
  obj->vertex_array_object_id = vertex_array_object_id;

  GLint  number_of_dimensions = 4;
  GLuint location = 0;
  GLuint VBO_model_coefficients_id;
  glCreateBuffers(1, &VBO_model_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
  glBufferStorage(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(GL_FLOAT), model.vertices.data(), GL_DYNAMIC_STORAGE_BIT);
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  location = 1;
  GLuint VBO_normal_coefficients_id;
  glCreateBuffers(1, &VBO_normal_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
  glBufferStorage(GL_ARRAY_BUFFER, model.normals.size() * sizeof(GL_FLOAT), model.normals.data(), GL_DYNAMIC_STORAGE_BIT);
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint indices_id;
  glCreateBuffers(1, &indices_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
  glBufferStorage(GL_ARRAY_BUFFER, model.indices.size() * sizeof(GL_UNSIGNED_INT), model.indices.data(), GL_DYNAMIC_STORAGE_BIT);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
  glBindVertexArray(0);
}

void DrawVirtualObject(scene_object_t obj, GLuint program_id)
{
  glBindVertexArray(obj.vertex_array_object_id);

  glDrawArrays(obj.rendering_mode, obj.first_index, obj.vertex_count);
  // glDrawElements(
  //     obj.rendering_mode,
  //     obj.vertex_count,
  //     GL_UNSIGNED_INT,
  //     (void*)(obj.first_index * sizeof(GLuint))
  // );

  // glBindVertexArray(0);
}