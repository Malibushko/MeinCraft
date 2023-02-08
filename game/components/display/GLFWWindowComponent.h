#pragma once
#include <string>

struct TGLFWWindowComponent
{
  const char * Title;

  struct GLFWwindow * Window = nullptr;
};