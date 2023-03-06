#pragma once
#include <magic_enum.hpp>
#include <glad/glad.h>

#include "core/entity/Registry.h"
#include "game/components/physics/BoundingVolume.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/render/GLShaderComponent.h"
#include "game/components/render/GLTextureComponent.h"
#include "game/systems/render/GLRenderUniformObjectsSystem.h"

void RenderMeshes(registry_t & Registry, auto && Meshes)
{
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry);

  GLuint PreviousShader = 0;
  GLuint PreviousTexture = 0;

  for (auto && [Entity, Mesh, Shader, Transform] : Meshes)
  {
    if (const TBoundingVolumeComponent * BBComponent = Registry.try_get<TBoundingVolumeComponent>(Entity))
    {
      if (!RenderData.RenderFrustum.Intersect(*BBComponent))
        continue;
    }

    assert(Mesh.IsBaked());

    if (PreviousShader != Shader.ShaderID)
    {
      assert(Shader.IsValid());
      glUseProgram(Shader.ShaderID);

      PreviousShader = Shader.ShaderID;
    }

    glUniformMatrix4fv(
      glGetUniformLocation(Shader.ShaderID, "u_Transform"),
      1,
      GL_FALSE,
      &Transform.Transform[0][0]
    );

    if (const auto * Texture = Registry.try_get<TGLTextureComponent>(Entity))
    {
      assert(Texture->IsValid());

      if (PreviousTexture != Texture->TextureID)
      {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture->TextureID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RenderData.DepthTexture);

        PreviousTexture = Texture->TextureID;
      }
    }

    glBindVertexArray(Mesh.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);
    glDrawElements(GL_TRIANGLES, Mesh.IndicesCount, GL_UNSIGNED_SHORT, nullptr);
  }
}

void RenderMeshesWithShader(registry_t & Registry, auto && Meshes, TGLShaderComponent ForceShader)
{
  auto & RenderData = QuerySingle<TGLRenderPassData>(Registry);

  GLuint PreviousTexture = 0;

  assert(ForceShader.IsValid());
  glUseProgram(ForceShader.ShaderID);

  for (auto && [Entity, Mesh, Shader, Transform] : Meshes)
  {
    if (const TBoundingVolumeComponent * BBComponent = Registry.try_get<TBoundingVolumeComponent>(Entity))
    {
      if (!RenderData.RenderFrustum.Intersect(*BBComponent))
        continue;
    }

    assert(Mesh.IsBaked());

    glUniformMatrix4fv(
      glGetUniformLocation(ForceShader.ShaderID, "u_Transform"),
      1,
      GL_FALSE,
      &Transform.Transform[0][0]
    );

    if (const auto * Texture = Registry.try_get<TGLTextureComponent>(Entity))
    {
      assert(Texture->IsValid());

      if (PreviousTexture != Texture->TextureID)
      {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture->TextureID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RenderData.DepthTexture);

        PreviousTexture = Texture->TextureID;
      }
    }

    glBindVertexArray(Mesh.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);
    glDrawElements(GL_TRIANGLES, Mesh.IndicesCount, GL_UNSIGNED_SHORT, nullptr);
  }
}

inline void BindShaderUniformBlocks(TGLShaderComponent & Shader)
{
  magic_enum::enum_for_each<EUniformBlock>([&](EUniformBlock UniformBlock)
  {
    const auto & BlockName = magic_enum::enum_name(UniformBlock);
    const auto   BlockID = glGetUniformBlockIndex(Shader.ShaderID, BlockName.data());

    if (BlockID != GL_INVALID_INDEX)
      glUniformBlockBinding(Shader.ShaderID, BlockID, static_cast<GLuint>(UniformBlock));
  });
}