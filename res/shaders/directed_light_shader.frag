#version 460 core
out vec4 FragColor;

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
};

void main()
{
    FragColor = vec4(DirectedLightColor * DirectedLightIntensity, 1.0);
}