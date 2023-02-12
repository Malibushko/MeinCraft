#version 330 core
out vec4 FragColor;

in vec2 TextureCoords;
in vec3 Position;
in vec3 Normal;

uniform sampler2D Texture_0;

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
};

const float FOG_FACTOR = 0.0001;
const vec4  FOG_COLOR  = vec4(0.5, 0.6, 0.7, 1.0);

vec4 ApplyFog(in vec4 Color)
{
  vec3 Distance = Position - CameraPosition;

  float FogAmount = 1.0 - exp(-dot(Distance, Distance) * FOG_FACTOR);

  return mix(Color, FOG_COLOR, FogAmount);
}

vec4 ApplyDirectedLight(in vec4 Color)
{
  // TODO: refactor
  vec4  Ambient = Color * 0.25;
  float Diffuse = max(dot(Normal, normalize(-DirectedLightDirection)), 0.0);

  return Ambient + Color * (Diffuse * vec4(DirectedLightColor, 1.0)) * DirectedLightIntensity;
}

void main()
{
	vec4 Color = texture(Texture_0, TextureCoords);

	Color = ApplyDirectedLight(Color);
	Color = ApplyFog(Color);

	FragColor = Color;
}