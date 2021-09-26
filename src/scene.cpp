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
  obj->vao_id = vertex_array_object_id;

  GLint  number_of_dimensions = 4;
  GLuint location = 0;
  GLuint VBO_model_coefficients_id;
  glCreateBuffers(1, &VBO_model_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, model.vertices.size() * sizeof(GL_FLOAT), model.vertices.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  obj->vbo_model_id = VBO_model_coefficients_id;

  location = 1;
  GLuint VBO_normal_coefficients_id;
  glCreateBuffers(1, &VBO_normal_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, model.normals.size() * sizeof(GL_FLOAT), model.normals.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  obj->vbo_normal_id = VBO_normal_coefficients_id;

  location = 2;
  GLuint VBO_projected_coefficients_id;
  int data_size = 4;
  float data[data_size] = {0.0f, 0.0f, 0.0f, 1.0f};
  glCreateBuffers(1, &VBO_projected_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_projected_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, data_size * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, data_size * sizeof(GL_FLOAT), data);
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  obj->vbo_projected_id = VBO_projected_coefficients_id;

  GLuint indices_id;
  glCreateBuffers(1, &indices_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
  glBufferStorage(GL_ARRAY_BUFFER, model.indices.size() * sizeof(GL_UNSIGNED_INT), model.indices.data(), GL_DYNAMIC_STORAGE_BIT);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
  glBindVertexArray(0);
}

void DrawVirtualObject(scene_object_t obj, GLuint program_id)
{
  glBindVertexArray(obj.vao_id);

  glDrawArrays(obj.rendering_mode, obj.first_index, obj.vertex_count);
}

void UseOpenGL(scene_object_t *obj, model_t model)
{
  obj->first_index = 0;
  obj->vertex_count = model.vertices.size();
  
  glBindVertexArray(obj->vao_id);

  GLint number_of_dimensions = 4;
  GLint location = 0;
  glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_model_id);
  glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, model.vertices.size() * sizeof(GL_FLOAT), model.vertices.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  location = 1;
  glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_normal_id);
  glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, model.normals.size() * sizeof(GL_FLOAT), model.normals.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}