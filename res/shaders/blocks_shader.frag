#version 460 core
#define MAX_LIGHTS 1024
#define TILE_SIZE 16
#define LIGHT_SOURCE_BRIGHT_FACTOR 2.0
#define EPS 0.0001

out vec4 FragColor;

in vec2 TextureCoords;
in vec3 Position;
in vec3 Normal;

layout(binding = 0) uniform sampler2D       u_Texture_0;
layout(binding = 1) uniform sampler2DShadow u_DepthMap;
layout(binding = 2) uniform sampler3D       u_WorldMap;

layout(location = 1) uniform unsigned int u_MaterialID;

struct TPointLight
{
  vec4  Position;
  vec4  Color;

  float Constant;
  float Linear;
  float Quadratic;
  float Radius;
};

struct TMaterial
{
  float Metallic;
  float Roughness;
  float Emissive;
  vec4  EmissiveColor;
};

layout(std430, binding = 0) readonly buffer MatricesBuffer
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
  mat4 InverseMVP;
};

layout(std430, binding = 1) readonly buffer CameraBuffer
{
  float ViewDistance;
  vec3  CameraDirection;
  vec3  CameraPosition;
};

layout(std430, binding = 2) readonly buffer DirectedLightBuffer
{
	float DirectedLightIntensity;
	vec3  DirectedLightDirection;
	vec3  DirectedLightColor;
	mat4  DirectedLightSpaceMatrix;
};

layout(std430, binding = 3) readonly buffer PointLightsBuffer
{
	TPointLight Lights[];
} LightBuffer;

layout(std430, binding = 4) readonly buffer VisiblePointLightsIndicesBuffer
{
	int Indices[];
} IndicesBuffer;

layout(std430, binding = 5) readonly buffer MaterialsBuffer
{
  TMaterial Materials[];
} Materials;

vec4 ApplyFog(in vec4 Color)
{
  const float FOG_FACTOR = 0.0001;
  const vec3  FOG_COLOR  = vec3(0.5, 0.6, 0.7);

  vec3  RayDirection = CameraPosition - Position;
  float RayLength    = length(RayDirection);

  RayDirection = normalize(RayDirection);

  float FogAmount = 5 * exp(-CameraPosition.y * FOG_FACTOR) * (1.0 - exp(-RayLength * RayDirection.y * FOG_FACTOR)) / RayDirection.y;

  return vec4(mix(Color.rgb, mix(FOG_COLOR, DirectedLightColor, 0.3 * DirectedLightIntensity), FogAmount), Color.w);
}

vec3 GetWorldPosition(vec4 FragmentPosition)
{
	vec4 NDC = vec4(
        (FragmentPosition.x / textureSize(u_DepthMap, 0).x - 0.5) * 2.0,
        (FragmentPosition.y / textureSize(u_DepthMap, 0).y - 0.5) * 2.0,
        (FragmentPosition.z - 0.5) * 2.0,
        1.0
	  );

	vec4 Clip   = InverseMVP * NDC;
    vec3 Vertex = Clip.xyz / Clip.w;

	return Vertex;
}

vec3 GetTerrainMapPosition(vec3 WorldPosition)
{
	vec3 WorldMapSize = textureSize(u_WorldMap, 0);

	WorldPosition += vec3(WorldMapSize.x / 2, 0, WorldMapSize.z / 2);

	return WorldPosition;
}

bool HasVoxelAt(vec3 WorldPosition)
{
	return texelFetch(u_WorldMap, ivec3(GetTerrainMapPosition(WorldPosition)), 0).r > 0.0;
}

bool RayCast(const vec3 ro, const vec3 rd)
{
  float t = 0.0;
  for (int i = 0; i < 100; i++)
  {
	vec3 pos = floor(ro + t * rd);
	if (HasVoxelAt(pos))
	  return true;

	t += 1;
  }

  return false;
}

float ShadowCalculation()
{
  int HitCount = 0;
  int CastCount = 0;

  for (int i = -1; i != 1; i++)
  {
	HitCount += RayCast(GetWorldPosition(gl_FragCoord + EPS * i), normalize(DirectedLightDirection)) ? 1 : 0;
	CastCount++;
  }

  return HitCount == CastCount ? 1.0 : 0.0;
}

vec4 ApplyLights(in vec4 Color)
{
  float Diffuse = max(dot(normalize(DirectedLightDirection), normalize(Normal)), 0.0);

  vec3 AmbientComponent = Color.xyz * vec3(0.1);
  vec3 DiffuseComponent = Color.xyz * Diffuse * DirectedLightColor;

  return vec4(AmbientComponent + (1.0 - ShadowCalculation()) * DiffuseComponent, 1.0);
}

// Attenuate the point light intensity
float Attenuate(vec3 LightDirection, float Radius)
{
	const float Cutoff = 0.5;

	float Attenuation = dot(LightDirection, LightDirection) / (100.0 * Radius);
	Attenuation = 1.0 / (Attenuation * 15.0 + 1.0);
	Attenuation = (Attenuation - Cutoff) / (1.0 - Cutoff);

	return clamp(Attenuation, 0.0, 1.0);
}

void main()
{
	vec4 Color = texture(u_Texture_0, TextureCoords);

	if (Color.a < 0.1)
		discard;

	ivec2 Location = ivec2(gl_FragCoord.xy);
	ivec2 TileID   = Location / ivec2(TILE_SIZE, TILE_SIZE);
	int   Index    = TileID.y * 80 + TileID.x;

	int  Offset    = Index * MAX_LIGHTS;
	vec4 Lightning = Color;

	for (int i = 0; i < MAX_LIGHTS && IndicesBuffer.Indices[i + Offset] != -1; i++)
	{
	  int         LightIndex = IndicesBuffer.Indices[i + Offset];
	  TPointLight Light      = LightBuffer.Lights[LightIndex];

	  vec3  LightDirection   = Light.Position.xyz - Position;
	  float LightDistance    = length(LightDirection);
	  float Attenuation      = Attenuate(LightDirection, Light.Radius);
	  vec3  NormalizedNormal = normalize(Normal);

	  LightDirection = normalize(LightDirection);

	  float Diffuse   = max(dot(LightDirection, NormalizedNormal), 0.0);
	  vec3 Irradiance = Color.rgb * Diffuse * Attenuation;

	  Color.rgb += Irradiance;
	}

	TMaterial Material = Materials.Materials[u_MaterialID];

	if (Material.Emissive > 0.0)
		Color += Material.EmissiveColor * Material.Emissive;
	else
		Color = ApplyLights(Color);

	Color = ApplyFog(Color);

	FragColor = Color;//HasVoxelAt(GetWorldPosition(gl_FragCoord)) ? vec4(1.0) : vec4(0.0);
}