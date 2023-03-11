#include "GLRenderLightAccumulationPassSystem.h"

#include <spdlog/spdlog.h>

#include "GLRenderTransparentPassSystem.h"
#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/tags/InvisibleMeshTag.h"
#include "game/resources/ShaderLibrary.h"
#include "game/utils/GLRenderUtils.h"

//
// Based on https://takahiroharada.files.wordpress.com/2015/04/forward_plus.pdf
//

//
// ISystem
//

void GLRenderLightAccumulationPassSystem::OnCreate(registry_t & Registry_)
{
  auto & RenderData = QueryOrCreate<TGLRenderPassData>(Registry_);
  auto & Display    = QuerySingle<TDisplayComponent>(Registry_);

  glGenFramebuffers(1, &m_FBO);

  glGenTextures(1, &RenderData.DepthTexture);
  glBindTexture(GL_TEXTURE_2D, RenderData.DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<GLsizei>(Display.Width), static_cast<GLsizei>(Display.Height), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	GLfloat BorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, RenderData.DepthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER); Status != GL_FRAMEBUFFER_COMPLETE)
		spdlog::critical("!!! ERROR: LightCulling framebuffer is not complete {}!!!", Status);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_DepthShader        = CShaderLibrary::Load("res/shaders/depth_shader");
  m_LightCullingShader = CShaderLibrary::Load("res/shaders/forward_light_culling.comp");

	if (!m_DepthShader.IsValid())
		spdlog::error("Light accumulation system unable to create depth shader");

	if (!m_LightCullingShader.IsValid())
    spdlog::error("Light accumulation system unable to create light culling shader");
}

void GLRenderLightAccumulationPassSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	const auto & RenderData = QuerySingle<TGLRenderPassData>(Registry_);

  glClear(GL_DEPTH_BUFFER_BIT);

	RenderMeshesWithShader(
			Registry_,
			Registry_.view<TGLSolidMeshComponent, TGLShaderComponent, TTransformComponent>().each(),
			m_DepthShader
		);

	RenderMeshesWithShader(
		  Registry_,
		  Registry_.view<TGLTranslucentMeshComponent, TGLShaderComponent, TTransformComponent>().each(),
		  m_DepthShader
	  );

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(m_LightCullingShader.ShaderID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RenderData.DepthTexture);

	glDispatchCompute(RenderData.LightCullingWorkGroupsX, RenderData.LightCullingWorkGroupsY, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderLightAccumulationPassSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
