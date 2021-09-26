#include <algorithm>
#include <iostream>

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/gpu_program.h"
#include "graphics/model.h"
#include "graphics/camera.h"
#include "input.h"
#include "scene.h"
#include "close2gl.h"

Camera g_Camera;
Input g_Input;
scene_object_t g_Scene;
model_t g_Model;

void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

void InitGLFW();
GLFWwindow* CreateGLFWwindow(int width, int height, const char* title);

void InitImgui(GLFWwindow* window);

void UpdateMouseMove(double dt);
void UpdateMoveCamera(double dt);
void UpdateCamera_SeparateControls(double dt);
void GenerateGUI(double dt);

void OpenObjectFile();

void ResetCamera();

#define CAMERA_CONTROLS_TRANSLATE 0
#define CAMERA_CONTROLS_ROTATE 1
#define CAMERA_CONTROLS_X_AXIS 2
#define CAMERA_CONTROLS_Y_AXIS 3
#define CAMERA_CONTROLS_Z_AXIS 4
#define USE_OPENGL 0
#define USE_CLOSE2GL 1
struct State_t
{
  float screen_width, screen_height, screen_ratio;
  double lastCursorPosX, lastCursorPosY;

  bool model_loaded = false;

  glm::vec3 camera_initial_position = glm::vec3(0.0f, 0.0f, 0.0f);
  float camera_initial_farplane = 1000.0f;
  float camera_initial_nearplane = 0.1f;

  int polygon_mode = GL_FILL;
  int front_face_mode = GL_CW;

  bool backface_culling = true;
  bool look_at = false;
  bool camera_separate_controls = true;
  int camera_control_action = CAMERA_CONTROLS_TRANSLATE;
  int camera_control_axis = CAMERA_CONTROLS_Z_AXIS;
  bool close = false;

  char model_filename[512] = "..\\res\\models\\cube.in";

  int shading_mode = PHONG_SHADING;
  bool lights_on = true;
  int use_api = USE_OPENGL;

  float gui_object_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
} State;

int main( int argc, char* argv[] )
{
  InitGLFW();
  double last_time = glfwGetTime();

  GLFWwindow* window = CreateGLFWwindow(400, 400, "Model");

  gl3wInit();
  
  const GLubyte *vendor      = glGetString(GL_VENDOR);
  const GLubyte *renderer    = glGetString(GL_RENDERER);
  const GLubyte *glversion   = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

  InitImgui(window);

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

  g_Camera.camera_view = LOOK_FREE;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);
  
  glLineWidth(1.5f);
  glPointSize(3.0f);

  double curr_time, dt;
  double update_fps = 0.0;
  unsigned int count_frames = 0;
  while (!glfwWindowShouldClose(window))
  {
    curr_time = glfwGetTime();
    dt = curr_time - last_time;

    update_fps += dt;
    count_frames++;
     if ( update_fps >= 0.5 ){
      double fps = double(count_frames) / dt;

      std::stringstream ss;
      ss << "Model - " << (State.use_api == USE_OPENGL ? "OpenGL" : "Close2GL") << " [" << fps << " FPS]";

      glfwSetWindowTitle(window, ss.str().c_str());

      count_frames = 0;
      update_fps = 0.0;
     }

    if (g_Input.GetKeyState(GLFW_KEY_ESCAPE).is_pressed || State.close)
      glfwSetWindowShouldClose(window, GL_TRUE);

    g_Camera.Update();

    if (State.camera_separate_controls)
    {
      UpdateCamera_SeparateControls(dt);
    }
    else
    {
      UpdateMouseMove(dt);
      UpdateMoveCamera(dt);
    }

    if (State.look_at)
      g_Camera.camera_view = LOOK_AT;
    else
      g_Camera.camera_view = LOOK_FREE;

    g_Camera.screen_ratio = State.screen_ratio;
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPolygonMode(GL_FRONT_AND_BACK, State.polygon_mode);

    if (State.use_api == USE_CLOSE2GL)
      glDisable(GL_CULL_FACE);
    else 
    {
      if (State.backface_culling )
        glEnable(GL_CULL_FACE);
      else
        glDisable(GL_CULL_FACE);
        
      glFrontFace(State.front_face_mode);
    }
    
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (State.model_loaded)
    {
      glm::mat4 view = g_Camera.Camera_View();
      glm::mat4 proj = g_Camera.Camera_Projection();
      glm::mat4 model = g_Scene.model_space;
      glm::mat4 mvp = g_Camera.Camera_ViewProj() * model;
      GLint vp[4];
      glGetIntegerv(GL_VIEWPORT, vp);
      glm::mat4 viewport_map = matrices::viewport(vp[0], vp[1], vp[2], vp[3]);
      
      glUseProgram(program.program_id);
      glUniformMatrix4fv(program.model_view_proj_uniform, 1, GL_FALSE, glm::value_ptr(mvp));
      glUniformMatrix4fv(program.view_uniform, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(program.model_uniform, 1, GL_FALSE, glm::value_ptr(model));
      glUniform4fv(program.color_uniform, 1 , State.gui_object_color);
      glUniform1i(program.shading_uniform, State.shading_mode);
      glUniform1i(program.lighting_uniform, State.lights_on);
      glUniform1i(program.close2gl_uniform, State.use_api);

      if (State.use_api == USE_CLOSE2GL)
        c2gl_Transform_Vertices(&g_Scene, g_Model, mvp, viewport_map, State.backface_culling, State.front_face_mode);

      DrawVirtualObject(g_Scene, program.program_id);
    }
    
    GenerateGUI(dt);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    g_Input.Update();
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
  State.screen_ratio = 4.0f/3.0f;

  glfwMakeContextCurrent(window);
  return window;
}

void InitImgui(GLFWwindow* window) 
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

void UpdateMouseMove(double dt)
{
  if (ImGui::GetIO().WantCaptureMouse == true)
    return;

  KeyState left_button = g_Input.GetKeyState(GLFW_MOUSE_BUTTON_LEFT);
  if (left_button.is_pressed) {
    State.lastCursorPosX = g_Input.cursor_state.xvalue;
    State.lastCursorPosY = g_Input.cursor_state.yvalue;
  }
  if (left_button.is_down && !left_button.is_pressed) {
    PairState cursor = g_Input.cursor_state;
    float dx = cursor.xvalue - State.lastCursorPosX;
    float dy = cursor.yvalue - State.lastCursorPosY;

    float yaw = -dx * dt * 0.1;
    float pitch = -dy * dt * 0.1;
    g_Camera.Rotate(pitch, yaw);

    State.lastCursorPosX = cursor.xvalue;
    State.lastCursorPosY = cursor.yvalue;
  }
}

void UpdateMoveCamera(double dt)
{
  glm::vec3 move_vector = glm::vec3(0.0f);
  KeyState key_w = g_Input.GetKeyState(GLFW_KEY_W);
  KeyState key_a = g_Input.GetKeyState(GLFW_KEY_A);
  KeyState key_s = g_Input.GetKeyState(GLFW_KEY_S);
  KeyState key_d = g_Input.GetKeyState(GLFW_KEY_D);
  glm::mat4 cam_space = glm::inverse(g_Camera.camera_space);
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
      g_Camera.Move(glm::normalize(move_vector) * 5.0f * (float)dt);
}

void UpdateCamera_SeparateControls(double dt)
{
  if (ImGui::GetIO().WantCaptureMouse == true)
    return;

  KeyState left_button = g_Input.GetKeyState(GLFW_MOUSE_BUTTON_LEFT);
  if (left_button.is_pressed) {
    State.lastCursorPosX = g_Input.cursor_state.xvalue;
    State.lastCursorPosY = g_Input.cursor_state.yvalue;
  }

  if (left_button.is_down && !left_button.is_pressed) {
    PairState cursor = g_Input.cursor_state;
    float dy = cursor.yvalue - State.lastCursorPosY;

    glm::mat4 cam_space = glm::inverse(g_Camera.camera_space);
    glm::vec3 axis;
    switch (State.camera_control_axis)
    {
    case CAMERA_CONTROLS_X_AXIS:
      axis = cam_space * LEFT4;
      break;      
    case CAMERA_CONTROLS_Y_AXIS:
      axis = cam_space * UP4;
      break;
    default:
      axis = cam_space * FORWARD4;
      break;
    }

    if (State.camera_control_action == CAMERA_CONTROLS_TRANSLATE)
    {
      float speed = dy * 0.1f * matrices::length(cam_space * ORIGIN4)/2.0f * dt;
      g_Camera.Move(axis * speed);
    }
    else if (State.camera_control_action == CAMERA_CONTROLS_ROTATE)
    {
      PrintVec3(axis);
      float angle = -dy * dt * 0.1;
      g_Camera.Rotate(axis, angle);
    }

    State.lastCursorPosX = cursor.xvalue;
    State.lastCursorPosY = cursor.yvalue;
  }
}

void GenerateGUI(double dt)
{
  ImGui::Begin("Controls");

  ImGui::Text("Render Mode");

  if (ImGui::RadioButton("OpenGL", &State.use_api, USE_OPENGL))
    UseOpenGL(&current_scene, current_model); 
  ImGui::SameLine();
  ImGui::RadioButton("Close2GL", &State.use_api, USE_CLOSE2GL);

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::RadioButton("Points", &State.polygon_mode, GL_POINT);
  ImGui::RadioButton("Wireframe", &State.polygon_mode, GL_LINE);
  ImGui::RadioButton("Solid", &State.polygon_mode, GL_FILL);

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::Text("Shading");
  ImGui::RadioButton("Gouraud AD", &State.shading_mode, GOURAUD_AD_SHADING); ImGui::SameLine();
  ImGui::RadioButton("Gouraud ADS", &State.shading_mode, GOURAUD_ADS_SHADING);
  ImGui::RadioButton("Phong", &State.shading_mode, PHONG_SHADING); ImGui::SameLine();
  ImGui::RadioButton("None", &State.shading_mode, NO_SHADING);
  
  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::Checkbox("Lights On", &State.lights_on);

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::Checkbox("Backface Culling", &State.backface_culling);

  ImGui::Text("Orientation");
  ImGui::RadioButton("CW", &State.front_face_mode, GL_CW); ImGui::SameLine();
  ImGui::RadioButton("CCW", &State.front_face_mode, GL_CCW);

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::Separator();
  ImGui::Text("Camera");

  ImGui::DragFloat("HFov", &camera.h_fov, PI/16.0f * dt, PI/16.0f, PI);
  ImGui::DragFloat("VFov", &camera.v_fov, PI/16.0f * dt, PI/16.0f, PI);
  
  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::DragFloat("Near plane", &camera.nearplane, (camera.nearplane / 2.0f) * dt, 
      std::numeric_limits<float>::epsilon(), 
      camera.farplane - std::numeric_limits<float>::epsilon());
  ImGui::DragFloat("Far plane", &camera.farplane, (camera.farplane / 2.0f) * dt, 
      camera.nearplane + std::numeric_limits<float>::epsilon(), 
      std::numeric_limits<float>::max());

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::Checkbox("Separate Camera Controls", &State.camera_separate_controls);
  ImGui::Checkbox("Look at Object", &State.look_at);

  if (!State.camera_separate_controls)
    ImGui::BeginDisabled();

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::RadioButton("Translate", &State.camera_control_action, CAMERA_CONTROLS_TRANSLATE);
  ImGui::RadioButton("Rotate", &State.camera_control_action, CAMERA_CONTROLS_ROTATE);

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::RadioButton("X", &State.camera_control_axis, CAMERA_CONTROLS_X_AXIS); ImGui::SameLine();
  ImGui::RadioButton("Y", &State.camera_control_axis, CAMERA_CONTROLS_Y_AXIS); ImGui::SameLine();
  ImGui::RadioButton("Z", &State.camera_control_axis, CAMERA_CONTROLS_Z_AXIS);

  if (!State.camera_separate_controls)
    ImGui::EndDisabled();

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  if (ImGui::Button("Reset Camera"))
    ResetCamera();

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::Separator();
  ImGui::Text("Model");
  ImGui::ColorEdit4("Object Color", State.gui_object_color);

  ImGui::Dummy(ImVec2(0.0f, 5.0f));
  ImGui::InputText("File Path", State.model_filename, IM_ARRAYSIZE(State.model_filename));
  if (ImGui::Button("Open file"))
    OpenObjectFile();

  ImGui::Dummy(ImVec2(0.0f, 10.0f));
  State.close = ImGui::Button("Close");

  ImGui::End();
}

void OpenObjectFile()
{
  try {
    g_Model = ReadModelFile(State.model_filename);
    AddModelToScene(&g_Scene, g_Model);
    glm::vec3 bbox_center = (g_Scene.bounding_box_max + g_Scene.bounding_box_min) / 2.0f;
    g_Scene.model_space *= glm::translate(-bbox_center);
    g_Scene.bounding_box_max -= bbox_center;
    g_Scene.bounding_box_min -= bbox_center;

    State.gui_object_color[0] = g_Model.materials[0].diffuse[0];
    State.gui_object_color[1] = g_Model.materials[0].diffuse[1];
    State.gui_object_color[2] = g_Model.materials[0].diffuse[2];
    State.gui_object_color[3] = 1.0f;

    float bbox_size = std::max(
      (g_Scene.bounding_box_max.x - g_Scene.bounding_box_min.x) * 2.0f,
      (g_Scene.bounding_box_max.y - g_Scene.bounding_box_min.y) * 2.0f
    ) / 2.0f;
    float distance = bbox_size / std::tan(g_Camera.h_fov / 2.0f);

    State.camera_initial_position = glm::vec3(0.0f, 0.0f, distance);
    g_Camera.position = State.camera_initial_position;

    State.camera_initial_farplane = distance * 2.0f;
    g_Camera.farplane = State.camera_initial_farplane;

    State.model_loaded = true;
  } catch ( std::exception& e ) {
    
  }
}

void ResetCamera()
{
  g_Camera.position = State.camera_initial_position;
  g_Camera.farplane = State.camera_initial_farplane;
  g_Camera.nearplane = State.camera_initial_nearplane;
  g_Camera.view_vector = FORWARD;
  g_Camera.up_vector = UP;
  g_Camera.camera_view = LOOK_FREE;
  g_Camera.h_fov = PI / 3.0f;
  g_Camera.v_fov = PI / 3.0f;
}

void ErrorCallback(int error, const char* description) 
{
  std::cerr << "ERROR: GLFW: " << description << std::endl;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
  g_Input.KeyCallback(key, action, mod);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  g_Input.KeyCallback(button, action, mods);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
  g_Input.cursor_state.xvalue = xpos;
  g_Input.cursor_state.yvalue = ypos;
  g_Input.cursor_changed = true;
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  g_Input.scroll_state.xvalue = xoffset;
  g_Input.scroll_state.yvalue = yoffset;
  g_Input.scroll_changed = true;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  State.screen_ratio = (float)width / height;
}
