#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;

layout(std140, binding=0) uniform MatricesBlock
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

layout(std140, binding=2) uniform LightBlock
{
	float DirectedLightIntensity;
	vec3  DirectedLightDirection;
	vec3  DirectedLightColor;
	mat4  DirectedLightSpaceMatrix;
};

uniform mat4 u_Transform;

out vec2 TextureCoords;

void main()
{
	gl_Position = DirectedLightSpaceMatrix * u_Transform * vec4(aPosition, 1.0f);

	TextureCoords = aTextureCoord;
}