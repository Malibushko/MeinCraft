#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoord;

out vec2 TextureCoords;

uniform mat4 u_Transform;

void main()
{
	gl_Position   = u_Transform * vec4(aPosition, 1.0);
	TextureCoords = aTextureCoord;
}