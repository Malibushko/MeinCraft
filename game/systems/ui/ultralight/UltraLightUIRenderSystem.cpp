#include "UltraLightUIRenderSystem.h"

#include <Ultralight/Ultralight.h>
#include <Ultralight/platform/Logger.h>
#include <AppCore/Platform.h>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"
#include "game/components/ui/ultralight/RendererComponent.h"
#include "game/components/ui/ultralight/ViewComponent.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"

//
// Construction/Destruction
//

CUltraLightUIRenderSystem::CUltraLightUIRenderSystem() = default;

CUltraLightUIRenderSystem::~CUltraLightUIRenderSystem() = default;

//
// ISystem
//

void CUltraLightUIRenderSystem::OnCreate(registry_t & Registry_)
{
  if (gladLoadGL() == 0)
    spdlog::critical("Failed to initialize OpenGL context");

  const auto & Display  = QuerySingle<TDisplayComponent>(Registry_);
  auto       & Renderer = QueryOrCreate<TUltraLightRendererComponent>(Registry_);
  auto       & View     = QueryOrCreate<TUltraLightViewComponent>(Registry_);

  ultralight::Config Config;

  Config.resource_path    = "./res/";
  Config.use_gpu_renderer = false;
  Config.device_scale     = 1.0;

  ultralight::Platform::instance().set_config(Config);
  ultralight::Platform::instance().set_font_loader(ultralight::GetPlatformFontLoader());
  ultralight::Platform::instance().set_file_system(ultralight::GetPlatformFileSystem("."));

  m_SurfaceFactory.reset(new UltraLightGLTextureSurfaceFactory());
  ultralight::Platform::instance().set_surface_factory(m_SurfaceFactory.get());

  Renderer.Renderer = ultralight::Renderer::Create();
  View.View         = Renderer.Renderer->CreateView(static_cast<uint32_t>(Display.Width), static_cast<uint32_t>(Display.Height), true, nullptr, true);

  InitGLUtils();
}

void CUltraLightUIRenderSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  auto & Renderer = QuerySingle<TUltraLightRendererComponent>(Registry_);
  auto & View     = QuerySingle<TUltraLightViewComponent>(Registry_);
  auto & Display  = QuerySingle<TDisplayComponent>(Registry_);

  Renderer.Renderer->Update();
  Renderer.Renderer->Render();

  UltraLightGLTextureSurface * Surface = reinterpret_cast<UltraLightGLTextureSurface*>(View.View->surface());

  if (!Surface)
    return;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glUseProgram(m_ScreenQuadShader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Surface->GetTextureAndSyncIfNeeded());
  glBindVertexArray(m_ScreenVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CUltraLightUIRenderSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void CUltraLightUIRenderSystem::InitGLUtils()
{
  m_ScreenQuadShader = CShaderLibrary::Load("res/shaders/screen_shader");

  if (!m_ScreenQuadShader.IsValid())
    spdlog::critical("[UI] !!! ERROR: Failed to load composite or screen shader !!!");

  const float ScreenQuadVertices[] = {
    // positions		    // uv
    -1.0f, -1.0f, 0.0f,	0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 0.0f,

     1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f
  };

  GLuint QuadVBO;

  glGenVertexArrays(1, &m_ScreenVAO);
  glGenBuffers(1, &QuadVBO);

  glBindVertexArray(m_ScreenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuadVertices), ScreenQuadVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
  glBindVertexArray(0);
}
