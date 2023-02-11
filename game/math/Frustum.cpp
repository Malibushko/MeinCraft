#include "Frustum.h"

//
// Construction/Destruction
//

CFrustum::CFrustum() :
	m_Planes()
{
	// Empty
}

CFrustum CFrustum::FromTransform(const TGlobalTransformComponent & Transform)
{
  CFrustum Result;

  const glm::mat4 MVP = Transform.Projection * Transform.View;

	// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf

	for (int i = A; i <= D; i++) Result.m_Planes[Left][i]  = MVP[i][3] + MVP[i][0];
	for (int i = A; i <= D; i++) Result.m_Planes[Right][i] = MVP[i][3] - MVP[i][0];
	for (int i = A; i <= D; i++) Result.m_Planes[Top][i]   = MVP[i][3] - MVP[i][1];
	for (int i = A; i <= D; i++) Result.m_Planes[Bottom][i]= MVP[i][3] + MVP[i][1];
	for (int i = A; i <= D; i++) Result.m_Planes[Front][i] = MVP[i][3] + MVP[i][2];
	for (int i = A; i <= D; i++) Result.m_Planes[Back][i]  = MVP[i][3] - MVP[i][2];

	for (const EPlane Plane : { Left, Right, Top, Bottom, Front, Back })
		Result.Normalize(Plane);

	return Result;
}

//
// Interface
//

bool CFrustum::Intersect(const TBoundingVolumeComponent & Volume) const
{
	bool Result = false;

	std::visit([&](auto && Volume_)
	{
		Result = Intersect(Volume_);
	}, Volume.Volume);

	return Result;
}

bool CFrustum::Intersect(const TAABBVolumeComponent & Volume) const
{
	// TODO: optimize https://gist.github.com/Kinwailo/d9a07f98d8511206182e50acda4fbc9b

	// check box outside/inside of frustum
	for (int i = 0; i < 6; i++)
	{
		int Outside = 0;

		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Min.x, Volume.Min.y, Volume.Min.z, 1.0f)) < 0.f) ? 1 : 0;
		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Max.x, Volume.Min.y, Volume.Min.z, 1.0f)) < 0.f) ? 1 : 0;
		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Min.x, Volume.Max.y, Volume.Min.z, 1.0f)) < 0.f) ? 1 : 0;
		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Max.x, Volume.Max.y, Volume.Min.z, 1.0f)) < 0.f) ? 1 : 0;
		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Min.x, Volume.Min.y, Volume.Max.z, 1.0f)) < 0.f) ? 1 : 0;
		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Max.x, Volume.Min.y, Volume.Max.z, 1.0f)) < 0.f) ? 1 : 0;
		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Min.x, Volume.Max.y, Volume.Max.z, 1.0f)) < 0.f) ? 1 : 0;
		Outside += (glm::dot(m_Planes[i], glm::vec4(Volume.Max.x, Volume.Max.y, Volume.Max.z, 1.0f)) < 0.f) ? 1 : 0;

		if (Outside == 8)
			return false;
	}

	return true;
}

bool CFrustum::Intersect(...) const
{
	return false;
}

//
// Service
//

void CFrustum::Normalize(EPlane Plane)
{
	const float Magnitude = glm::sqrt(
		m_Planes[Plane][A] * m_Planes[Plane][A] +
		m_Planes[Plane][B] * m_Planes[Plane][B] +
		m_Planes[Plane][C] * m_Planes[Plane][C]
	);

	m_Planes[Plane] /= Magnitude;
}