#version 460 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;

layout(std140, binding=0) uniform MatricesBlock
{
  mat4 Projection;
  mat4 View;
  mat4 MVP;
};

uniform mat4 u_Transform;

out vec2 TextureCoords;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	gl_Position   = MVP * u_Transform * vec4(aPosition, 1.f);
	TextureCoords = aTextureCoord;
	Normal        = mat3(transpose(inverse(u_Transform))) * aNormal;
	FragPos       = vec3(u_Transform * vec4(aPosition, 1.0));
}