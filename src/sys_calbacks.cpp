#include "sys_callbacks.h"

void ErrorCallback(int error, const char* description) 
{
  fprintf(stderr, "ERROR: GLFW: %s\n", description);
}
