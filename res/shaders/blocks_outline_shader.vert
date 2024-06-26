#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;

out vec2 TextureCoord;

layout(std430, binding = 0) readonly buffer MatricesBuffer
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

layout(location=0) uniform mat4 u_Transform;
layout(location=1) uniform unsigned int u_MaterialID;

void main()
{
	gl_Position  = MVP * u_Transform * vec4(aPosition, 1.0);
	TextureCoord = aTextureCoord;
}