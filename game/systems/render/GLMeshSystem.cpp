#include "GLMeshSystem.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLUnbakedMeshComponent.h"

//
// Construction/Destruction
//

GLMeshSystem::GLMeshSystem() = default;

GLMeshSystem::~GLMeshSystem() = default;

//
// ISystem
//

void GLMeshSystem::OnCreate(registry_t & Registry_)
{
  // Empty
}

void GLMeshSystem::OnUpdate(registry_t & Registry_, float Delta_)
{
  static_assert(sizeof(glm::vec3) == 3 * sizeof(float));
  static_assert(sizeof(glm::vec2) == 2 * sizeof(float));

  for (const auto & [Entity, UnbakedMesh] : Registry_.view<TGLUnbakedMeshComponent>().each())
  {
    assert(!UnbakedMesh.Vertices.empty());

    auto & MeshComponent = Registry_.emplace<TGLMeshComponent>(Entity);

    size_t TotalSize = 0;

    TotalSize += UnbakedMesh.Vertices.size() * sizeof(UnbakedMesh.Vertices[0]);
    TotalSize += UnbakedMesh.UV.size() * sizeof(UnbakedMesh.UV[0]);
    TotalSize += UnbakedMesh.Normals.size() * sizeof(UnbakedMesh.Normals[0]);

    glGenVertexArrays(1, &MeshComponent.VAO);
    glBindVertexArray(MeshComponent.VAO);

    glGenBuffers(1, &MeshComponent.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, MeshComponent.VBO);
    glBufferData(GL_ARRAY_BUFFER, TotalSize, nullptr, GL_STATIC_DRAW);

    int       BufferOffset = 0;
    const int VerticesSize = UnbakedMesh.Vertices.size() * sizeof(UnbakedMesh.Vertices[0]);
    const int UVSize       = UnbakedMesh.UV.size() * sizeof(UnbakedMesh.UV[0]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, VerticesSize, UnbakedMesh.Vertices.data());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

#ifndef NDEBUG
    if (int Result = glGetError(); Result != GL_NO_ERROR)
      spdlog::error("glBufferSubData failed {}", Result);
#endif

    BufferOffset += VerticesSize;

    if (!UnbakedMesh.UV.empty())
    {
      assert(UnbakedMesh.UV.size() == UnbakedMesh.Vertices.size());

      glBufferSubData(GL_ARRAY_BUFFER, BufferOffset, UVSize, UnbakedMesh.UV.data());
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) VerticesSize);

#ifndef NDEBUG
      if (int Result = glGetError(); Result != GL_NO_ERROR)
        spdlog::error("glBufferSubData failed {}", Result);
#endif

      BufferOffset += UVSize;
    }

    if (!UnbakedMesh.Normals.empty())
    {
      assert(UnbakedMesh.Normals.size() == UnbakedMesh.Vertices.size());

      const int NormalsSize = UnbakedMesh.Normals.size() * sizeof(UnbakedMesh.Normals[0]);

      glBufferSubData(GL_ARRAY_BUFFER, BufferOffset, NormalsSize, UnbakedMesh.Normals.data());
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) (VerticesSize + UVSize));
      glEnableVertexAttribArray(2);

#ifndef NDEBUG
      if (int Result = glGetError(); Result != GL_NO_ERROR)
        spdlog::error("glBufferSubData failed {}", Result);
#endif

      BufferOffset += NormalsSize;
    }

    glGenBuffers(1, &MeshComponent.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshComponent.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, UnbakedMesh.Indices.size() * sizeof(UnbakedMesh.Indices[0]), UnbakedMesh.Indices.data(), GL_STATIC_DRAW);

    MeshComponent.IndicesCount = static_cast<int>(UnbakedMesh.Indices.size());

    Registry_.remove<TGLUnbakedMeshComponent>(Entity);
  }
}

void GLMeshSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
