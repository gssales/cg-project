#include "scene.h"

void SuperScene::DrawScene()
{
  glBindVertexArray(this->vao_id);

  glDrawArrays(
    this->rendering_mode, 
    this->first_index, 
    this->vertex_count);
}

/* ==================== OpenGL ====================== */

void OpenGL_Scene::LoadModelToScene(model_t model)
{
  if (!this->vao_id) {
    GLuint bufs[3] = { this->vbo_model_id, this->vbo_normal_id, this->vbo_projected_id };
    glDeleteBuffers(3, bufs);
    glDeleteVertexArrays(1, &this->vao_id);
  }

  GLuint vertex_array_object_id;
  glGenVertexArrays(1, &vertex_array_object_id);
  glBindVertexArray(vertex_array_object_id);

  this->model_matrix      = glm::mat4(1.0f);
  this->bounding_box_max = model.bounding_box_max;
  this->bounding_box_min = model.bounding_box_min;
  this->name             = model.model_name;
  this->vertex_count     = model.vertices.size() / 4;
  this->rendering_mode   = GL_TRIANGLES;
  this->vao_id           = vertex_array_object_id;
  
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
  this->vbo_model_id = VBO_model_coefficients_id;

  location = 1;
  GLuint VBO_normal_coefficients_id;
  glCreateBuffers(1, &VBO_normal_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, model.normals.size() * sizeof(GL_FLOAT), model.normals.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->vbo_normal_id = VBO_normal_coefficients_id;

  location = 2;
  GLuint VBO_projected_coefficients_id;
  int data_size = 4;
  float data[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  glCreateBuffers(1, &VBO_projected_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_projected_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, data_size * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, data_size * sizeof(GL_FLOAT), data);
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->vbo_projected_id = VBO_projected_coefficients_id;

  glBindVertexArray(0);
}

void OpenGL_Scene::Enable(scene_state_t state)
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glPolygonMode(GL_FRONT_AND_BACK, state.polygon_mode);
  if (state.face_culling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(state.front_face);
  }
  else {
    glDisable(GL_CULL_FACE);
  }
}

void OpenGL_Scene::Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix)
{
  glm::mat4 mvp = projection_matrix * view_matrix * this->model_matrix;
  glUseProgram(this->shader.program_id);
  glUniformMatrix4fv(
    this->shader.model_view_proj_uniform, 
    1, GL_FALSE, 
    glm::value_ptr(mvp)
  );
  glUniformMatrix4fv(
    this->shader.view_uniform,
    1, GL_FALSE,
    glm::value_ptr(view_matrix)
  );
  glUniformMatrix4fv(
    this->shader.model_uniform,
    1, GL_FALSE,
    glm::value_ptr(this->model_matrix)
  );
  glUniform4fv(this->shader.color_uniform, 1 , state.gui_object_color);
  glUniform1i(this->shader.shading_uniform, state.shading_mode);
  glUniform1i(this->shader.lighting_uniform, state.lights_on);

  this->DrawScene();
}

void OpenGL_Scene::New_Frame()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* ==================== Close2GL ====================== */

void Close2GL_Scene::LoadTrianglesToScene()
{
  float vertex_data[] = {
   -1.0f, -1.0f, 
    1.0f, -1.0f, 
    1.0f,  1.0f, 
    1.0f,  1.0f, 
   -1.0f,  1.0f, 
   -1.0f, -1.0f };
  int vertex_count = 12;
  float texture_coords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f };
  int texcoords_count = 12;

  GLuint vertex_array_object_id;
  glGenVertexArrays(1, &vertex_array_object_id);
  glBindVertexArray(vertex_array_object_id);

  this->name           = "Triangles";
  this->first_index    = 0;
  this->vertex_count   = vertex_count / 2;
  this->rendering_mode = GL_TRIANGLES;
  this->vao_id = vertex_array_object_id;

  GLint  number_of_dimensions = 2;
  GLuint location = 0;
  GLuint VBO_vertex_coefficients_id;
  glCreateBuffers(1, &VBO_vertex_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex_coefficients_id);
    glBufferStorage( GL_ARRAY_BUFFER, vertex_count * sizeof(GL_FLOAT), vertex_data, GL_DYNAMIC_STORAGE_BIT);
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->vbo_vertex_id = VBO_vertex_coefficients_id;

  location = 1;
  GLuint VBO_texcoords_coefficients_id;
  glCreateBuffers(1, &VBO_texcoords_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_texcoords_coefficients_id);
    glBufferStorage( GL_ARRAY_BUFFER, texcoords_count * sizeof(GL_FLOAT), texture_coords, GL_DYNAMIC_STORAGE_BIT);
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->vbo_texture_coords_id = VBO_texcoords_coefficients_id;

  glBindVertexArray(0);
}

void Close2GL_Scene::Enable(scene_state_t state)
{
  glDisable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, state.polygon_mode);
  glDisable(GL_CULL_FACE);

  this->ResizeBuffers(state);
  
  // int half_h = std::floor(state.screen_height/2.0f);
  // int half_w = std::floor(state.screen_width/2.0f);

  // for (int i = state.screen_height -1; i >= 0; i--)
  //   for (int j = state.screen_width -1; j >= 0; j--)
  //     // if (i <= 100 && j <= 100)
  //       color_buffer[i*state.screen_height+j] = { 1.0f, 0.0f, 0.0f, 1.0f };
        
  // glBindTexture(GL_TEXTURE_2D, this->texture_id);
  // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, state.screen_width, state.screen_height, GL_RGBA, GL_FLOAT, color_buffer);

}

void Close2GL_Scene::Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix)
{
  // this->ResizeBuffers(state);

  // int half_h = std::floor(state.screen_height/2.0f);
  // int half_w = std::floor(state.screen_width/2.0f);

  // for (int i = state.screen_height -1; i >= 0; i--)
  //   for (int j = state.screen_width -1; j >= 0; j--)
  //       color_buffer[i*state.screen_height+j] = { 1.0f, 0.0f, 0.0f, 1.0f };
        
  // glBindTexture(GL_TEXTURE_2D, this->texture_id);
  // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, state.screen_width, state.screen_height, GL_RGBA, GL_FLOAT, color_buffer);

  glUseProgram(this->shader.program_id);
  glUniform1i(this->shader.texture_uniform, GL_TEXTURE0);

  this->DrawScene();
}

void Close2GL_Scene::New_Frame()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Close2GL_Scene::ResizeBuffers(scene_state_t state)
{
  delete[] this->color_buffer;
  delete[] this->depth_buffer;
  this->color_buffer = new rgba[state.screen_width * state.screen_height];
  this->depth_buffer = new float[state.screen_width * state.screen_height];

  glDeleteTextures(1, &this->texture_id);

  for (int i = state.screen_height -1; i >= 0; i--)
    for (int j = state.screen_width -1; j >= 0; j--)
      if (j <= 100 && i <= 100)
        color_buffer[i*state.screen_width+j] = { 1.0f, 0.0f, 0.0f, 1.0f };
        
  GLuint tex_id;
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, state.screen_width, state.screen_height, 0, GL_RGBA, GL_FLOAT, this->color_buffer);  
  this->texture_id = tex_id;
}
