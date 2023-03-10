#version 460 core

in vec2 TextureCoords;

layout (location = 0) out vec4 FragmentColor;

layout(binding = 0) uniform sampler2D ScreenTexture;

void main()
{
	FragmentColor = texture(ScreenTexture, TextureCoords);
}