
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
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

void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

void InitGLFW();
GLFWwindow* CreateGLFWwindow(int width, int height, const char* title);

ImGuiIO* InitImgui(GLFWwindow* window);

void UpdateMouseMove(double dt);
void UpdateMoveCamera(double dt);
void UpdateSceneColor(double dt);
void UpdateCameraPlanes(double dt);
void UpdateCameraFOV(double dt);

void ChangePolygonMode();
void ChangeFrontFaceMode();
void ChangeCameraType();
void ResetCamera();

#define POLYGON_MODES_SIZE 3
#define FRONT_FACE_MODES_SIZE 2
struct State_t
{
  float g_ScreenRatio;
  double g_LastCursorPosX, g_LastCursorPosY;

  glm::vec3 camera_initial_position = glm::vec3(0.0f, 0.0f, 0.0f);
  float camera_initial_farplane = 100.0f;

  const GLenum polygon_modes[POLYGON_MODES_SIZE] = {GL_FILL, GL_LINE, GL_POINT};
  unsigned int polygon_mode = 0;

  const GLenum front_face_modes[POLYGON_MODES_SIZE] = {GL_CCW, GL_CW};
  unsigned int front_face_mode = 0;

  glm::vec4 scene_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
} State;

void PrintMatrix(glm::mat4 M)
{
  printf("\n");
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][0], M[1][0], M[2][0], M[3][0]);
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][1], M[1][1], M[2][1], M[3][1]);
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][2], M[1][2], M[2][2], M[3][2]);
  printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n",
          M[0][3], M[1][3], M[2][3], M[3][3]);
}

int main( int argc, char* argv[] )
{
  char* model_filename = argv[1];
  if (model_filename == nullptr)
  {
    std::cout << "Please inform the path to the file you wish to view" << std::endl;
    std::exit(0);
  }

  InitGLFW();
  double last_time = glfwGetTime();

  GLFWwindow* window = CreateGLFWwindow(400, 400, "Model");

  gl3wInit();
  
  const GLubyte *vendor      = glGetString(GL_VENDOR);
  const GLubyte *renderer    = glGetString(GL_RENDERER);
  const GLubyte *glversion   = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

  ImGuiIO* io = InitImgui(window);

  GpuProgram program = GpuProgram();
  program.shader_files = 
  {
    { GL_VERTEX_SHADER, "../res/shaders/default.vs" },
    { GL_FRAGMENT_SHADER, "../res/shaders/default.fs" },
  };
  CreateGpuProgram(&program);
  if (program.program_id == 0) {
    std::cerr << "Linking failed" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  model_t model = ReadModelFile(model_filename);
  AddModelToScene(&current_scene, model);
  glm::vec3 bbox_center = (current_scene.bounding_box_max + current_scene.bounding_box_min) / 2.0f;
  current_scene.model_space *= glm::translate(-bbox_center);
  current_scene.bounding_box_max -= bbox_center;
  current_scene.bounding_box_min -= bbox_center;
  State.scene_color = glm::make_vec4(model.materials[0].diffuse);
  State.scene_color.w = 1.0f;

  float bbox_size = std::max(
    (current_scene.bounding_box_max.x - current_scene.bounding_box_min.x) * 2.0f,
    (current_scene.bounding_box_max.y - current_scene.bounding_box_min.y) * 2.0f
  ) / 2.0f;
  float distance = bbox_size / std::tan(camera.field_of_view / 2.0f);

  State.camera_initial_position = glm::vec3(0.0f, 0.0f, distance);
  camera.position = State.camera_initial_position;

  State.camera_initial_farplane = distance * 2.0f;
  camera.farplane = State.camera_initial_farplane;
  
  camera.camera_view = LOOK_FREE;

  glEnable(GL_DEPTH_TEST);                   
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);
  
  glLineWidth(1.5f);
  glPointSize(3.0f);

  double curr_time, dt;
  GLenum draw_style = GL_FILL;
  GLenum front_face = GL_CW;
  while (!glfwWindowShouldClose(window))
  {
    curr_time = glfwGetTime();
    dt = curr_time - last_time;

    if (input.GetKeyState(GLFW_KEY_ESCAPE).is_pressed)
      glfwSetWindowShouldClose(window, GL_TRUE);

    UpdateMouseMove(dt);
    UpdateMoveCamera(dt);
    UpdateSceneColor(dt);
    UpdateCameraPlanes(dt);
    UpdateCameraFOV(dt);

    ChangePolygonMode();
    ChangeFrontFaceMode();
    ChangeCameraType();
    ResetCamera();

    camera.Update();
    camera.screen_ratio = State.g_ScreenRatio;
    
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glm::mat4 view = camera.Camera_View();
    glm::mat4 model = current_scene.model_space;
    glm::mat4 mvp = camera.Camera_ViewProj() * model;
    glUseProgram(program.program_id);
    glUniformMatrix4fv(program.model_view_proj_uniform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(program.view_uniform, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(program.model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(program.color_uniform, 1 , glm::value_ptr(State.scene_color));

    DrawVirtualObject(current_scene, program.program_id);
    
    bool demo = true;
    ImGui::ShowDemoWindow(&demo);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    input.Update();
    glfwPollEvents();

    last_time = curr_time;
  }
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);

  glfwTerminate();
}

void InitGLFW()
{
  int success = glfwInit();
  if (!success)
  {
    fprintf(stderr, "ERROR: glfwInit() failed.\n");
    std::exit(EXIT_FAILURE);
  }
  
  glfwSetErrorCallback(ErrorCallback);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  #ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* CreateGLFWwindow(int width, int height, const char* title)
{
  GLFWwindow *window;
  window = glfwCreateWindow(800, 600, "CMP143", NULL, NULL);
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
  State.g_ScreenRatio = 4.0f/3.0f;

  glfwMakeContextCurrent(window);
  return window;
}

ImGuiIO* InitImgui(GLFWwindow* window) 
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO io = ImGui::GetIO(); (void)io;
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");
  return &io;
}

void UpdateMouseMove(double dt)
{
  KeyState left_button = input.GetKeyState(GLFW_MOUSE_BUTTON_LEFT);
  if (left_button.is_pressed) {
    State.g_LastCursorPosX = input.cursor_state.xvalue;
    State.g_LastCursorPosY = input.cursor_state.yvalue;
  }
  if (left_button.is_down && !left_button.is_pressed) {
    PairState cursor = input.cursor_state;
    float dx = cursor.xvalue - State.g_LastCursorPosX;
    float dy = cursor.yvalue - State.g_LastCursorPosY;

    float yaw = -dx * dt * 0.1;
    float pitch = -dy * dt * 0.1;
    camera.Rotate(pitch, yaw);

    State.g_LastCursorPosX = cursor.xvalue;
    State.g_LastCursorPosY = cursor.yvalue;
  }
}

void UpdateMoveCamera(double dt)
{
  glm::vec3 move_vector = glm::vec3(0.0f);
  KeyState key_w = input.GetKeyState(GLFW_KEY_W);
  KeyState key_a = input.GetKeyState(GLFW_KEY_A);
  KeyState key_s = input.GetKeyState(GLFW_KEY_S);
  KeyState key_d = input.GetKeyState(GLFW_KEY_D);
  glm::mat4 cam_space = glm::inverse(camera.camera_space);
  if (key_w.is_down)
  {
    glm::vec4 m = cam_space * FORWARD4;
    move_vector += glm::vec3(m.x, m.y, m.z);
  }
  if (key_a.is_down)
  {
    glm::vec4 m = cam_space * LEFT4;
    move_vector += glm::vec3(m.x, m.y, m.z);
  }
  if (key_s.is_down)
  {
    glm::vec4 m = cam_space * BACKWARD4;
    move_vector += glm::vec3(m.x, m.y, m.z);
  }
  if (key_d.is_down)
  {
    glm::vec4 m = cam_space * RIGHT4;
    move_vector += glm::vec3(m.x, m.y, m.z);
  }

  if (glm::length(move_vector) > 0.0f)
      camera.Move(glm::normalize(move_vector) * 5.0f * (float)dt);
}

void UpdateSceneColor(double dt)
{
  KeyState key_r = input.GetKeyState(GLFW_KEY_R);
  if (key_r.is_down) {
    if (key_r.modifiers == GLFW_MOD_SHIFT)
      State.scene_color.r = std::max(State.scene_color.r - 1.0f * (float)dt, 0.0f);
    else
      State.scene_color.r = std::min(State.scene_color.r + 1.0f * (float)dt, 1.0f);
  }
  KeyState key_g = input.GetKeyState(GLFW_KEY_G);
  if (key_g.is_down) {
    if (key_g.modifiers == GLFW_MOD_SHIFT)
      State.scene_color.g = std::max(State.scene_color.g - 1.0f * (float)dt, 0.0f);
    else
      State.scene_color.g = std::min(State.scene_color.g + 1.0f * (float)dt, 1.0f);
  }
  KeyState key_b = input.GetKeyState(GLFW_KEY_B);
  if (key_b.is_down) {
    if (key_b.modifiers == GLFW_MOD_SHIFT)
      State.scene_color.b = std::max(State.scene_color.b - 1.0f * (float)dt, 0.0f);
    else
      State.scene_color.b = std::min(State.scene_color.b + 1.0f * (float)dt, 1.0f);
  }
}

void UpdateCameraPlanes(double dt)
{
  KeyState key_z = input.GetKeyState(GLFW_KEY_Z);
  if (key_z.is_down) {
    if (key_z.modifiers == GLFW_MOD_SHIFT)
      camera.farplane = std::max(camera.farplane - (camera.farplane / 2.0f) * (float)dt, camera.nearplane + 0.1f);
    else
      camera.farplane = camera.farplane + (camera.farplane / 2.0f) * (float)dt;
  }
  KeyState key_x = input.GetKeyState(GLFW_KEY_X);
  if (key_x.is_down) {
    if (key_x.modifiers == GLFW_MOD_SHIFT)
      camera.nearplane = std::max(camera.nearplane - (camera.nearplane / 2.0f) * (float)dt, 0.1f);
    else
      camera.nearplane = std::min(camera.nearplane + (camera.nearplane / 2.0f) * (float)dt, camera.farplane - 0.1f);
  }
}

void UpdateCameraFOV(double dt)
{
  KeyState key_f = input.GetKeyState(GLFW_KEY_F);
  if (key_f.is_down) {
    if (key_f.modifiers == GLFW_MOD_SHIFT)
      camera.field_of_view = std::max(camera.field_of_view - PI/16.0f * (float)dt, PI/16.0f);
    else
      camera.field_of_view = std::min(camera.field_of_view + PI/16.0f * (float)dt, PI -0.001f);
  }
}

void ChangePolygonMode()
{
  KeyState key_q = input.GetKeyState(GLFW_KEY_Q);
  if (key_q.is_pressed)
  {
    if (key_q.modifiers == GLFW_MOD_SHIFT)
    {
      State.polygon_mode -= 1;
      if (State.polygon_mode < 0)
        State.polygon_mode = POLYGON_MODES_SIZE;
    }
    else
    {
      State.polygon_mode += 1;
      if (State.polygon_mode >= POLYGON_MODES_SIZE)
        State.polygon_mode = 0;
    }
    glPolygonMode(GL_FRONT_AND_BACK, State.polygon_modes[State.polygon_mode]);
  }
}

void ChangeFrontFaceMode()
{
  KeyState key_e = input.GetKeyState(GLFW_KEY_E);
  if (key_e.is_pressed)
  {
    if (key_e.modifiers == GLFW_MOD_SHIFT)
    {
      State.front_face_mode -= 1;
      if (State.front_face_mode < 0)
        State.front_face_mode = FRONT_FACE_MODES_SIZE;
    }
    else
    {
      State.front_face_mode += 1;
      if (State.front_face_mode >= FRONT_FACE_MODES_SIZE)
        State.front_face_mode = 0;
    }
    glFrontFace(State.front_face_modes[State.front_face_mode]);
  }
}

void ChangeCameraType()
{
  KeyState key_v = input.GetKeyState(GLFW_KEY_V);
  if (key_v.is_pressed) {
    if (camera.camera_view == LOOK_FREE)
      camera.camera_view = LOOK_AT;
    else if (camera.camera_view == LOOK_AT)
      camera.camera_view = LOOK_FREE;
  }
}

void ResetCamera()
{
  KeyState key_c = input.GetKeyState(GLFW_KEY_C);
  if (key_c.is_pressed)
  {
    camera.position = State.camera_initial_position;
    camera.farplane = State.camera_initial_farplane;
    camera.nearplane = 0.1f;
    camera.view_vector = FORWARD;
  }
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
  State.g_ScreenRatio = (float)width / height;
}
