#version 420 core
layout (location = 0) in vec3 aPos;

layout(std140, binding=0) uniform MatricesBlock
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

uniform mat4 u_Transform;

void main()
{
    gl_Position = MVP  * u_Transform * vec4(aPos, 1.0);
}