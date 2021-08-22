#include "graphics/gpu_program.h"

void LinkGpuProgram(GpuProgram* gpu_program)
{
  GLuint program_id = glCreateProgram();

  std::list<ShaderInfo>::iterator it = gpu_program->shader_files.begin();
  while (it != gpu_program->shader_files.end()) {
    glAttachShader(program_id, it->shader_id);
    ++it;
  }

  glLinkProgram(program_id);

  GLint linked_ok = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

  if ( linked_ok == GL_FALSE )
  {
    GLint log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

    GLchar* log = new GLchar[log_length];

    glGetProgramInfoLog(program_id, log_length, &log_length, log);

    std::cerr << "ERROR: OpenGL linking of program failed.\n";
    std::cerr << "== Start of link log\n";
    std::cerr << log;
    std::cerr << "\n== End of link log\n";
    std::cerr << std::endl;

    delete [] log;

    std::list<ShaderInfo>::iterator it = gpu_program->shader_files.begin();
    while (it != gpu_program->shader_files.end()) {
      glDeleteShader(it->shader_id);
      ++it;
    }
    gpu_program->program_id = 0;
  }
  else 
  {
    gpu_program->program_id = program_id;
  }
}

int PrintGLSL_Logs(const char* filename, unsigned int shader_id)
{
  int compiled_ok;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

  int log_length = 0;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

  char* log = new GLchar[log_length];
  glGetShaderInfoLog(shader_id, log_length, &log_length, log);

  if ( log_length != 0 )
  {
    if ( !compiled_ok ) {
      std::clog << "ERROR: ";
    } else {
      std::clog << "WARNING: ";
    }
    std::clog << "OpenGL compilation of \"";
    std::clog << filename;
    std::clog << "\" failed.\n";
    std::clog << "== Start of compilation log\n";
    std::clog << log;
    std::clog << "== End of compilation log\n";
    std::clog << std::endl;
  }

  delete [] log;
  return compiled_ok;
}

void CompileShader(const char* filename, unsigned int shader_id)
{
  std::string file_string;
  try {
    file_string = ReadFileContent(filename);
  } catch ( std::exception& e ) {
    throw e;
  }
  
  const char* shader_string = file_string.c_str();
  const int   shader_string_length = static_cast<int>( file_string.length() );

  glShaderSource(shader_id, 1, &shader_string, &shader_string_length);
  delete [] shader_string;
  glCompileShader(shader_id);
  
  int compiled_status = PrintGLSL_Logs(filename, shader_id);
  
  if (!compiled_status) {
    throw std::runtime_error("Shader compilation failed");
  }
}

void LoadShader(ShaderInfo* shader)
{
  unsigned int shader_id = glCreateShader(shader->type);
  
  try {
    CompileShader(shader->filename, shader_id);
  } catch ( std::exception& e ) {
    glDeleteShader(shader_id);
    shader_id = 0;
  }

  shader->shader_id = shader_id;
}

void LoadShaderFiles(std::list<ShaderInfo>* shaders) 
{
  std::list<ShaderInfo>::iterator it = (*shaders).begin();
  while (it != (*shaders).end()) {
    ShaderInfo* shader = &*it;

    LoadShader(shader);

    it++;
  }
}

void CreateGpuProgram(GpuProgram* gpu_program) 
{
  LoadShaderFiles(&(gpu_program->shader_files));

  if ( gpu_program->program_id != 0 )
    glDeleteProgram(gpu_program->program_id);

  LinkGpuProgram(gpu_program);

  gpu_program->model_view_proj_uniform 
    = glGetUniformLocation(gpu_program->program_id, "model_view_proj");
}