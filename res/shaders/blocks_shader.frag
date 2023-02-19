#version 460 core
out vec4 FragColor;

in vec2 TextureCoords;
in vec3 Position;
in vec3 Normal;
in vec4 LightFragmentPosition;

layout(binding = 0) uniform sampler2D       u_Texture_0;
layout(binding = 1) uniform sampler2DShadow u_DepthMap;

layout(std140) uniform MatricesBlock
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

layout(std140) uniform CameraBlock
{
  float ViewDistance;
  vec3  Direction;
  vec3  CameraPosition;
};

layout(std140) uniform LightBlock
{
	float DirectedLightIntensity;
	vec3  DirectedLightDirection;
	vec3  DirectedLightColor;
	mat4  DirectedLightSpaceMatrix;
};

const float FOG_FACTOR = 0.0001;
const vec4  FOG_COLOR  = vec4(0.5, 0.6, 0.7, 1.0);

vec4 ApplyFog(in vec4 Color)
{
  vec3 Distance = Position - CameraPosition;

  float FogAmount = 1.0 - exp(-dot(Distance, Distance) * FOG_FACTOR);

  return mix(Color, FOG_COLOR, FogAmount);
}

float ShadowCalculation(vec4 LightSpaceFragmentPosition)
{
	vec3 LightSpaceFragmentPosition3D = LightSpaceFragmentPosition.xyz / LightSpaceFragmentPosition.w;

	LightSpaceFragmentPosition3D = LightSpaceFragmentPosition3D * 0.5f + 0.5f;

	float Bias   = max(0.05 * (1.0 - dot(Normal, normalize(DirectedLightDirection.xyz))), 0.005);
	float Shadow = 0.0;

	for (int x = -1; x <= 1; ++x)
		for (int y = -1; y <= 1; ++y)
			Shadow += texture(u_DepthMap, vec3(LightSpaceFragmentPosition3D.xy, LightSpaceFragmentPosition3D.z - Bias));

	return Shadow / 18.0;
}

vec4 ApplyDirectedLight(in vec4 Color)
{
  // TODO: refactor
  vec3  Ambient = Color.xyz * 0.25;
  float Diffuse = max(dot(Normal, normalize(DirectedLightDirection.xyz)), 0.0);
  float Shadow = ShadowCalculation(LightFragmentPosition);

  return vec4(Ambient + (1.0 - Shadow) * Color.xyz * (Diffuse * DirectedLightColor.rgb) * DirectedLightIntensity, Color.w);
}

void main()
{
	vec4 Color = texture(u_Texture_0, TextureCoords);

	if (Color.a < 0.1)
		discard;

	Color = ApplyDirectedLight(Color);
	Color = ApplyFog(Color);

	FragColor = Color;
}