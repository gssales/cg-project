#ifndef _CLOSE2GL_H
#define _CLOSE2GL_H

#include "scene.h"

typedef struct
{
  std::vector<float> new_vertices;
  std::vector<float> new_normals;
  std::vector<float> projected;
} new_buffer_data;

bool c2gl_Culling(glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, GLint render_mode);
new_buffer_data c2gl_Transform_Model(model_t model, glm::mat4 mvp, glm::mat4 viewport_map, bool culling_enabled, GLint render_mode);
// void c2gl_Transform_Vertices(scene_object_t *obj, model_t model, glm::mat4 mvp, glm::mat4 viewport_map, bool culling_enabled, GLint render_mode);

#endif // CLOSE2GL_H_INCLUDED