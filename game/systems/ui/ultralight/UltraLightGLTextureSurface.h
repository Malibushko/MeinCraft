#pragma once
#include <Ultralight/platform/Surface.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class UltraLightGLTextureSurface : public ultralight::Surface
{
public:
  virtual ~UltraLightGLTextureSurface()
  {
  }

  virtual GLuint GetTextureAndSyncIfNeeded() = 0;
};

class UltraLightGLTextureSurfaceFactory : public ultralight::SurfaceFactory
{
public:
  UltraLightGLTextureSurfaceFactory();

  virtual ~UltraLightGLTextureSurfaceFactory();

  virtual ultralight::Surface * CreateSurface(uint32_t width, uint32_t height) override;

  virtual void DestroySurface(ultralight::Surface * surface) override;
};