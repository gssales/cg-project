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

  getline(file, str); 
  getline(file, str);

  for (int t = 0; t < triangle_count; ++t)
  {
    for (int v = 0; v < 3; ++v)
    {
      float vx, vy, vz, nx, ny, nz;
      int color_index;
      file >> str >> vx >> vy >> vz >> nx >> ny >> nz >> color_index;
    
      model.bounding_box_min.x = std::min(model.bounding_box_min.x, vx);
      model.bounding_box_min.y = std::min(model.bounding_box_min.y, vy);
      model.bounding_box_min.z = std::min(model.bounding_box_min.z, vz);
      model.bounding_box_max.x = std::max(model.bounding_box_max.x, vx);
      model.bounding_box_max.y = std::max(model.bounding_box_max.y, vy);
      model.bounding_box_max.z = std::max(model.bounding_box_max.z, vz);

      model.vertices.push_back(vx);
      model.vertices.push_back(vy);
      model.vertices.push_back(vz);
      model.vertices.push_back(1.0f);
      model.normals.push_back(nx);
      model.normals.push_back(ny);
      model.normals.push_back(nz);
      model.normals.push_back(0.0f);
      model.color_indices.push_back(color_index);      
      model.indices.push_back(t*3 + v);      
    }
    
    float fnx, fny, fnz;
    file >> str >> str >> fnx >> fny >> fnz;

    model.face_normals.push_back(fnx);
    model.face_normals.push_back(fny);
    model.face_normals.push_back(fnz);
    model.face_normals.push_back(0.0f);
  }
  file.close();

  return model;
}
  