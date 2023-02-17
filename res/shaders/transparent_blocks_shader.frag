#version 460 core
layout (location = 0) out vec4  Accumulator;
layout (location = 1) out float Reveal;

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
  vec3  Ambient = Color.xyz * 0.25;
  float Diffuse = max(dot(Normal, normalize(-DirectedLightDirection)), 0.0);

  return vec4(Ambient + Color.x * (Diffuse * DirectedLightColor) * DirectedLightIntensity, Color.w);
}

void main()
{
	vec4 Color = texture(Texture_0, TextureCoords);

	Color = ApplyDirectedLight(Color);
	Color = ApplyFog(Color);

	// OIT stuff

	float Weight = clamp(pow(min(1.0, Color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

	// store pixel color accumulation
	Accumulator = vec4(Color.rgb * Color.a, Color.a) * Weight;

	// store pixel revealage threshold
	Reveal = Color.a;
}