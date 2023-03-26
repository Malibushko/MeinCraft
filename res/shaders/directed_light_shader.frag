#version 460 core
out vec4 FragColor;

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

void main()
{
    FragColor = vec4(DirectedLightColor * DirectedLightIntensity, 1.0);
}