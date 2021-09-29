#include "close2gl.h"

void insert_vec4_vector(std::vector<float>* vector, glm::vec4 v)
{
  vector->push_back(v.x);
  vector->push_back(v.y);
  vector->push_back(v.z);
  vector->push_back(v.w);
}

bool c2gl_Culling(glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, GLint render_mode)
{
  float a = (v0.x*v1.y - v1.x*v0.y) + (v1.x*v2.y - v2.x*v1.y) + (v2.x*v0.y - v0.x*v2.y);

  if (render_mode == GL_CW)
    return a > 0;
  else
    return a < 0;
}

new_buffer_data c2gl_Transform_Model(model_t model, glm::mat4 mvp, glm::mat4 viewport_map, bool culling_enabled, GLint render_mode)
{
  new_buffer_data buffer_data;

  std::vector<float>::iterator it = model.vertices.begin();
  std::vector<float>::iterator it_n = model.normals.begin();
  while (it != model.vertices.end())
  {
    glm::vec4 v0 = glm::vec4(    *it, *(it+1), *(it+2),  *(it+3));
    glm::vec4 v1 = glm::vec4(*(it+4), *(it+5), *(it+6),  *(it+7));
    glm::vec4 v2 = glm::vec4(*(it+8), *(it+9), *(it+10), *(it+11));

    glm::vec4 v0_ = mvp * v0;
    glm::vec4 v1_ = mvp * v1;
    glm::vec4 v2_ = mvp * v2;

    if (v0_.w <= 0 || v1_.w <= 0 || v2_.w <= 0)
    {
      it += 12;
      it_n += 12;
      continue;
    }
    v0_ /= v0_.w;
    v1_ /= v1_.w;
    v2_ /= v2_.w;

    if (culling_enabled)
    {
      bool is_front_facing = c2gl_Culling(viewport_map * v0_, viewport_map * v1_, viewport_map * v2_, render_mode);
      if (!is_front_facing)
      {
        it += 12;
        it_n += 12;
        continue;
      }
    }

    insert_vec4_vector(&buffer_data.projected, v0_);
    insert_vec4_vector(&buffer_data.projected, v1_);
    insert_vec4_vector(&buffer_data.projected, v2_);

    for (int i = 0; i < 12; i++)
    {
      buffer_data.new_normals.push_back(*(it_n+i));
      buffer_data.new_vertices.push_back(*(it+i));
    }

    it += 12;
    it_n += 12;
  }

  return buffer_data;
}

// void c2gl_Transform_Vertices(scene_object_t *obj, model_t model, glm::mat4 mvp, glm::mat4 viewport_map, bool culling_enabled, GLint render_mode)
// {
//   new_buffer_data bf_data = c2gl_Transform_Model(model, mvp, viewport_map, culling_enabled, render_mode);
//   std::vector<float> new_vx = bf_data.new_vertices;
//   std::vector<float> new_nl = bf_data.new_normals;
//   std::vector<float> proj = bf_data.projected;
  
//   obj->first_index = 0;
//   obj->vertex_count = new_vx.size();
  
//   glBindVertexArray(obj->vao_id);

//   GLint number_of_dimensions = 4;
//   GLint location = 0;
//   glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_model_id);
//   glBufferData(GL_ARRAY_BUFFER, new_vx.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
//   glBufferSubData(GL_ARRAY_BUFFER, 0, new_vx.size() * sizeof(GL_FLOAT), new_vx.data());
//   glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
//   glEnableVertexAttribArray(location);
//   glBindBuffer(GL_ARRAY_BUFFER, 0);

//   location = 1;
//   glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_normal_id);
//   glBufferData(GL_ARRAY_BUFFER, new_nl.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
//   glBufferSubData(GL_ARRAY_BUFFER, 0, new_nl.size() * sizeof(GL_FLOAT), new_nl.data());
//   glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
//   glEnableVertexAttribArray(location);
//   glBindBuffer(GL_ARRAY_BUFFER, 0);
  
//   location = 2;
//   glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_projected_id);
//   glBufferData(GL_ARRAY_BUFFER, proj.size() * sizeof(GL_FLOAT), NULL, GL_DYNAMIC_DRAW);
//   glBufferSubData(GL_ARRAY_BUFFER, 0, proj.size() * sizeof(GL_FLOAT), proj.data());
//   glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
//   glEnableVertexAttribArray(location);
//   glBindBuffer(GL_ARRAY_BUFFER, 0);

//   glBindVertexArray(0);
// }