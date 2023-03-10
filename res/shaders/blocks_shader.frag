#version 460 core
out vec4 FragColor;

in vec2 TextureCoords;
in vec3 Position;
in vec3 Normal;
in vec4 LightFragmentPosition;

layout(binding = 0) uniform sampler2D       u_Texture_0;
layout(binding = 1) uniform sampler2DShadow u_DepthMap;

struct TPointLight
{
  vec3  Position;
  vec3  Color;

  float Constant;
  float Linear;
  float Quadratic;
  float Radius;
};

layout(std430, binding = 0) readonly buffer MatricesBuffer
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
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

const float FOG_FACTOR = 0.0001;
const vec3  FOG_COLOR  = vec3(0.5, 0.6, 0.7);

vec4 ApplyFog(in vec4 Color)
{
  vec3  RayDirection = CameraPosition - Position;
  float RayLength    = length(RayDirection);

  RayDirection = normalize(RayDirection);

  float FogAmount = 5 * exp(-CameraPosition.y * FOG_FACTOR) * (1.0 - exp(-RayLength * RayDirection.y * FOG_FACTOR)) / RayDirection.y;

  return vec4(mix(Color.rgb, mix(FOG_COLOR, DirectedLightColor, 0.3 * DirectedLightIntensity), FogAmount), Color.w);
}

float ShadowCalculation(vec4 LightSpaceFragmentPosition)
{
  vec3 LightSpaceFragmentPosition3D = LightSpaceFragmentPosition.xyz / LightSpaceFragmentPosition.w;

  LightSpaceFragmentPosition3D = LightSpaceFragmentPosition3D * 0.5f + 0.5f;

  if (LightSpaceFragmentPosition3D.z > 1.0)
  	return 1.0;

  return texture(u_DepthMap, LightSpaceFragmentPosition3D);
}

vec4 ApplyLights(in vec4 Color)
{
  float Diffuse = max(dot(normalize(DirectedLightDirection), normalize(Normal)), 0.0);

  vec3 AmbientComponent = Color.xyz * vec3(0.25);
  vec3 DiffuseComponent = Color.xyz * Diffuse * DirectedLightColor;

  float Shadow = ShadowCalculation(LightFragmentPosition);

  return vec4(AmbientComponent + (Shadow * DiffuseComponent), 1.0);
}

void main()
{
	vec4 Color = texture(u_Texture_0, TextureCoords);

	if (Color.a < 0.1)
		discard;

	ivec2 Location = ivec2(gl_FragCoord.xy);
	ivec2 TileID   = Location / ivec2(16, 16);
	int   Index    = TileID.y * 80 + TileID.x;

	vec3 ViewDirection = normalize(CameraPosition - Position);
	int  Offset        = Index * 1024;

	Color = ApplyLights(Color);
	Color = ApplyFog(Color);

	FragColor = Color;
}