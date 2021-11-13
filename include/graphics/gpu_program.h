#ifndef _GPU_PROGRAM_H
#define _GPU_PROGRAM_H

#include <iostream>
#include <list>
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include "loaders.h"

#define NO_SHADING 0
#define FLAT_SHADING 1
#define GOURAUD_SHADING 2
#define PHONG_SHADING 3
#define FLAT_PHONG_SHADING 4 // Calculates phong lighting on surface normals

#define AMBIENT_LIGHT 1
#define DIFFUSE_LIGHT 2
#define SPECULAR_LIGHT 4

typedef struct {
    unsigned int type;
    const char* filename;
    unsigned int shader_id;
} ShaderInfo;

class GpuProgram {
  public:
    std::list<ShaderInfo> shader_files;
    unsigned int program_id;
};

class OpenGL_GpuProgram : public GpuProgram {
  public:
    int model_view_proj_uniform;
    int model_uniform;
    int view_uniform;
    int color_uniform;
    int shading_uniform;
    int lighting_uniform;
};

class Close2GL_GpuProgram : public GpuProgram {
  public:
    int texture_uniform;
};

void CreateGpuProgram(OpenGL_GpuProgram* gpu_program);
void CreateGpuProgram(Close2GL_GpuProgram* gpu_program);

#endif // _GPU_PROGRAM_H