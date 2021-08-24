#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

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
} model_t;

model_t ReadModelFile(const char* filename);

#endif // _MODEL_H
