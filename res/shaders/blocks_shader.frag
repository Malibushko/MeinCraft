#version 330 core
out vec4 FragColor;

in vec2 TextureCoords;
in vec3 Position;

uniform sampler2D Texture_0;

layout(std140) uniform Matrices
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

layout(std140) uniform Camera
{
  vec3  Direction;
  vec3  CameraPosition;
  float ViewDistance;
};

const float FOG_FACTOR = 0.0001;
const vec4  FOG_COLOR  = vec4(0.5, 0.6, 0.7, 1.0);

vec4 ApplyFog(in vec4 Color)
{
  vec3 Distance = Position - CameraPosition;

  float FogAmount = 1.0 - exp(-dot(Distance, Distance) * FOG_FACTOR) * exp(-ViewDistance * FOG_FACTOR);

  return mix(Color, FOG_COLOR, FogAmount);
}

void main()
{
	FragColor = ApplyFog(texture(Texture_0, TextureCoords));
}