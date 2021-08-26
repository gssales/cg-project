#ifndef _MODEL_H
#define _MODEL_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glm/vec3.hpp>

typedef struct {
  float ambient[3];
  float diffuse[3];
  float specular[3];
  float shininess;
} material_t;

typedef struct {
  std::string model_name;
  std::vector<material_t> materials;
  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<float> face_normals;
  std::vector<unsigned int> color_indices;
  std::vector<unsigned int> indices;
  glm::vec3    bounding_box_min = glm::vec3(0.0f);
  glm::vec3    bounding_box_max = glm::vec3(0.0f);
} model_t;

model_t ReadModelFile(const char* filename);

#endif // _MODEL_H
