#include "scene.h"

float clamp(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

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
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);

  this->ResizeBuffers(state);
}

void Close2GL_Scene::Render(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix)
{  
  glm::mat4 viewport_map = matrices::viewport(0, 0, state.screen_width, state.screen_height);

  this->TransformModel(state, view_matrix, projection_matrix, viewport_map);
  
  this->Rasterize(state, projection_matrix, viewport_map);
        
  glBindTexture(GL_TEXTURE_2D, this->texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, state.screen_width, state.screen_height, 0, GL_RGBA, GL_FLOAT, this->color_buffer); 

  glUseProgram(this->shader.program_id);
  glUniform1i(this->shader.texture_uniform, GL_TEXTURE0);

  this->DrawScene();
}

void Close2GL_Scene::New_Frame()
{
  for (int i = 0; i < buffer_size; i++)
  {
    this->color_buffer[i] = black;
    this->depth_buffer[i] = std::numeric_limits<float>::infinity();
  }

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Close2GL_Scene::ResizeBuffers(scene_state_t state)
{
  delete[] this->color_buffer;
  delete[] this->depth_buffer;
  this->buffer_size = state.screen_width * state.screen_height;
  this->color_buffer = new rgba_t[this->buffer_size];
  this->depth_buffer = new float[this->buffer_size];

  glDeleteTextures(1, &this->texture_id);

  GLuint tex_id;
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, state.screen_width, state.screen_height, 0, GL_RGBA, GL_FLOAT, this->color_buffer);  
  this->texture_id = tex_id;
}

void Close2GL_Scene::SetModel(model_t model)
{
  this->model = model;
}

void Close2GL_Scene::TransformModel(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::mat4 viewport_matrix)
{
  triangles.clear();

  glm::mat4 to_ccs = view_matrix * model_matrix;
  glm::mat4 mvp = projection_matrix * view_matrix * this->model_matrix;

  std::vector<float> *vertices = &this->model.vertices;
  std::vector<float> *normals = &this->model.normals;
  for (int i = 0; i < vertices->size(); i += 12)
  {
    // OBJECT SPACE
    glm::vec4 v0 = glm::vec4(  (*vertices)[i], (*vertices)[i+1], (*vertices)[i+2], (*vertices)[i+3]);
    glm::vec4 v1 = glm::vec4((*vertices)[i+4], (*vertices)[i+5], (*vertices)[i+6], (*vertices)[i+7]);
    glm::vec4 v2 = glm::vec4((*vertices)[i+8], (*vertices)[i+9], (*vertices)[i+10], (*vertices)[i+11]);

    triangle_t t;
    t.ccs_vertices[0] = to_ccs * v0;
    t.ccs_vertices[1] = to_ccs * v1;
    t.ccs_vertices[2] = to_ccs * v2;
    // CAMERA/EYE SPACE

    t.vertices[0] = mvp * v0;
    t.vertices[1] = mvp * v1;
    t.vertices[2] = mvp * v2;
    // HOMOGENUOUS CLIPPING SPACE

    if (t.vertices[0].w <= 0 || t.vertices[1].w <= 0 || t.vertices[2].w <= 0)
      continue;

    t.vertices[0] /= t.vertices[0].w;
    t.vertices[1] /= t.vertices[1].w;
    t.vertices[2] /= t.vertices[2].w;
    // NORMALIZED DEVICE SPACE

    if (std::abs(t.vertices[0].x) > 1.0f 
        || std::abs(t.vertices[0].y) > 1.0f 
        || std::abs(t.vertices[0].z) > 1.0f)
      continue;

    if (std::abs(t.vertices[1].x) > 1.0f 
        || std::abs(t.vertices[1].y) > 1.0f 
        || std::abs(t.vertices[1].z) > 1.0f)
      continue;

    if (std::abs(t.vertices[2].x) > 1.0f 
        || std::abs(t.vertices[2].y) > 1.0f 
        || std::abs(t.vertices[2].z) > 1.0f)
      continue;

    t.vp_vertices[0] = viewport_matrix * t.vertices[0];
    t.vp_vertices[1] = viewport_matrix * t.vertices[1];
    t.vp_vertices[2] = viewport_matrix * t.vertices[2];

    if (state.face_culling)
    {
      bool is_front_facing = FaceCulling(t.vp_vertices, state.front_face);
      if (!is_front_facing)
        continue;
    }

    t.normals[0] = mvp * glm::vec4(  (*normals)[i], (*normals)[i+1], (*normals)[i+2], (*normals)[i+3]);
    t.normals[1] = mvp * glm::vec4((*normals)[i+4], (*normals)[i+5], (*normals)[i+6], (*normals)[i+7]);
    t.normals[2] = mvp * glm::vec4((*normals)[i+8], (*normals)[i+9], (*normals)[i+10], (*normals)[i+11]);

    glm::vec4 u = t.vertices[1] - t.vertices[0];
    glm::vec4 v = t.vertices[2] - t.vertices[0];
    t.face_normal.x = u.y*v.z - u.z*v.y;
    t.face_normal.y = u.z*v.x - u.x*v.z;
    t.face_normal.z = u.x*v.y - u.y*v.x;
    t.face_normal.w = 0.0f;

    this->triangles.push_back(t);
  }
}

void Close2GL_Scene::Rasterize(scene_state_t state, glm::mat4 projection_matrix, glm::mat4 viewport_matrix)
{

  glm::vec4 color = glm::vec4(state.gui_object_color[0], state.gui_object_color[1], state.gui_object_color[2], state.gui_object_color[3]);
  glm::vec4 colors[3] = { glm::vec4(1.0, 0.0, 0.0, 1.0), glm::vec4(0.0, 1.0, 0.0, 1.0), glm::vec4(0.0, 0.0, 1.0, 1.0) };

  for (int i = 0; i < this->triangles.size(); i++)
  {
    triangle_t t = this->triangles[i];
    interpolating_attr_t vertex_attrs[3];
    for (int a = 0; a < 3; a++)
    {
      float w = t.vertices[a].w;
      interpolating_attr_t attrs;
      attrs.vertex_p   = t.vertices[a] / w;
      attrs.vertex_ccs = t.ccs_vertices[a] / w;
      attrs.normal     = t.normals[a] / w;
      attrs.color_rgba = colors[a] / w; // vertex color shader
      attrs.ww        /= w;

      vertex_attrs[a] = attrs;
    }

    edge_t* edges = new edge_t[3];
    for (int e = 0; e < 3; e++)
    {
      int next_e = (e+1) % 3;
      edges[e] = this->FindEdge(
        t.vp_vertices[e],      vertex_attrs[e],
        t.vp_vertices[next_e], vertex_attrs[next_e]
      );
    }

    edges = this->OrderEdges(edges);
    
    int active_edge = 1;
    int max_inc = std::ceil(edges[0].vertex_delta.y);

    int y, x;
    glm::vec4 p_a, p_b;
    interpolating_attr_t attr_a, attr_b;
    for (int inc_y = 0; inc_y < max_inc; inc_y++)
    {
      int inc = inc_y;

      p_a = edges[0].vertex_top;
      p_a.y = p_a.y + inc;
      p_a.x = p_a.x + inc * edges[0].inc_x;
      p_a.z = p_a.z + inc * edges[0].inc_z;
      y = std::floor(p_a.y);
      x = std::floor(p_a.x);

      attr_a = this->Interpolate(edges[0].bottom, edges[0].top, 0, max_inc, inc);

      this->ChangeBuffer(state, x, y, p_a.z, vec4_to_rgba(attr_a.color_rgba / attr_a.ww));
      
      if (active_edge == 2)
        inc -= edges[1].vertex_delta.y;

      float min_x, max_x;
      if (edges[active_edge].vertex_top.x >= edges[active_edge].vertex_bottom.x)
      {
        max_x = edges[active_edge].vertex_top.x;
        min_x = edges[active_edge].vertex_bottom.x;
      }
      else
      {
        max_x = edges[active_edge].vertex_bottom.x;
        min_x = edges[active_edge].vertex_top.x;
      }

      p_b = edges[active_edge].vertex_top;
      p_b.y = p_b.y + inc;
      p_b.x = clamp(p_b.x + inc * edges[active_edge].inc_x, min_x, max_x);
      p_b.z = p_b.z + inc * edges[active_edge].inc_z;
      y = std::floor(p_b.y);
      x = std::floor(p_b.x);

      if (edges[active_edge].vertex_delta.y == 0.0f)
      {
        scanline_t sl = this->FindScanline(
            edges[active_edge].vertex_top,    edges[active_edge].top, 
            edges[active_edge].vertex_bottom, edges[active_edge].bottom);
        this->RasterScanline(state, sl);
      }

      attr_b = this->Interpolate(edges[active_edge].bottom, edges[active_edge].top, 0, edges[active_edge].vertex_delta.y, inc);

      this->ChangeBuffer(state, x, y, p_b.z, vec4_to_rgba(attr_b.color_rgba / attr_b.ww));
      
      if (state.polygon_mode == GL_FILL)
      {
        float my = (p_a.y + p_b.y) / 2.0f;
        p_a.y = my;
        p_b.y = my;
        scanline_t sl = this->FindScanline(glm::make_vec4(p_a), attr_a, glm::make_vec4(p_b), attr_b);
        this->RasterScanline(state, sl);
      }

      if (active_edge == 1 && p_b.y > edges[1].vertex_bottom.y)
        active_edge = 2;
    }
  }
}

edge_t Close2GL_Scene::FindEdge(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr)
{
  edge_t e;

  if (v0.y < v1.y) {
    e.vertex_top    = v0;
    e.top    = v0_attr;
    e.vertex_bottom = v1;
    e.bottom = v1_attr;
  } else {
    e.vertex_top    = v1;
    e.top    = v1_attr;
    e.vertex_bottom = v0;
    e.bottom = v0_attr;
  }

  glm::vec4 d = e.vertex_bottom - e.vertex_top;
  if (std::abs(d.y) >= 1.0)
  {
    e.inc_x = d.x / d.y;
    e.inc_z = d.z / d.y;
  }
  else
  {
    e.inc_x = d.x;
    e.inc_z = d.z;
  }
  e.vertex_delta = d;
  
  return e;
}

scanline_t Close2GL_Scene::FindScanline(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr)
{
  scanline_t sl;

  if (v0.x < v1.x) {
    sl.vertex_top    = v0;
    sl.top    = v0_attr;
    sl.vertex_bottom = v1;
    sl.bottom = v1_attr;
  } else {
    sl.vertex_top    = v1;
    sl.top    = v1_attr;
    sl.vertex_bottom = v0;
    sl.bottom = v0_attr;
  }

  glm::vec4 d = sl.vertex_bottom - sl.vertex_top;
  sl.inc_x = 1;
  sl.inc_z = d.z / d.x;

  sl.vertex_delta = d;
  
  return sl;
}

edge_t* Close2GL_Scene::OrderEdges(edge_t* edges)
{
  float bottom_y = 0.0f, 
    top_y = std::numeric_limits<float>::max(), 
    tallest_y = 0.0f;
  bool has_horizontal_edge = false;
  for (int i = 0; i < 3; i++)
  {
    tallest_y = std::max(tallest_y, std::abs(edges[i].vertex_delta.y));
    bottom_y  = std::max(bottom_y, edges[i].vertex_bottom.y);
    top_y     = std::min(top_y,    edges[i].vertex_top.y);
    has_horizontal_edge = has_horizontal_edge || (edges[i].vertex_delta.y == 0.0f);
  }

  edge_t* new_edges = new edge_t[3];
  for (int i = 0; i < 3; i++)
  {
    if (has_horizontal_edge)
      if (edges[i].vertex_delta.y == 0.0f)
        if (edges[i].vertex_top.y == top_y)
          new_edges[1] = edges[i];
        else
          new_edges[2] = edges[i];
      else
        if (new_edges[0].vertex_top.w == 0.0f)
          new_edges[0] = edges[i];
        else
          if (new_edges[1].vertex_top.w == 0.0f)
            new_edges[1] = edges[i];
          else
            new_edges[2] = edges[i];
    else
      if (edges[i].vertex_top.y == top_y)
        if (edges[i].vertex_bottom.y == bottom_y)
          new_edges[0] = edges[i];
        else
          new_edges[1] = edges[i];
      else
        new_edges[2] = edges[i];    
  }

  return new_edges;
}

interpolating_attr_t Close2GL_Scene::Interpolate(interpolating_attr_t attr_0, interpolating_attr_t attr_1, float min, float max, float value)
{
  float alpha = (value - min) / (max - min);
  interpolating_attr_t result;
  result.color_rgba = alpha * attr_0.color_rgba + (1 - alpha) * attr_1.color_rgba;
  result.normal     = alpha * attr_0.normal     + (1 - alpha) * attr_1.normal;
  result.vertex_ccs = alpha * attr_0.vertex_ccs + (1 - alpha) * attr_1.vertex_ccs;
  result.vertex_p   = alpha * attr_0.vertex_p   + (1 - alpha) * attr_1.vertex_p;
  result.ww         = alpha * attr_0.ww         + (1 - alpha) * attr_1.ww;
  return result;
}

void Close2GL_Scene::RasterScanline(scene_state_t state, scanline_t line)
{
  int max_inc = std::ceil(line.vertex_delta.x);
  int x, y;
  float z;
  y = std::floor((line.vertex_top.y + line.vertex_bottom.y) / 2.0f);
  for (int inc_x = 0; inc_x < max_inc; inc_x++)
  {
    z = line.vertex_top.z + inc_x * line.inc_z;
    x = std::floor(line.vertex_top.x + inc_x);

    interpolating_attr_t attr = this->Interpolate(line.bottom, line.top, 0, max_inc, inc_x);

    this->ChangeBuffer(state, x, y, z, vec4_to_rgba(attr.color_rgba / attr.ww));
  }
}

void Close2GL_Scene::ChangeBuffer(scene_state_t state, int x, int y, float z, rgba_t color)
{
  if (x < 0 || y < 0 || x >= state.screen_width || y >= state.screen_height)
    return;
  int index = (state.screen_height - y -1)*state.screen_width+x % this->buffer_size;
  if (z < this->depth_buffer[index])
  {
    this->depth_buffer[index] = z;
    this->color_buffer[index] = color;
  }
}

bool FaceCulling(glm::vec4 *vertices, int face_orientation)
{
  glm::vec4 v0 = vertices[0];
  glm::vec4 v1 = vertices[1];
  glm::vec4 v2 = vertices[2];

  float a = (v0.x*v1.y - v1.x*v0.y) + (v1.x*v2.y - v2.x*v1.y) + (v2.x*v0.y - v0.x*v2.y);

  return (face_orientation == GL_CW) && (a > 0) || (a < 0);
}

rgba_t vec4_to_rgba(glm::vec4 vec)
{
  rgba_t c;
  c.r = vec.x;
  c.g = vec.g;
  c.b = vec.b;
  c.a = vec.a;
  return c;
}

void PrintTriangle(triangle_t t)
{
  PrintVec4(t.vertices[0]);
  PrintVec4(t.vertices[1]);
  PrintVec4(t.vertices[2]);
}

void PrintEdge(edge_t e)
{
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ] >> [ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n", 
      e.vertex_top.x, e.vertex_top.y, e.vertex_top.z, e.vertex_top.w, 
      e.vertex_bottom.x, e.vertex_bottom.y, e.vertex_bottom.z, e.vertex_bottom.w);
}
