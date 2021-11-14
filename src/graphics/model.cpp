#include "graphics/model.h"

using namespace std;

model_t ReadModelFile(const char* filename)
{ 
  ifstream file;
  try {
    file.exceptions(ifstream::failbit);
    file.open(filename);
  } catch ( exception& e ) {
    cerr << "ERROR: Cannot open file \"" << filename << "\"." << endl;
    throw e;
  }

  model_t model;

  string str;
  
  file >> str >> str >> str >> model.model_name;

  int triangle_count;
  file >> str >> str >> str >> triangle_count;

  int material_count;
  file >> str >> str >> str >> material_count;

  for (int i = 0; i < material_count; ++i)
  {
    material_t material;
    file >> str >> str 
      >> material.ambient[0] >> material.ambient[1] >> material.ambient[2];
    file >> str >> str 
      >> material.diffuse[0] >> material.diffuse[1] >> material.diffuse[2];
    file >> str >> str 
      >> material.specular[0] >> material.specular[1] >> material.specular[2];
    file >> str >> str >> material.shininess;
    model.materials.push_back(material);
  }

  file >> str >> str >> str;
  model.has_texture = str == "YES";
  
  getline(file, str); 
  getline(file, str);
  
  for (int a = 0; a < triangle_count; ++a)
  {
    model_triangle_t triangle;
    for (int v = 0; v < 3; ++v)
    {
      float vx, vy, vz, nx, ny, nz, s, t;
      int color_index;

      file >> str >> vx >> vy >> vz >> nx >> ny >> nz >> color_index;
      if (model.has_texture)
        file >> s >> t;
      
      glm::vec4 vertex = glm::vec4(vx, vy, vz, 1.0f);

      int index = -1;
      for (int i = 0; i < model.vertices.size(); i++)
        if (vertex == model.vertices[i])
          index = i;
      if (index == -1) {
        index = model.vertices.size();
        model.vertices.push_back(vertex);

        glm::vec4 normal = glm::vec4(vx, vy, vz, 0.0f);
        model.normals.push_back(normal);
        
        model.color_indices.push_back(color_index);   
      }

      triangle.indices[v] = index;

      if (model.has_texture)
        triangle.tex_coords[v*2]   = s;
        triangle.tex_coords[v*2+1] = t;

      model.raw_normals.push_back(nx);
      model.raw_normals.push_back(ny);
      model.raw_normals.push_back(nz);
      model.raw_normals.push_back(0.0f);
    
      model.bounding_box_min.x = std::min(model.bounding_box_min.x, vx);
      model.bounding_box_min.y = std::min(model.bounding_box_min.y, vy);
      model.bounding_box_min.z = std::min(model.bounding_box_min.z, vz);
      model.bounding_box_max.x = std::max(model.bounding_box_max.x, vx);
      model.bounding_box_max.y = std::max(model.bounding_box_max.y, vy);
      model.bounding_box_max.z = std::max(model.bounding_box_max.z, vz);    
    }
    
    float fnx, fny, fnz;
    file >> str >> str >> fnx >> fny >> fnz;

    triangle.face_normal = glm::vec4(fnx, fny, fnz, 0.0);

    model.triangles.push_back(triangle);
  }
  file.close();

  return model;
}

void CalculateNormals(model_t *model, bool ccw_face)
{
  for (std::vector<model_triangle_t>::iterator it = model->triangles.begin();
       it != model->triangles.end(); ++it) {
    
    glm::vec4 v0 = model->vertices[it->indices[0]];
    glm::vec4 v1 = model->vertices[it->indices[1]];
    glm::vec4 v2 = model->vertices[it->indices[2]];

    glm::vec4 u = ccw_face ? v1 - v0 : v2 - v0;
    glm::vec4 v = ccw_face ? v2 - v0 : v1 - v0;

    it->calculated_face_normal = matrices::cw_surface_normal(u, v);
  }

  model->calculated_normals.clear();
  for (int i = 0; i < model->vertices.size(); i++) {
    glm::vec4 calculated_normal = glm::vec4(0.0);
    int count_triangles = 0;

    for (model_triangle_t t : model->triangles)
      if (t.indices[0] == i || t.indices[1] == i || t.indices[2] == i) {
        calculated_normal += t.calculated_face_normal;
        count_triangles++;
      }

    calculated_normal = glm::normalize(calculated_normal/(float)count_triangles);
    model->calculated_normals.push_back(calculated_normal);
  }

  for (std::vector<model_triangle_t>::iterator it = model->triangles.begin();
       it != model->triangles.end(); ++it) {
    it->calculated_face_normal = glm::normalize(it->calculated_face_normal);
  }
}

std::vector<float> ExtractVertices(model_t model)
{
  std::vector<float> vertices;
  for (model_triangle_t t : model.triangles)
    for (int v = 0; v < 3; v++) {
      glm::vec4 vertex = model.vertices[t.indices[v]];
      vertices.push_back(vertex.x);
      vertices.push_back(vertex.y);
      vertices.push_back(vertex.z);
      vertices.push_back(vertex.w);
    }
  return vertices;
}

std::vector<float> ExtractNormals(model_t model)
{
  std::vector<float> normals;
  for (model_triangle_t t : model.triangles)
    for (int n = 0; n < 3; n++) {
      glm::vec4 normal = model.normals[t.indices[n]];
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
      normals.push_back(normal.w);
    }
  return normals;
}
std::vector<float> ExtractCalculatedNormals(model_t model)
{
  std::vector<float> normals;
  for (model_triangle_t t : model.triangles)
    for (int n = 0; n < 3; n++) {
      glm::vec4 normal = model.calculated_normals[t.indices[n]];
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
      normals.push_back(normal.w);
    }
  return normals;
}
std::vector<float> ExtractSurfaceNormals(model_t model)
{
  std::vector<float> normals;
  for (model_triangle_t t : model.triangles) {
    glm::vec4 normal = t.face_normal;
    for (int n = 0; n < 3; n++) {
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
      normals.push_back(normal.w);
    }
  }
  return normals;
}
std::vector<float> ExtractCalculatedSurfaceNormals(model_t model)
{
  std::vector<float> normals;
  for (model_triangle_t t : model.triangles) {
    glm::vec4 normal = t.calculated_face_normal;
    for (int n = 0; n < 3; n++) {
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
      normals.push_back(normal.w);
    }
  }
  return normals;
}