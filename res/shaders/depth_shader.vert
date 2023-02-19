#version 460 core
layout (location = 0) in vec3 aPosition;

layout(std140, binding=0) uniform MatricesBlock
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

layout(std140, binding=2) uniform LightBlock
{
	float DirectedLightIntensity;
	vec4  DirectedLightDirection;
	vec4  DirectedLightColor;
	mat4  DirectedLightSpaceMatrix;
};

uniform mat4 u_Transform;
uniform mat4 u_LightSpaceMatrix;

void main()
{
	gl_Position = u_LightSpaceMatrix * u_Transform * vec4(aPosition, 1.0f);
}