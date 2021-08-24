
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <iostream>
#include "graphics/gpu_program.h"
#include "graphics/model.h"
#include "graphics/camera.h"
#include "input.h"
#include "scene.h"

Camera camera;
Input input;
scene_object_t current_scene;
float g_ScreenRatio;
double g_LastCursorPosX, g_LastCursorPosY;

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
  g_ScreenRatio = 4.0f/3.0f;

  glfwMakeContextCurrent(window);

  gl3wInit();
  
  const GLubyte *vendor      = glGetString(GL_VENDOR);
  const GLubyte *renderer    = glGetString(GL_RENDERER);
  const GLubyte *glversion   = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

  printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

  model_t cube = ReadModelFile("res/models/cube.in");
  AddModelToScene(&current_scene, cube);

  GpuProgram program = GpuProgram();
  program.shader_files = 
  {
    { GL_VERTEX_SHADER, "res/shaders/default.vs" },
    { GL_FRAGMENT_SHADER, "res/shaders/triangles.fs" },
  };
  CreateGpuProgram(&program);
  if (program.program_id == 0) {
    std::cerr << "Linking failed" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  camera.position = glm::vec3(-1.5f, 2.0f, -4.0f);
  camera.free = false;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  while (!glfwWindowShouldClose(window))
  {
    static const float black[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearBufferfv(GL_COLOR, 0, black);

    camera.screen_ratio = g_ScreenRatio;
    glm::mat4 view = camera.Camera_View();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = camera.Camera_ViewProj() * model;

    glUseProgram(program.program_id);
    glUniformMatrix4fv(program.model_view_proj_uniform, 
        1 , GL_FALSE , glm::value_ptr(mvp));
    glUniformMatrix4fv(program.view_uniform, 
        1 , GL_FALSE , glm::value_ptr(view));
    glUniformMatrix4fv(program.model_uniform, 
        1 , GL_FALSE , glm::value_ptr(model));

    DrawVirtualObject(current_scene, program.program_id);

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
  std::cerr << "ERROR: GLFW: " << description << std::endl;
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
