#pragma once
#include <string>

struct TGLFWWindowComponent
{
  const char * Title = nullptr;
  const char * Icon  = nullptr;

  struct GLFWwindow * Window = nullptr;
};