#version 460 core
#define MAX_LIGHTS 64
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TextureCoords;
out vec3 Position;
out vec3 Normal;
out vec4 LightFragmentPosition;

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

uniform mat4 u_Transform;

void main()
{
	gl_Position           = MVP * u_Transform * vec4(aPosition, 1.0);
	TextureCoords         = aTextureCoord;
	Normal                = mat3(transpose(inverse(u_Transform))) * aNormal;
	Position              = (u_Transform * vec4(aPosition, 1.0)).xyz;
	LightFragmentPosition = DirectedLightSpaceMatrix * u_Transform * vec4(aPosition, 1.0);
}