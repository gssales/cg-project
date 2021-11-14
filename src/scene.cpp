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

void OpenGL_Scene::LoadModelToScene(scene_state_t state, model_t model)
{
  if (this->vao_id) {
    GLuint bufs[3] = { this->vbo_model_id, this->vbo_normal_id, this->vbo_surface_normal_id };
    glDeleteBuffers(3, bufs);
    glDeleteVertexArrays(1, &this->vao_id);
  }

  GLuint vertex_array_object_id;
  glGenVertexArrays(1, &vertex_array_object_id);
  glBindVertexArray(vertex_array_object_id);

  std::vector<float> vertices = ExtractVertices(model);
  std::vector<float> normals, surface_normals;
  if (state.use_raw_normals) {
    normals = model.raw_normals;
    surface_normals = ExtractSurfaceNormals(model);
  } else
  if (state.use_calculated_normals) {
    normals = ExtractCalculatedNormals(model);
    surface_normals = ExtractCalculatedSurfaceNormals(model);
  } else {
    normals = ExtractNormals(model);
    surface_normals = ExtractSurfaceNormals(model);
  }

  this->model_matrix     = glm::mat4(1.0f);
  this->bounding_box_max = model.bounding_box_max;
  this->bounding_box_min = model.bounding_box_min;
  this->name             = model.model_name;
  this->vertex_count     = vertices.size() / 4;
  this->rendering_mode   = GL_TRIANGLES;
  this->vao_id           = vertex_array_object_id;
  
  GLint  number_of_dimensions = 4;
  GLuint location = 0;
  GLuint VBO_model_coefficients_id;
  glCreateBuffers(1, &VBO_model_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GL_FLOAT), vertices.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->vbo_model_id = VBO_model_coefficients_id;

  location = 1;
  number_of_dimensions = 4;
  GLuint VBO_normal_coefficients_id;
  glCreateBuffers(1, &VBO_normal_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(GL_FLOAT), normals.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->vbo_normal_id = VBO_normal_coefficients_id;

  location = 2;
  number_of_dimensions = 4;
  GLuint VBO_surface_normal_coefficients_id;
  glCreateBuffers(1, &VBO_surface_normal_coefficients_id);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_surface_normal_coefficients_id);
  glBufferData(GL_ARRAY_BUFFER, surface_normals.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, surface_normals.size() * sizeof(GL_FLOAT), surface_normals.data());
  glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  this->vbo_surface_normal_id = VBO_surface_normal_coefficients_id;

  if (model.has_texture) {
    std::vector<float> texcoords = ExtractTextureCoords(model);
    location = 3;
    number_of_dimensions = 2;
    GLuint VBO_texture_coords_coefficients_id;
    glCreateBuffers(1, &VBO_texture_coords_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coords_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, texcoords.size() * sizeof(GL_FLOAT), texcoords.data());
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    this->vbo_texture_coords_id = VBO_texture_coords_coefficients_id;
  }

  glBindVertexArray(0);
}

void OpenGL_Scene::LoadTextureToScene(scene_state_t state, texture_t tex)
{
  if (!this->texture_id)
    glDeleteBuffers(1, &this->texture_id);

  GLuint tex_id;
  glActiveTexture(GL_TEXTURE1);
  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state.texture_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state.texture_filter);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, tex.data);
  glGenerateMipmap(GL_TEXTURE_2D);
  this->texture_id = tex_id;
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
  glUniform1i(this->shader.lighting_uniform, state.lighting_mode);
  glUniform1i(this->shader.texture_uniform, 1);
  glUniform1i(this->shader.has_texture_uniform, state.enable_texture);

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
  
  this->Rasterize(state, view_matrix, projection_matrix, viewport_map);
        
  glBindTexture(GL_TEXTURE_2D, this->texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

void Close2GL_Scene::SetModel(model_t model)
{
  this->model = model;
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



/* ==================== Close2GL PRIVATE ====================== */

void Close2GL_Scene::TransformModel(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::mat4 viewport_matrix)
{
  this->triangles.clear();

  glm::mat4 mvp = projection_matrix * view_matrix * this->model_matrix;

  std::vector<model_triangle_t> model_triangles = this->model.triangles;
  std::vector<glm::vec4> model_vertices= this->model.vertices;

  int vertex_index = 0;
  for (glm::vec4 &v : model_vertices) 
  {
    v = mvp * v;
    if (v.w <= 0.0) {
      EraseTriangleWithVertex(&model_triangles, vertex_index);
      vertex_index++;
      continue;
    }

    glm::vec4 ndc_v = v / v.w;
    if (std::abs(ndc_v.x) > 1.0f || std::abs(ndc_v.y) > 1.0f || std::abs(ndc_v.z) > 1.0f) {
      EraseTriangleWithVertex(&model_triangles, vertex_index);
      vertex_index++;
      continue;
    }

    vertex_index++;
  }

  glm::vec4 color = glm::vec4(state.gui_object_color[0], state.gui_object_color[1], state.gui_object_color[2], state.gui_object_color[3]);
  glm::vec4 debug_colors[3] = { glm::vec4(1.0, 0.0, 0.0, 1.0), glm::vec4(0.0, 1.0, 0.0, 1.0), glm::vec4(0.0, 0.0, 1.0, 1.0) };

  for (model_triangle_t model_triangle : model_triangles) {

    triangle_t t;

    glm::vec4 v0 = model_vertices[model_triangle.indices[0]];
    glm::vec4 v1 = model_vertices[model_triangle.indices[1]];
    glm::vec4 v2 = model_vertices[model_triangle.indices[2]];

    t.vertices[0] = v0;
    t.vertices[1] = v1;
    t.vertices[2] = v2;

    t.mapped_vertices[0] = viewport_matrix * (v0 / v0.w);
    t.mapped_vertices[1] = viewport_matrix * (v1 / v1.w);
    t.mapped_vertices[2] = viewport_matrix * (v2 / v2.w);
    
    if (state.face_culling)
    {
      bool is_front_facing = FaceCulling(t.mapped_vertices, state.front_face);
      if (!is_front_facing)
        continue;
    }

    if (state.use_raw_normals) {
      int i0 = 4 * model_triangle.indices[0];
      int i1 = 4 * model_triangle.indices[1];
      int i2 = 4 * model_triangle.indices[2];
      
      t.normals[0] = glm::vec4(model.raw_normals[i0], model.raw_normals[i0+1], model.raw_normals[i0+2], model.raw_normals[i0+3]);
      t.normals[1] = glm::vec4(model.raw_normals[i1], model.raw_normals[i1+1], model.raw_normals[i1+2], model.raw_normals[i1+3]);
      t.normals[2] = glm::vec4(model.raw_normals[i2], model.raw_normals[i2+1], model.raw_normals[i2+2], model.raw_normals[i2+3]);

      t.face_normal = model_triangle.face_normal;
    } else
    if (state.use_calculated_normals) {
      t.normals[0] = model.calculated_normals[model_triangle.indices[0]];
      t.normals[1] = model.calculated_normals[model_triangle.indices[1]];
      t.normals[2] = model.calculated_normals[model_triangle.indices[2]];

      t.face_normal = model_triangle.calculated_face_normal;
    } else {
      t.normals[0] = model.normals[model_triangle.indices[0]];
      t.normals[1] = model.normals[model_triangle.indices[1]];
      t.normals[2] = model.normals[model_triangle.indices[2]];

      t.face_normal = model_triangle.face_normal;
    }

    for (int i = 0; i < 3; i++) {
      if (state.enable_texture && model.has_texture) 
        color = glm::vec4(model_triangle.tex_coords[2*i], model_triangle.tex_coords[2*i+1], 1.0f, 1.0f);

      t.attrs[i].ww = 1.0f / t.vertices[i].w;

      t.attrs[i].ccs_position = (glm::inverse(projection_matrix) * t.vertices[i]) * t.attrs[i].ww;
      t.attrs[i].ccs_normal = (view_matrix * model_matrix * t.normals[i]) * t.attrs[i].ww;

      glm::vec4 c = state.debug_colors ? debug_colors[i] : color;
      t.attrs[i].color = c * t.attrs[i].ww; 
      t.attrs[i].flatColor = c;
      t.attrs[i].flatCcsNormal = view_matrix * model_matrix * t.face_normal;
    }

    this->triangles.push_back(t);
  }
}

void Close2GL_Scene::Rasterize(scene_state_t state, glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::mat4 viewport_matrix)
{
  for (triangle_t t : this->triangles)
  {
    for (int a = 0; a < 3; a++) {
      if (state.shading_mode != NO_SHADING)
        Shading(state, &(t.attrs[a]));
      if (state.shading_mode == FLAT_SHADING) {
        t.attrs[a].flatColor = t.attrs[0].flatColor;
        t.attrs[a].flatCcsNormal = t.attrs[0].flatCcsNormal;
      }
    }

    edge_t* edges = new edge_t[3];
    for (int e = 0; e < 3; e++)
    {
      int next_e = (e+1) % 3;
      edges[e] = FindEdge(
        t.mapped_vertices[e],      t.attrs[e],
        t.mapped_vertices[next_e], t.attrs[next_e]);
    }
    
    edges = OrderEdges(edges);
    
    int active_edge = 1;
    int max_inc = std::round(edges[0].vertex_delta.y);

    int y, x;    
    glm::vec4 p_a, p_b;
    interpolating_attr_t attr_a, attr_b;
    for (int inc_y = 0; inc_y < max_inc; inc_y++)
    {
      int inc = inc_y;

      // ARESTA PRINCIPAL
      p_a = edges[0].vertex_top;
      p_a.y = p_a.y + inc;
      p_a.x = p_a.x + inc * edges[0].inc_x;
      p_a.z = p_a.z + inc * edges[0].inc_z;
      y = std::round(p_a.y);
      x = std::round(p_a.x);
      
      attr_a = Interpolate(edges[0].bottom, edges[0].top, 0, max_inc, inc);

      glm::vec4 color;
      switch (state.shading_mode)
      {
        case FLAT_SHADING:
          color = t.attrs[0].flatColor;
          break;

        case PHONG_SHADING:
        case FLAT_PHONG_SHADING:
          Shading(state, &attr_a);

        case GOURAUD_SHADING:
        case NO_SHADING:
        default:
          color = attr_a.color / attr_a.ww;
      }
      color = glm::pow(color, glm::vec4(1.0)/2.2f);

      this->ChangeBuffer(state, x, y, p_a.z, vec4_to_rgba(color));

      // ARESTA SECUNDÁRIA
      
      if (active_edge == 2)
        inc -= edges[1].vertex_delta.y;

      p_b = edges[active_edge].vertex_top;
      p_b.y = p_b.y + inc;
      p_b.x = glm::clamp(p_b.x + inc * edges[active_edge].inc_x, edges[active_edge].min_x, edges[active_edge].max_x);
      p_b.z = p_b.z + inc * edges[active_edge].inc_z;
      y = std::round(p_b.y);
      x = std::round(p_b.x);

      if (std::abs(edges[active_edge].vertex_delta.y) < 0.5f)
      {
        scanline_t sl = FindScanline(
            edges[active_edge].vertex_top,    edges[active_edge].top, 
            edges[active_edge].vertex_bottom, edges[active_edge].bottom);
        this->RasterScanline(state, sl, view_matrix);
      }

      attr_b = Interpolate(edges[active_edge].bottom, edges[active_edge].top, 0, edges[active_edge].vertex_delta.y, inc);

      switch (state.shading_mode)
      {
        case FLAT_SHADING:
          color = t.attrs[0].flatColor;
          break;

        case PHONG_SHADING:
        case FLAT_PHONG_SHADING:
          Shading(state, &attr_b);

        case GOURAUD_SHADING:
        case NO_SHADING:
        default:
          color = attr_b.color / attr_b.ww;
      }
      color = glm::pow(color, glm::vec4(1.0)/2.2f);
      
      this->ChangeBuffer(state, x, y, p_b.z, vec4_to_rgba(color));
      
      // PREENCHIMENTO
      if (state.polygon_mode == GL_FILL)
      {
        float my = (p_a.y + p_b.y) / 2.0f;
        p_a.y = my;
        p_b.y = my;
        scanline_t sl = FindScanline(glm::make_vec4(p_a), attr_a, glm::make_vec4(p_b), attr_b);
        this->RasterScanline(state, sl, view_matrix);
      }

      if (active_edge == 1 && p_b.y > edges[1].vertex_bottom.y)
        active_edge = 2;
    }
  }
}

void Close2GL_Scene::RasterScanline(scene_state_t state, scanline_t line, glm::mat4 view_matrix)
{
  int max_inc = std::ceil(line.vertex_delta.x);
  int x, y;
  float z;
  y = std::round((line.vertex_top.y + line.vertex_bottom.y) / 2.0f);
  for (int inc_x = 0; inc_x < max_inc; inc_x++)
  {
    z = line.vertex_top.z + inc_x * line.inc_z;
    x = std::round(line.vertex_top.x + inc_x);

    interpolating_attr_t attr = Interpolate(line.bottom, line.top, 0, max_inc, inc_x);

    glm::vec4 color;
    switch (state.shading_mode)
    {
      case FLAT_SHADING:
        color = attr.flatColor;
        break;

      case PHONG_SHADING:
      case FLAT_PHONG_SHADING:
        Shading(state, &attr);

      case GOURAUD_SHADING:
      case NO_SHADING:
      default:
        color = attr.color / attr.ww;
    }
    color = glm::pow(color, glm::vec4(1.0)/2.2f);

    this->ChangeBuffer(state, x, y, z, vec4_to_rgba(color));
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


/* ==================== Close2GL AUXILIAR ====================== */

rgba_t vec4_to_rgba(glm::vec4 vec)
{
  rgba_t c;
  c.r = vec.x;
  c.g = vec.g;
  c.b = vec.b;
  c.a = vec.a;
  return c;
}

void EraseTriangleWithVertex(std::vector<model_triangle_t> *triangles, int index)
{
  auto it = triangles->begin();
  while (it != triangles->end())
    if (it->indices[0] == index || it->indices[1] == index || it->indices[2] == index)
      it = triangles->erase(it);
    else
      it++;
}

bool FaceCulling(glm::vec4 *vertices, int face_orientation)
{
  glm::vec4 v0 = vertices[0];
  glm::vec4 v1 = vertices[1];
  glm::vec4 v2 = vertices[2];

  float a = (v0.x*v1.y - v1.x*v0.y) + (v1.x*v2.y - v2.x*v1.y) + (v2.x*v0.y - v0.x*v2.y);

  return (face_orientation == GL_CW) && (a > 0) || (a < 0);
}

edge_t FindEdge(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr)
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
  
  e.min_x = std::min(v0.x, v1.x);
  e.max_x = std::max(v0.x, v1.x);

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

scanline_t FindScanline(glm::vec4 v0, interpolating_attr_t v0_attr, glm::vec4 v1, interpolating_attr_t v1_attr)
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

edge_t* OrderEdges(edge_t* edges)
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

interpolating_attr_t Interpolate(interpolating_attr_t attr_0, interpolating_attr_t attr_1, float min, float max, float value)
{
  float alpha = (value - min) / (max - min);
  interpolating_attr_t result;
  result.color        = alpha * attr_0.color        + (1.0f - alpha) * attr_1.color;
  result.ccs_normal   = alpha * attr_0.ccs_normal   + (1.0f - alpha) * attr_1.ccs_normal;
  result.ccs_position = alpha * attr_0.ccs_position + (1.0f - alpha) * attr_1.ccs_position;
  result.ww           = alpha * attr_0.ww           + (1.0f - alpha) * attr_1.ww;

  result.flatColor     = attr_0.flatColor;
  result.flatCcsNormal = attr_1.flatCcsNormal;
  return result;
}

glm::vec4 AmbientLighting(glm::vec4 color)
{
  return color * 0.2f;
}

glm::vec4 DiffuseLighting(glm::vec4 color, glm::vec4 ccs_normal, glm::vec4 ccs_position)
{
  glm::vec4 light_position = glm::vec4(2.0,2.0,2.0,1.0);
  glm::vec4 n = glm::normalize(ccs_normal);
  glm::vec4 l = glm::normalize(light_position - ccs_position);
  return color * std::max(0.0f, glm::dot(n, l));
}

glm::vec4 SpecularLighting(glm::vec4 ccs_normal, glm::vec4 ccs_position)
{
  glm::vec4 eye_position   = glm::vec4(0.0,0.0,0.0,1.0);
  glm::vec4 light_position = glm::vec4(2.0,2.0,2.0,1.0);
  glm::vec4 n = glm::normalize(ccs_normal);
  glm::vec4 l = glm::normalize(light_position - ccs_position);
  glm::vec4 v = glm::normalize(eye_position - ccs_position);
  float q = 120.0;
  glm::vec4 r = glm::normalize(2.0f * n * glm::dot(l,n) -l);
  glm::vec4 h = glm::normalize(v + l);
  return glm::vec4(0.5) * std::pow(std::max(0.0f, glm::dot(h, r)), q);
}

glm::vec4 Lighting(scene_state_t state, interpolating_attr_t attr, glm::vec4 normal)
{
  int lighting = state.lighting_mode;

  int count_terms = 0;
  glm::vec4 specular_term = glm::vec4(0.0);
  if (lighting >= SPECULAR_LIGHT) {
    lighting -= SPECULAR_LIGHT;
    specular_term = SpecularLighting(normal, attr.ccs_position);
    count_terms++;
  }

  glm::vec4 diffuse_term = glm::vec4(0.0);
  if (lighting >= DIFFUSE_LIGHT) {
    lighting -= DIFFUSE_LIGHT;
    diffuse_term = DiffuseLighting(attr.color, normal, attr.ccs_position);
    count_terms++;
  }
  
  glm::vec4 ambient_term = glm::vec4(0.0);
  if (lighting >= AMBIENT_LIGHT) {
    lighting -= AMBIENT_LIGHT;
    ambient_term = AmbientLighting(attr.color);
    count_terms++;
  }

  return ambient_term + diffuse_term + specular_term;
}

void Shading(scene_state_t state, interpolating_attr_t *attr)
{
  attr->ccs_position /= attr->ww;
  attr->ccs_normal /= attr->ww;
  attr->color /= attr->ww;

  glm::vec4 color;
  switch (state.shading_mode)
  {
    case FLAT_SHADING:
    case FLAT_PHONG_SHADING:
      color = Lighting(state, *attr, attr->flatCcsNormal);
      break;
      
    case GOURAUD_SHADING:
    case PHONG_SHADING:
      color = Lighting(state, *attr, attr->ccs_normal);
  }

  if (state.shading_mode == FLAT_SHADING)
    attr->flatColor = color;
  else
    attr->color = color;
  
  attr->ccs_position *= attr->ww;
  attr->ccs_normal *= attr->ww;
  attr->color *= attr->ww;
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
