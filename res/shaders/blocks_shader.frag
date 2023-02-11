#version 330 core
out vec4 FragColor;

in vec2 TextureCoords;

uniform sampler2D Texture_0;

void main()
{
	FragColor = texture(Texture_0, TextureCoords);
}