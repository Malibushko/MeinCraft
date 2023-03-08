#version 460 core
#define MAX_LIGHTS 64

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

layout(std140, binding=0) uniform MatricesBlock
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

layout(std140, binding=1) uniform CameraBlock
{
  float ViewDistance;
  vec3  CameraDirection;
  vec3  CameraPosition;
};

layout(std140, binding=2) uniform LightBlock
{
	float DirectedLightIntensity;
	vec3  DirectedLightDirection;
	vec3  DirectedLightColor;
	mat4  DirectedLightSpaceMatrix;

	TPointLight PointLights[MAX_LIGHTS];
};

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

	vec3 Lightning     = Color.xyz;
	vec3 ViewDirection = normalize(CameraDirection - Position);

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
	  float Distance = length(PointLights[i].Position - Position);

	  if (Distance < PointLights[i].Radius)
	  {
		vec3 LightDirection = normalize(PointLights[i].Position - Position);
	    float Diffuse       = max(dot(Normal, LightDirection), 0.0);
	    float Attenuation   = 1.0 / (PointLights[i].Constant + PointLights[i].Linear * Distance + PointLights[i].Quadratic * (Distance * Distance));
	    float Intensity     = PointLights[i].Radius / Distance;

	    Lightning += Diffuse * PointLights[i].Color * Attenuation * Intensity;
	  }
	}

	Color = ApplyLights(vec4(Lightning, 1.0));
	Color = ApplyFog(Color);

	FragColor = Color;
}