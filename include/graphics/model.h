#ifndef _MODEL_H
#define _MODEL_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glm/vec4.hpp>

#include "matrices.h"

typedef struct {
  float ambient[3];
  float diffuse[3];
  float specular[3];
  float shininess;
} material_t;

typedef struct
{
  int indices[3];
  glm::vec4 face_normal;
  glm::vec4 calculated_face_normal;
  float tex_coords[6];
} model_triangle_t;

typedef struct {
  std::string model_name;

  bool has_texture = false;
  std::vector<model_triangle_t> triangles;
  std::vector<glm::vec4> vertices; // vértices não repetidos
  std::vector<glm::vec4> normals; // normais de cada vértice
  std::vector<glm::vec4> calculated_normals;
  std::vector<float> raw_normals; // normais de cada vértice

  std::vector<material_t> materials;
  std::vector<unsigned int> color_indices;

  glm::vec3    bounding_box_min = glm::vec3(0.0f);
  glm::vec3    bounding_box_max = glm::vec3(0.0f);
} model_t;


model_t ReadModelFile(const char* filename);
void CalculateNormals(model_t *model, bool ccw_face);

// Because the old code used the vertex list in this format, I added these 
// functions in order to mantain compatibility
std::vector<float> ExtractVertices(model_t model);
std::vector<float> ExtractNormals(model_t model);
std::vector<float> ExtractCalculatedNormals(model_t model);
std::vector<float> ExtractSurfaceNormals(model_t model);
std::vector<float> ExtractCalculatedSurfaceNormals(model_t model);

#endif // _MODEL_H
