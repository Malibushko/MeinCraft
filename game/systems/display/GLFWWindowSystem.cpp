#pragma once
#include "GLFWWindowSystem.h"
#include "core/entity/System.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <magic_enum.hpp>
#include <stb_image.h>
#include <glm/vec2.hpp>

#include "game/components/display/GLFWWindowComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/input/KeyboardState.h"
#include "game/components/input/MouseClickData.h"
#include "game/components/input/MouseMovementData.h"
#include "game/components/input/MouseWheelData.h"

struct TMouseMovementData;

static int KeyboardKeyToGLFWKey(EKeyboardKey Key)
{
  switch (Key)
  {
      case EKeyboardKey::KEY_SPACE : return GLFW_KEY_SPACE;
      case EKeyboardKey::KEY_APOSTROPHE : return GLFW_KEY_APOSTROPHE;
      case EKeyboardKey::KEY_COMMA : return GLFW_KEY_COMMA;
      case EKeyboardKey::KEY_MINUS : return GLFW_KEY_MINUS;
      case EKeyboardKey::KEY_PERIOD : return GLFW_KEY_PERIOD;
      case EKeyboardKey::KEY_SLASH : return GLFW_KEY_SLASH;
      case EKeyboardKey::KEY_0 : return GLFW_KEY_0;
      case EKeyboardKey::KEY_1 : return GLFW_KEY_1;
      case EKeyboardKey::KEY_2 : return GLFW_KEY_2;
      case EKeyboardKey::KEY_3 : return GLFW_KEY_3;
      case EKeyboardKey::KEY_4 : return GLFW_KEY_4;
      case EKeyboardKey::KEY_5 : return GLFW_KEY_5;
      case EKeyboardKey::KEY_6 : return GLFW_KEY_6;
      case EKeyboardKey::KEY_7 : return GLFW_KEY_7;
      case EKeyboardKey::KEY_8 : return GLFW_KEY_8;
      case EKeyboardKey::KEY_9 : return GLFW_KEY_9;
      case EKeyboardKey::KEY_SEMICOLON : return GLFW_KEY_SEMICOLON;
      case EKeyboardKey::KEY_EQUAL : return GLFW_KEY_EQUAL;
      case EKeyboardKey::KEY_A : return GLFW_KEY_A;
      case EKeyboardKey::KEY_B : return GLFW_KEY_B;
      case EKeyboardKey::KEY_C : return GLFW_KEY_C;
      case EKeyboardKey::KEY_D : return GLFW_KEY_D;
      case EKeyboardKey::KEY_E : return GLFW_KEY_E;
      case EKeyboardKey::KEY_F : return GLFW_KEY_F;
      case EKeyboardKey::KEY_G : return GLFW_KEY_G;
      case EKeyboardKey::KEY_H : return GLFW_KEY_H;
      case EKeyboardKey::KEY_I : return GLFW_KEY_I;
      case EKeyboardKey::KEY_J : return GLFW_KEY_J;
      case EKeyboardKey::KEY_K : return GLFW_KEY_K;
      case EKeyboardKey::KEY_L : return GLFW_KEY_L;
      case EKeyboardKey::KEY_M : return GLFW_KEY_M;
      case EKeyboardKey::KEY_N : return GLFW_KEY_N;
      case EKeyboardKey::KEY_O : return GLFW_KEY_O;
      case EKeyboardKey::KEY_P : return GLFW_KEY_P;
      case EKeyboardKey::KEY_Q : return GLFW_KEY_Q;
      case EKeyboardKey::KEY_R : return GLFW_KEY_R;
      case EKeyboardKey::KEY_S : return GLFW_KEY_S;
      case EKeyboardKey::KEY_T : return GLFW_KEY_T;
      case EKeyboardKey::KEY_U : return GLFW_KEY_U;
      case EKeyboardKey::KEY_V : return GLFW_KEY_V;
      case EKeyboardKey::KEY_W : return GLFW_KEY_W;
      case EKeyboardKey::KEY_X : return GLFW_KEY_X;
      case EKeyboardKey::KEY_Y : return GLFW_KEY_Y;
      case EKeyboardKey::KEY_Z : return GLFW_KEY_Z;
      case EKeyboardKey::KEY_LEFT_BRACKET : return GLFW_KEY_LEFT_BRACKET;
      case EKeyboardKey::KEY_BACKSLASH : return GLFW_KEY_BACKSLASH;
      case EKeyboardKey::KEY_RIGHT_BRACKET : return GLFW_KEY_RIGHT_BRACKET;
      case EKeyboardKey::KEY_GRAVE_ACCENT : return GLFW_KEY_GRAVE_ACCENT;
      case EKeyboardKey::KEY_WORLD_1 : return GLFW_KEY_WORLD_1;
      case EKeyboardKey::KEY_WORLD_2 : return GLFW_KEY_WORLD_2;
      case EKeyboardKey::KEY_ESCAPE : return GLFW_KEY_ESCAPE;
      case EKeyboardKey::KEY_ENTER : return GLFW_KEY_ENTER;
      case EKeyboardKey::KEY_TAB : return GLFW_KEY_TAB;
      case EKeyboardKey::KEY_BACKSPACE : return GLFW_KEY_BACKSPACE;
      case EKeyboardKey::KEY_INSERT : return GLFW_KEY_INSERT;
      case EKeyboardKey::KEY_DELETE : return GLFW_KEY_DELETE;
      case EKeyboardKey::KEY_RIGHT : return GLFW_KEY_RIGHT;
      case EKeyboardKey::KEY_LEFT : return GLFW_KEY_LEFT;
      case EKeyboardKey::KEY_DOWN : return GLFW_KEY_DOWN;
      case EKeyboardKey::KEY_UP : return GLFW_KEY_UP;
      case EKeyboardKey::KEY_PAGE_UP : return GLFW_KEY_PAGE_UP;
      case EKeyboardKey::KEY_PAGE_DOWN : return GLFW_KEY_PAGE_DOWN;
      case EKeyboardKey::KEY_HOME : return GLFW_KEY_HOME;
      case EKeyboardKey::KEY_END : return GLFW_KEY_END;
      case EKeyboardKey::KEY_CAPS_LOCK : return GLFW_KEY_CAPS_LOCK;
      case EKeyboardKey::KEY_SCROLL_LOCK : return GLFW_KEY_SCROLL_LOCK;
      case EKeyboardKey::KEY_NUM_LOCK : return GLFW_KEY_NUM_LOCK;
      case EKeyboardKey::KEY_PRINT_SCREEN : return GLFW_KEY_PRINT_SCREEN;
      case EKeyboardKey::KEY_PAUSE : return GLFW_KEY_PAUSE;
      case EKeyboardKey::KEY_F1 : return GLFW_KEY_F1;
      case EKeyboardKey::KEY_F2 : return GLFW_KEY_F2;
      case EKeyboardKey::KEY_F3 : return GLFW_KEY_F3;
      case EKeyboardKey::KEY_F4 : return GLFW_KEY_F4;
      case EKeyboardKey::KEY_F5 : return GLFW_KEY_F5;
      case EKeyboardKey::KEY_F6 : return GLFW_KEY_F6;
      case EKeyboardKey::KEY_F7 : return GLFW_KEY_F7;
      case EKeyboardKey::KEY_F8 : return GLFW_KEY_F8;
      case EKeyboardKey::KEY_F9 : return GLFW_KEY_F9;
      case EKeyboardKey::KEY_F10 : return GLFW_KEY_F10;
      case EKeyboardKey::KEY_F11 : return GLFW_KEY_F11;
      case EKeyboardKey::KEY_F12 : return GLFW_KEY_F12;
      case EKeyboardKey::KEY_F13 : return GLFW_KEY_F13;
      case EKeyboardKey::KEY_F14 : return GLFW_KEY_F14;
      case EKeyboardKey::KEY_F15 : return GLFW_KEY_F15;
      case EKeyboardKey::KEY_F16 : return GLFW_KEY_F16;
      case EKeyboardKey::KEY_F17 : return GLFW_KEY_F17;
      case EKeyboardKey::KEY_F18 : return GLFW_KEY_F18;
      case EKeyboardKey::KEY_F19 : return GLFW_KEY_F19;
      case EKeyboardKey::KEY_F20 : return GLFW_KEY_F20;
      case EKeyboardKey::KEY_F21 : return GLFW_KEY_F21;
      case EKeyboardKey::KEY_F22 : return GLFW_KEY_F22;
      case EKeyboardKey::KEY_F23 : return GLFW_KEY_F23;
      case EKeyboardKey::KEY_F24 : return GLFW_KEY_F24;
      case EKeyboardKey::KEY_F25 : return GLFW_KEY_F25;
      case EKeyboardKey::KEY_KP_0 : return GLFW_KEY_KP_0;
      case EKeyboardKey::KEY_KP_1 : return GLFW_KEY_KP_1;
      case EKeyboardKey::KEY_KP_2 : return GLFW_KEY_KP_2;
      case EKeyboardKey::KEY_KP_3 : return GLFW_KEY_KP_3;
      case EKeyboardKey::KEY_KP_4 : return GLFW_KEY_KP_4;
      case EKeyboardKey::KEY_KP_5 : return GLFW_KEY_KP_5;
      case EKeyboardKey::KEY_KP_6 : return GLFW_KEY_KP_6;
      case EKeyboardKey::KEY_KP_7 : return GLFW_KEY_KP_7;
      case EKeyboardKey::KEY_KP_8 : return GLFW_KEY_KP_8;
      case EKeyboardKey::KEY_KP_9 : return GLFW_KEY_KP_9;
      case EKeyboardKey::KEY_KP_DECIMAL : return GLFW_KEY_KP_DECIMAL;
      case EKeyboardKey::KEY_KP_DIVIDE : return GLFW_KEY_KP_DIVIDE;
      case EKeyboardKey::KEY_KP_MULTIPLY : return GLFW_KEY_KP_MULTIPLY;
      case EKeyboardKey::KEY_KP_SUBTRACT : return GLFW_KEY_KP_SUBTRACT;
      case EKeyboardKey::KEY_KP_ADD : return GLFW_KEY_KP_ADD;
      case EKeyboardKey::KEY_KP_ENTER : return GLFW_KEY_KP_ENTER;
      case EKeyboardKey::KEY_KP_EQUAL : return GLFW_KEY_KP_EQUAL;
      case EKeyboardKey::KEY_LEFT_SHIFT : return GLFW_KEY_LEFT_SHIFT;
      case EKeyboardKey::KEY_LEFT_CONTROL : return GLFW_KEY_LEFT_CONTROL;
      case EKeyboardKey::KEY_LEFT_ALT : return GLFW_KEY_LEFT_ALT;
      case EKeyboardKey::KEY_LEFT_SUPER : return GLFW_KEY_LEFT_SUPER;
      case EKeyboardKey::KEY_RIGHT_SHIFT : return GLFW_KEY_RIGHT_SHIFT;
      case EKeyboardKey::KEY_RIGHT_CONTROL : return GLFW_KEY_RIGHT_CONTROL;
      case EKeyboardKey::KEY_RIGHT_ALT : return GLFW_KEY_RIGHT_ALT;
      case EKeyboardKey::KEY_RIGHT_SUPER : return GLFW_KEY_RIGHT_SUPER;
      case EKeyboardKey::KEY_MENU : return GLFW_KEY_MENU;
  }

  assert(false && "Unknown key");
  return GLFW_KEY_UNKNOWN;
}


static EMouseButton GLFWMouseButtonToButton(int Button)
{
  switch (Button)
  {
    case GLFW_MOUSE_BUTTON_LEFT:
      return EMouseButton::Left;
    case GLFW_MOUSE_BUTTON_RIGHT:
      return EMouseButton::Right;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      return EMouseButton::Middle;
  }

  return EMouseButton::Invalid;
}

//
// Construction/Destruction
//

CGLFWWindowSystem::CGLFWWindowSystem() = default;

CGLFWWindowSystem::~CGLFWWindowSystem() = default;

//
// ISystem
//

void CGLFWWindowSystem::OnCreate(registry_t & Registry_)
{
  glfwInit();

  InitGLFWWindow(
      Registry_,
      QuerySingle<TGLFWWindowComponent>(Registry_),
      QuerySingle<TDisplayComponent>(Registry_)
    );
}

void CGLFWWindowSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  SendInput(Registry_);
}

void CGLFWWindowSystem::OnDestroy(registry_t & Registry_)
{
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

void CGLFWWindowSystem::OnFrameBegin(registry_t & Registry_)
{
  // Empty
}

void CGLFWWindowSystem::OnFrameEnd(registry_t & Registry_)
{
  Registry_.clear<TMouseMovementData>();
  Registry_.clear<TMouseWheelData>();
  Registry_.clear<TMouseClickData>();

  glfwPollEvents();
  glfwSwapBuffers(m_Window);
}

//
// Service
//

void CGLFWWindowSystem::InitGLFWWindow(registry_t & Registry, TGLFWWindowComponent & Window, const TDisplayComponent & Display)
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 1);

  Window.Window = glfwCreateWindow(
      static_cast<int>(Display.Width),
      static_cast<int>(Display.Height),
      Window.Title,
      nullptr,
      nullptr
    );

  if (const int Result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)); Result == -1)
  {
    spdlog::critical("Failed to load GLAD for GLFWWindowSystem");

    return;
  }

  assert(Window.Window != nullptr);
  m_Window = Window.Window;

  glfwMakeContextCurrent(Window.Window);
  glfwSetWindowUserPointer(Window.Window, &Registry);

  if (Window.Icon != nullptr)
  {
    GLFWimage images[1];

    images[0].pixels = stbi_load(Window.Icon, &images[0].width, &images[0].height, nullptr, 4);

    if (images[0].pixels == nullptr)
    {
      spdlog::warn("Failed to load icon: {}", Window.Icon);
    }
    else
    {
      glfwSetWindowIcon(m_Window, 1, images);

      stbi_image_free(images[0].pixels);
    }
  }

  glfwSetInputMode(Window.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetCursorPosCallback(Window.Window, [](GLFWwindow * Window_, double PositionX_, double PositionY_)
  {
    static double LastPositionX_ = PositionX_;
    static double LastPositionY_ = PositionY_;

    const auto Registry = static_cast<registry_t *>(glfwGetWindowUserPointer(Window_));

    const auto & [Entity, Mouse] = Create<TMouseMovementData>(*Registry);

    Mouse.DeltaX = PositionX_ - LastPositionX_;
    Mouse.DeltaY = LastPositionY_ - PositionY_;

    LastPositionX_ = PositionX_;
    LastPositionY_ = PositionY_;
  });

  glfwSetScrollCallback(Window.Window, [](GLFWwindow * Window_, double OffsetX_, double OffsetY_)
  {
    const auto Registry = static_cast<registry_t *>(glfwGetWindowUserPointer(Window_));

    const auto & [Entity, Mouse] = Create<TMouseWheelData>(*Registry);

    Mouse.DeltaX = OffsetX_;
    Mouse.DeltaY = OffsetY_;
  });

  glfwSetMouseButtonCallback(Window.Window, [](GLFWwindow * Window, int Button, int Action, int Modes)
  {
    const auto    Registry       = static_cast<registry_t*>(glfwGetWindowUserPointer(Window));
    const auto & [Entity, Mouse] = Create<TMouseClickData>(*Registry);

    Mouse.Button = GLFWMouseButtonToButton(Button);
    Mouse.Action = Action == GLFW_PRESS ? EMouseClickAction::Press : EMouseClickAction::Release;
  });

  glfwSetErrorCallback([](int Error_, const char * Description_)
  {
    spdlog::error("GLFW Error: {} - {}", Error_, Description_);
  });
}

void CGLFWWindowSystem::SendInput(registry_t & Registry_)
{
  auto & Keyboard = QueryOrCreate<TKeyboardState>(Registry_);

  auto KeyboardStateMapping = [](int State_)
  {
    if (State_ == GLFW_PRESS || State_ == GLFW_REPEAT)
      return EKeyState::Pressed;

    return EKeyState::Released;
  };

  magic_enum::enum_for_each<EKeyboardKey>([&](EKeyboardKey Key)
  {
    Keyboard.KeyStates[static_cast<size_t>(Key)] = KeyboardStateMapping(
        glfwGetKey(m_Window, KeyboardKeyToGLFWKey(Key))
      );
  });
}
