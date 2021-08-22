
#include <GL3/gl3.h>
#include <GL3/gl3w.h>

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include "loaders.h"
#include "graphics/gpu_program.h"
#include "input.h"

#define BUFFER_OFFSET(a) ((void*)(a))

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];

const GLuint  NumVertices = 6;

Input input;
float g_ScreenRatio;

void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

int main( int argc, char** argv )
{

  int success = glfwInit();
  if (!success)
  {
    fprintf(stderr, "ERROR: glfwInit() failed.\n");
    std::exit(EXIT_FAILURE);
  }

  double last_time = glfwGetTime();
  
  glfwSetErrorCallback(ErrorCallback);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  #ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "CMP143", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
    std::exit(EXIT_FAILURE);
  }
  
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSetScrollCallback(window, ScrollCallback);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetWindowSize(window, 800, 600);

  glfwMakeContextCurrent(window);
  // Não sei pq tem essa linha
  gl3wInit();
  
  const GLubyte *vendor      = glGetString(GL_VENDOR);
  const GLubyte *renderer    = glGetString(GL_RENDERER);
  const GLubyte *glversion   = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

  printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

  // glEnable(GL_DEPTH_TEST); // Configurações importantes
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  // glFrontFace(GL_CCW);

  glGenVertexArrays(NumVAOs, VAOs);
  glBindVertexArray(VAOs[Triangles]);

  GLfloat  vertices[NumVertices][2] = {
      { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
      {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
  };

  glCreateBuffers(NumBuffers, Buffers);
  glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
  glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0);

  GpuProgram program = GpuProgram();
  program.shader_files = 
  {
    { GL_VERTEX_SHADER, "res/shaders/triangles.vs" },
    { GL_FRAGMENT_SHADER, "res/shaders/triangles.fs" },
  };
  CreateGpuProgram(&program);
  if (program.program_id == 0) {
    std::cerr << "Linking failed" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  glUseProgram(program.program_id);

  glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(vPosition);

  while (!glfwWindowShouldClose(window))
  {
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray(VAOs[Triangles]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (input.GetKeyState(GLFW_KEY_ESCAPE).is_pressed)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}

void ErrorCallback(int error, const char* description) 
{
  fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
  input.KeyCallback(key, action, mod);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  input.KeyCallback(button, action, mods);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
  input.cursor_state.xvalue = xpos;
  input.cursor_state.yvalue = ypos;
  input.cursor_changed = true;
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  input.scroll_state.xvalue = xoffset;
  input.scroll_state.yvalue = yoffset;
  input.scroll_changed = true;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  g_ScreenRatio = (float)width / height;
}
