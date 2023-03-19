#include "GLRenderSkyboxSystem.h"

#include <spdlog/spdlog.h>

#include "core/components/TransformComponent.h"
#include "game/components/display/DisplayComponent.h"
#include "game/components/render/GLMeshComponent.h"
#include "game/components/render/GLRenderPassData.h"
#include "game/components/tags/InvisibleMeshTag.h"
#include "game/utils/GLRenderUtils.h"

//
// ISystem
//

void GLRenderSkyboxSystem::OnCreate(registry_t & Registry_)
{
}

void GLRenderSkyboxSystem::OnUpdate(registry_t & Registry_, float Delta_)
{

}

void GLRenderSkyboxSystem::OnDestroy(registry_t & Registry_)
{
  // Empty
}
