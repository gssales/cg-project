#ifndef _GPU_PROGRAM_H
#define _GPU_PROGRAM_H

#include <iostream>
#include <list>
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include "loaders.h"

typedef struct {
    unsigned int type;
    const char* filename;
    unsigned int shader_id;
} ShaderInfo;

class GpuProgram {
  public:
    std::list<ShaderInfo> shader_files;
    unsigned int program_id;
    int model_view_proj_uniform;
    int model_uniform;
    int view_uniform;
    int color_uniform;
};

void CreateGpuProgram(GpuProgram* gpu_program);

#endif // _GPU_PROGRAM_H