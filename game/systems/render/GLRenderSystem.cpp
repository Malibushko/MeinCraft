#include "GLRenderSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <magic_enum.hpp>
#include "core/components/PositionComponent.h"
#include "core/components/TransformComponent.h"
#include "game/components/camera/CameraBasisComponent.h"

#include "game/components/camera/GlobalTransformComponent.h"
#include "game/components/camera/PerspectiveCameraComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/events/CameraChangedEvent.h"
#include "game/components/lightning/DirectedLightComponent.h"
#include "game/components/lightning/LightComponent.h"
#include "game/components/physics/BoundingVolume.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/components/render/GLUnbakedMeshComponent.h"
#include "game/resources/ShaderLibrary.h"
#include "game/resources/TextureLibrary.h"
#include "game/utils/NumericUtils.h"

enum class EUniformBlock
{
  MatricesBlock,
  CameraBlock,
  LightBlock
};

//
// Construction/Destruction
//

GLRenderSystem::GLRenderSystem() = default;

GLRenderSystem::~GLRenderSystem() = default;

//
// ISystem
//

void GLRenderSystem::OnCreate(registry_t & Registry_)
{
  if (gladLoadGL() == 0)
    spdlog::critical("Failed to initialize OpenGL context");

  const auto & Display = QuerySingle<TDisplayComponent>(Registry_);

  glViewport(0, 0, static_cast<int>(Display.Width), static_cast<int>(Display.Height));
  //glEnable(GL_FRAMEBUFFER_SRGB);

  InitSolidFramebuffer(Display.Width, Display.Height);
  InitTransparentFramebuffer(Display.Width, Display.Height);
  InitShadowMap(Display.Width, Display.Height);
  InitScreenVAO();
}

void GLRenderSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  if (IsNeedUpdateFrustum(Registry_))
  {
    UpdateFrustum(QueryOrCreate<TGlobalTransformComponent>(Registry_).second);
    UpdateUniformBlocks(Registry_);
  }
  else
    UpdateLightUBO(Registry_);

  glm::vec4 ZeroFillVector(0.0f);
  glm::vec4 OneFillVector(1.0f);

  const auto RenderPass = [&](auto && Meshes, GLuint PredefinedShader = 0)
  {
    GLuint PreviousShader = 0;
    GLuint PreviousTexture = 0;

    for (auto && [Entity, Mesh, Shader, Texture, Transform] : Meshes.each())
    {


      assert(Mesh.IsBaked());

      if (!PredefinedShader && PreviousShader != Shader.ShaderID)
      {
        assert(Shader.IsValid());
        glUseProgram(Shader.ShaderID);

        PreviousShader = Shader.ShaderID;

        UpdateShaderUniformBindings(Shader.ShaderID);
      }

      glUniformMatrix4fv(
        glGetUniformLocation(PredefinedShader == 0 ? Shader.ShaderID : PredefinedShader, "u_Transform"),
        1,
        GL_FALSE,
        &Transform.Transform[0][0]
      );

      if (!PredefinedShader)
        glUniformMatrix4fv(glGetUniformLocation(Shader.ShaderID, "u_LightSpaceMatrix"), 1, GL_FALSE, &m_LightSpaceMatrix[0][0]);

      assert(Texture.IsValid());

      if (!PredefinedShader && PreviousTexture != Texture.TextureID)
      {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture.TextureID);
        glUniform1i(glGetUniformLocation(Shader.ShaderID, "u_Texture_0"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
        glUniform1i(glGetUniformLocation(Shader.ShaderID, "u_DepthMap"), 1);

        PreviousTexture = Texture.TextureID;
      }

      glBindVertexArray(Mesh.VAO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);
      glDrawElements(GL_TRIANGLES, Mesh.IndicesCount, GL_UNSIGNED_SHORT, nullptr);
    }
  };

  // Solid objects

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  auto && SolidObjects = Registry_.view<TGLSolidMeshComponent, TGLShaderComponent, TGLTextureComponent, TTransformComponent>();

  glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(m_DepthShader.ShaderID);
  glUniformMatrix4fv(glGetUniformLocation(m_DepthShader.ShaderID, "u_LightSpaceMatrix"), 1, GL_FALSE, &m_LightSpaceMatrix[0][0]);
  RenderPass(SolidObjects, m_DepthShader.ShaderID);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  glClearColor(0.52f, 0.807f, 0.92f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /*
  auto DebugShaderID = CShaderLibrary::Load("res/shaders/debug_depth_shader").ShaderID;

  glUseProgram(DebugShaderID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
  glUniform1i(glGetUniformLocation(DebugShaderID, "depthMap"), 0);
  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  */

  glClearColor(0.52f, 0.807f, 0.92f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  RenderPass(SolidObjects);

  /*
  // Transparent objects

  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunci(0, GL_ONE, GL_ONE);
  glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
  glBlendEquation(GL_FUNC_ADD);

  glBindFramebuffer(GL_FRAMEBUFFER, m_TransparentFBO);
  glClearBufferfv(GL_COLOR, 0, &ZeroFillVector[0]);
  glClearBufferfv(GL_COLOR, 1, &OneFillVector[0]);

  RenderPass(Registry_.view<TGLTranslucentMeshComponent, TGLShaderComponent, TGLTextureComponent, TTransformComponent>());

  // Draw composite image

  glDepthFunc(GL_ALWAYS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindFramebuffer(GL_FRAMEBUFFER, m_SolidFBO);

  glUseProgram(m_CompositeShader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_AccumulatorTexture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_RevealTexture);
  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Finally draw to backbuffer
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);

  // Bind backbuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glUseProgram(m_ScreenShader.ShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_SolidTexture);
  glBindVertexArray(m_ScreenQuadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  */
}

void GLRenderSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}

void GLRenderSystem::UpdateFrustum(const TGlobalTransformComponent & Transform)
{
  m_RenderFrustum       = CFrustum::FromTransform(Transform);
  m_IsNeedUpdateFrustum = false;
}

void GLRenderSystem::UpdateUniformBlocks(registry_t & Registry_)
{
  UpdateMatricesUBO(Registry_);
  UpdateCameraUBO(Registry_);
  UpdateLightUBO(Registry_);
}

bool GLRenderSystem::IsNeedUpdateFrustum(registry_t & Registry) const
{
  if (m_IsNeedUpdateFrustum)
    return true;

  return HasComponent<TCameraChangedEvent>(Registry);
}

void GLRenderSystem::UpdateMatricesUBO(registry_t & Registry_)
{
  struct TMatricesUBO
  {
    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 MVP;
  } UBO;

  static_assert(std::is_standard_layout_v<TMatricesUBO>);

  const auto & Transform = QuerySingle<TGlobalTransformComponent>(Registry_);

  UBO.Projection = Transform.Projection;
  UBO.View       = Transform.View;
  UBO.MVP        = Transform.Projection * Transform.View * Transform.Model;

  if (m_MatricesUBO == 0)
    glGenBuffers(1, &m_MatricesUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, m_MatricesUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TMatricesUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::MatricesBlock), m_MatricesUBO, 0, sizeof(TMatricesUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TMatricesUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderSystem::UpdateCameraUBO(registry_t & Registry_)
{
  struct TCameraUBO
  {
    float                                ViewDistance;
    alignas(sizeof(float) * 4) glm::vec3 Position;
    alignas(sizeof(float) * 4) glm::vec3 Direction;
  } UBO;

  static_assert(std::is_standard_layout_v<TCameraUBO>);

  const auto & [CameraBasis, Position] = QuerySingle<TCameraBasisComponent, TPositionComponent>(Registry_);
  const auto & Transform               = QuerySingle<TGlobalTransformComponent>(Registry_);

  UBO.Direction    = CameraBasis.Front;
  UBO.Position     = glm::vec3(0.f);
  UBO.ViewDistance = static_cast<float>(m_RenderFrustum.GetViewDistance());

  if (m_CameraUBO == 0)
    glGenBuffers(1, &m_CameraUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TCameraUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::CameraBlock), m_CameraUBO, 0, sizeof(TCameraUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TCameraUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderSystem::UpdateLightUBO(registry_t & Registry_)
{
  struct TLightUBO
  {
    float     DirectedLightIntensity;
    glm::vec4 DirectedLightDirection;
    glm::vec4 DirectedLightColor;
    glm::mat4 DirectedLightSpaceMatrix;
  } UBO;

  static_assert(std::is_standard_layout_v<TLightUBO>);

  auto && [DirectedLight, Light] = QuerySingle<TDirectedLightComponent, TLightComponent>(Registry_);

  const glm::mat4 LightProjection = glm::ortho(-100.f, 100.f, -100.f, 100.f, 0.1f, 250.f);
  const glm::mat4 LightView       = glm::lookAt(DirectedLight.Direction, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

  UBO.DirectedLightDirection   = glm::vec4(DirectedLight.Direction, 1.0);
  UBO.DirectedLightIntensity   = DirectedLight.Intensity;
  UBO.DirectedLightColor       = glm::vec4(Light.Ambient + Light.Diffuse + Light.Specular, 0.0);
  UBO.DirectedLightSpaceMatrix = LightProjection * LightView;
  m_LightSpaceMatrix           = UBO.DirectedLightSpaceMatrix;

  if (m_LightUBO == 0)
    glGenBuffers(1, &m_LightUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, m_LightUBO);

  glBufferData(GL_UNIFORM_BUFFER, sizeof(TLightUBO), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(EUniformBlock::LightBlock), m_LightUBO, 0, sizeof(TLightUBO));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(TLightUBO), &UBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLRenderSystem::UpdateShaderUniformBindings(GLuint ShaderID)
{
  magic_enum::enum_for_each<EUniformBlock>([&](EUniformBlock UniformBlock)
  {
    const auto & BlockName = magic_enum::enum_name(UniformBlock);
    const auto   BlockID   = glGetUniformBlockIndex(ShaderID, BlockName.data());

    if (BlockID != GL_INVALID_INDEX)
      glUniformBlockBinding(ShaderID, BlockID, static_cast<GLuint>(UniformBlock));
  });
}

void GLRenderSystem::InitSolidFramebuffer(size_t Width, size_t Height)
{
  glGenFramebuffers(1, &m_SolidFBO);

  glGenTextures(1, &m_SolidTexture);
  glBindTexture(GL_TEXTURE_2D, m_SolidTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &m_SolidDepthTexture);
  glBindTexture(GL_TEXTURE_2D, m_SolidDepthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, m_SolidFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SolidTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_SolidDepthTexture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR Opaque framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderSystem::InitTransparentFramebuffer(size_t Width, size_t Height)
{
  glGenFramebuffers(1, &m_TransparentFBO);

  glGenTextures(1, &m_AccumulatorTexture);
  glBindTexture(GL_TEXTURE_2D, m_AccumulatorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &m_RevealTexture);
  glBindTexture(GL_TEXTURE_2D, m_RevealTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Width, Height, 0, GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, m_TransparentFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulatorTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_RevealTexture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,  m_SolidDepthTexture, 0);

  const GLenum TransparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, TransparentDrawBuffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR: Transparent framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderSystem::InitScreenVAO()
{
  m_CompositeShader.ShaderID = CShaderLibrary::Load("res/shaders/composite_shader").ShaderID;
  m_ScreenShader.ShaderID    = CShaderLibrary::Load("res/shaders/screen_shader").ShaderID;

  if (!m_CompositeShader.IsValid() || !m_ScreenShader.IsValid())
    spdlog::critical("!!! ERROR: Failed to load composite or screen shader !!!");

  const float ScreenQuadVertices[] = {
    // positions		    // uv
    -1.0f, -1.0f, 0.0f,	0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
  };

  GLuint QuadVBO;

  glGenVertexArrays(1, &m_ScreenQuadVAO);
  glGenBuffers(1, &QuadVBO);

  glBindVertexArray(m_ScreenQuadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenQuadVertices), ScreenQuadVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
  glBindVertexArray(0);
}

void GLRenderSystem::InitShadowMap(size_t Width, size_t Height)
{
  glGenFramebuffers(1, &m_DepthFBO);

  glGenTextures(1, &m_DepthTexture);
  glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    spdlog::critical("!!! ERROR: Depth framebuffer is not complete !!!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_DepthShader = CShaderLibrary::Load("res/shaders/depth_shader");

  if (!m_DepthShader.IsValid())
    spdlog::error("!!! ERROR: Failed to load directional shadow shader !!!");

  UpdateShaderUniformBindings(m_DepthShader.ShaderID);
}

void GLRenderSystem::RenderSolidObjects(registry_t & Registry)
{

}

void GLRenderSystem::RenderTransparentObjects(registry_t & Registry)
{
}

void GLRenderSystem::RenderBackbuffer(registry_t & Registry)
{

}
