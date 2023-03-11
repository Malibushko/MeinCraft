#version 460 core
layout (location = 0) in vec3 aPos;

layout(std430, binding = 0) readonly buffer MatricesBuffer
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

layout(location=0) uniform mat4 u_Transform;

void main()
{
    gl_Position = MVP * u_Transform * vec4(aPos, 1.0);
}